/* The Clear BSD License
 *
 * Copyright (c) 2025 EdgeImpulse Inc.
 * All rights reserved.
 */

#define LIGHT_INTERVAL 10000
//#include "ei_at_handlers.h"
#include "ei_device_thingy53.h"
#include "ble/ei_ble_com.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "inference/ei_run_impulse.h"
#include "ble/ble_nus.h"

/* Edge Impulse sensors */
#include "sensors/ei_environment_sensor.h"
#include "sensors/ei_light_sensor.h"

/* Zephyr */
#include <zephyr/kernel.h>
#include <zephyr/drivers/uart.h>
#include <zephyr/logging/log.h>
#include <nrfx_clock.h>
#include <zephyr/bluetooth/bluetooth.h>

//accesing light sensor BH1749
#include <zephyr/device.h>
#include <zephyr/drivers/sensor.h>

/* edge impulse model */
#include "ei-model/edge-impulse-sdk/classifier/ei_run_classifier.h"
#include "ei-model/edge-impulse-sdk/classifier/ei_print_results.h"

//threshold caluclation
#include <math.h>
#define LUX_THRESHOLD 5.0f


/*  Custom GATT (from custom_gatt.cpp) */
extern "C" void custom_gatt_init(void);
extern "C" void custom_gatt_notify_environment(void);
extern "C" void custom_gatt_notify_light(void);
extern "C" float get_last_lux_value(void); //to fill in buffer r=from sensor readinsg

extern "C" float get_avg_rtt(void);
extern "C" uint32_t get_rtt_count(void);

/* LIGHT METHODS */
extern "C" float read_light_sensor();
extern "C" void send_light_ble();


#define LOG_MODULE_NAME main
LOG_MODULE_REGISTER(LOG_MODULE_NAME);

/* UART for AT / logging */
const struct device *uart;

/* Faster than printf */
void ei_putchar(char c)
{
    uart_fifo_fill(uart, (const uint8_t *)&c, 1);
}



// Callback function declaration
static int get_signal_data(size_t offset, size_t length, float *out_ptr);


//innitializing buffer
#define WINDOW_SIZE EI_CLASSIFIER_DSP_INPUT_FRAME_SIZE //25
static float input_buf[WINDOW_SIZE];
static int window_index = 0;
static bool window_full = false;

//function to initialize the buffer at the start by filling the 1st 25 readings
void init_fill_buffer(void) {
    int i = 0;
    uint32_t last_light = 0;
    //loop to fill buffer 25 vals
    while (i < WINDOW_SIZE) {
        uint32_t now = k_uptime_get_32();

        if (now - last_light > LIGHT_INTERVAL) {

            last_light = now;

            //Send sensor reading
            //custom_gatt_notify_light();
            float light_reading = read_light_sensor();
            //get_last_lux_value();
            //= read_light_sensor();

            send_light_ble(); //while filling buffer we send to rpi

            //Fill buffer
            input_buf[i] = light_reading;
            window_index++;
            i++;

            LOG_INF("New Lux reading to buffer %d : %.2f", i, light_reading);

            //RTT summary
            float current_avg = get_avg_rtt();
            uint32_t n = get_rtt_count();

            printk("Buffer fill [%d/%d] | Cumulative Avg RTT = %.2f ms (n=%u)\n", i, WINDOW_SIZE, current_avg, n + 1);
        }
        //small deleya to allow BLE stack to run
        k_sleep(K_MSEC(10));
    }

    window_full = true;
    float final_avg = get_avg_rtt();

    LOG_INF("Buffer initialization complete, ready for inference!");
    LOG_INF("Final Cumulative Avg RTT = %.2f ms", final_avg);
}

//function to update the buffer with prediction or reading while running the inference after filling the buffer
void update_buffer(float data) {

    for (int i = 0; i < WINDOW_SIZE - 1; i++) {
        input_buf[i] = input_buf[i + 1];
    }
    //appened data at end of buffer
    input_buf[WINDOW_SIZE - 1] = data;
}



//main
int main(void) {
    //nordic stuff 
    LOG_INF("System start");

    EiDeviceThingy53 *dev =
        static_cast<EiDeviceThingy53 *>(EiDeviceInfo::get_device());

    uint8_t rcv_char;
    //ATServer *at;

    /* Unbuffered stdout */
    setvbuf(stdout, NULL, _IONBF, 0);

    /* Switch CPU core clock to 128 MHz */
    // nrfx_clock_divider_set(NRF_CLOCK_DOMAIN_HFCLK,
    //                        NRF_CLOCK_HFCLK_DIV_1);

//uart init
    uart = device_get_binding("CDC_ACM_0");
    if (!uart) {
        LOG_ERR("Failed to init CDC_ACM_0");
    }

    /*    SENSOR INIT    */
    if (!ei_lightsensor_init()) {
        LOG_ERR("Light sensor init failed");
    }

    if (!ei_environment_init()) {
        LOG_ERR("Environment sensor init failed");
    }

//BLE init
    /* Edge Impulse BLE*/
    ei_ble_com_init();

    /* Enable Zephyr BLE */
    bt_enable(NULL);

    /* Init custom GATT service */
    custom_gatt_init();

    /* BLE stack must be up before this */
    dev->init_device_id();

    dev->set_state(eiStateFinished);

    /* UART greeting only (not BLE) */
    ei_printf("Thingy:53 ready (Custom GATT enabled)\r\n");

    k_sleep(K_MSEC(500));



// model running part 
    //edge impulse model declerations
    signal_t signal;            // Wrapper for raw input buffer
    ei_impulse_result_t result; // Used to store inference output
    EI_IMPULSE_ERROR res;       // Return code from inference

   
    
// inits for the main loop 
    uint32_t last_tx = 0;
    uint32_t last_light = 0;
    uint32_t last_env   = 0;

    LOG_INF("Starting buffer initialization...");
    init_fill_buffer();
    //captures current time to prepare b4 the loop
    last_light = k_uptime_get_32();

    //small delay equivilant to avg rrt such that rpi and nordic inference at the same time!
    k_sleep(K_MSEC(get_avg_rtt()));
    float prediction;
    // = result.classification[0].value;
    // printk("Regression value: %.2f\n", prediction);
    
    int i = 25; ///buffer number if inputs counter
    bool inference_done = false;
    float light_reading = input_buf[WINDOW_SIZE - 1];
    int TXCounter = 0;

//main loop
    while (1){
        uint32_t now = k_uptime_get_32();
        /* Light @ 1 Hz */
        if (now - last_light > LIGHT_INTERVAL) {
            last_light = now;
            //custom_gatt_notify_light();
            //fill buffer with sensor readign every 30 secs
            light_reading = read_light_sensor();
            //update_buffer(light_reading);
            i++;
            LOG_INF("New Lux reading %d: %.2f", i, light_reading);

            inference_done = false;
        }

        if (window_full && !inference_done) {
            //k_sleep(K_MSEC(get_avg_rtt()));
            signal.total_length = WINDOW_SIZE;
            signal.get_data = &get_signal_data;
            EI_IMPULSE_ERROR res = run_classifier(&signal, &result, false);

            // If inference work
            if (res == EI_IMPULSE_OK) { 
                prediction = result.classification[0].value;
                printk("Regression value: %.2f\n", prediction);
                //printk("Value after multiplication: %.2f\n", prediction * scaleFactor);
            } else {
                LOG_ERR("Classifier failed!!: %d\n", res);
            }

            //checking if predicted and actual are within a threshold
            float diff = fabs(light_reading - prediction);
            printk("Real %.2f | Pred %.2f | Diff %.2f\n", light_reading, prediction, diff);

            if (diff > LUX_THRESHOLD) {
                printk("Threshold exceeded -> send REAL value\n");
                //custom_gatt_notify_light();// BLE send
                send_light_ble();
                update_buffer(light_reading);
                TXCounter++;
                printk("Number of Re-Transmissions: %d\n", TXCounter);
            } else {
                printk("Within threshold -> suppress TX\n");
                printk("Number of Re-Transmissions: %d\n", TXCounter);
                update_buffer(prediction);  // no BLE send
            }

            inference_done = true;
        }
        // /* Environment @ 0.5 Hz */
        // if (now - last_env > 2000) {
        //     last_env = now;
        //     custom_gatt_notify_environment();
        // }

        k_sleep(K_MSEC(10));
        
    }

    //return 0;
}


// Callback: fill a section of the out_ptr buffer when requested
static int get_signal_data(size_t offset, size_t length, float *out_ptr) {
    for (size_t i = 0; i < length; i++) {
        out_ptr[i] = input_buf[offset + i];
    }
    return EIDSP_OK;
}