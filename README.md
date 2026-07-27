# Adaptive Dual Prediction for IoT

## Overview

This project presents an adaptive dual prediction framework for Internet of Things (IoT) applications using Nordic Thingy:53 and Raspberry Pi devices. The objective is to reduce unnecessary sensor data transmissions while maintaining accurate monitoring of environmental conditions.

Traditional IoT systems transmit every sensor reading to a receiver, which increases communication overhead, bandwidth usage, and energy consumption. To address this challenge, this project implements a Dual Prediction Scheme (DPS), where prediction models are maintained at both the sensor node and receiver. Data is transmitted only when the prediction error exceeds a predefined threshold.

The proposed architecture improves communication efficiency and reduces energy consumption, making it suitable for resource-constrained IoT deployments.

---

## Problem Statement

IoT devices continuously generate sensor data that is often transmitted regardless of whether the information provides significant value.

This leads to:

- Increased network traffic
- Higher energy consumption
- Reduced device lifetime
- Limited scalability

The goal of this project is to develop an intelligent transmission framework capable of reducing communication while preserving data accuracy through machine learning-based prediction.

---

## Objectives

- Reduce the number of sensor transmissions
- Improve energy efficiency
- Maintain prediction accuracy
- Implement an adaptive threshold-based communication mechanism
- Evaluate different machine learning models for time-series prediction
- Develop a practical edge computing solution using Nordic Thingy:53 and Raspberry Pi

---

## System Architecture

The overall architecture of the proposed system is shown below.

images/system_model.png

### Components

- **Nordic Thingy:53** for environmental sensing
- **Machine Learning Prediction Model**
- **Adaptive Threshold Evaluation Module**
- **Wireless Communication Layer**
- **Raspberry Pi Edge Receiver**
- **Prediction Synchronization Mechanism**

### Workflow

1. Sensor data is collected by the Nordic Thingy:53.
2. A prediction model estimates the next sensor value.
3. The predicted value is compared with the actual sensor reading.
4. If the error remains within the threshold, transmission is suppressed.
5. If the error exceeds the threshold, the actual value is transmitted.
6. The receiver updates and synchronizes its prediction state.
7. Performance metrics are recorded and analyzed.

---

## Machine Learning Architecture

The machine learning workflow used for prediction is illustrated below.

images/ML_Architecture_senior_FINAL_MODEL.png

The solution performs:

- Data collection
- Data preprocessing
- Feature engineering
- Model training
- Model evaluation
- Prediction generation
- Threshold-based transmission control

---

## Implemented Models

Multiple machine learning approaches were explored and evaluated during development.

### LightGBM Model

images/LGBM.png

LightGBM was evaluated due to its:

- Fast training performance
- Low memory requirements
- Strong predictive capability
- Suitability for edge computing environments

### Neural Network Model

images/NeuralNetwork.png

Neural networks were investigated to capture more complex temporal relationships in sensor data.

### Final Prediction Model

images/Final_Outdoor_Model.png

The final model selected for deployment demonstrated the best balance between:

- Prediction accuracy
- Computational efficiency
- Transmission reduction performance

---

## Adaptive Threshold Mechanism

The adaptive threshold mechanism is a key component of the Dual Prediction Scheme.

images/Threshold_values.png

### Transmission Decision Logic

**If:**

```text
Prediction Error ≤ Threshold
```

No data transmission occurs.

**If:**

```text
Prediction Error > Threshold
```

Actual sensor data is transmitted to the Raspberry Pi to re-establish synchronization.

This strategy significantly reduces unnecessary communication while maintaining data quality.

---

## Experimental Evaluation

A series of experiments were conducted to evaluate the effectiveness of the adaptive dual prediction framework.

### Evaluation Metrics

- Number of Transmissions
- Energy Consumption
- Prediction Accuracy
- Communication Reduction
- Synchronization Performance
- System Efficiency

---

## Transmission Analysis

The chart below illustrates the effect of the dual prediction strategy on communication frequency.

images/chart_numofTX.png

### Observations

- Significant reduction in sensor transmissions
- Lower communication overhead
- Improved bandwidth utilization
- Greater efficiency compared to traditional approaches

---

## Energy Consumption Analysis

### Dual Prediction Scheme (DPS)

images/energy10mDPS.png

### Traditional Transmission Approach

images/energy10mTrad.png

### Comparison

The Dual Prediction Scheme demonstrates improved energy efficiency by reducing the frequency of wireless transmissions.

Benefits include:

- Lower power consumption
- Extended battery life
- Reduced communication cost
- Improved long-term deployment sustainability

---

## Repository Structure

```text
adaptive-dual-prediction-iot/
│
├── images/
│   ├── chart_numofTX
│   ├── energy10mDPS
│   ├── energy10mTrad
│   ├── Final_Outdoor_Model
│   ├── LGBM
│   ├── ML_Architecture_senior_FINAL_MODEL
│   ├── NeuralNetwork
│   ├── system_model
│   └── Threshold_values
│
├── models/
│   ├── trained_models
│   ├── evaluation_scripts
│   └── prediction_models
│
├── thingy53/
│   ├── sensor_data
│   ├── deployment_files
│   └── preprocessing
│
└── README.md
```

---

## Technologies Used

### Hardware

- Nordic Thingy:53
- Raspberry Pi

### Software

- Python
- Machine Learning
- LightGBM
- Neural Networks

### Domains

- Internet of Things (IoT)
- Edge Computing
- Time-Series Forecasting
- Intelligent Communication Systems

---

## Applications

The proposed framework can be applied to:

- Smart Cities
- Environmental Monitoring
- Industrial IoT
- Smart Agriculture
- Smart Buildings
- Energy Monitoring Systems
- Remote Sensor Networks

---

## Key Contributions

✅ Designed and implemented an adaptive dual prediction framework

✅ Integrated Nordic Thingy:53 with Raspberry Pi edge infrastructure

✅ Developed machine learning-based sensor prediction models

✅ Implemented threshold-driven transmission control

✅ Evaluated energy consumption and communication efficiency

✅ Demonstrated the feasibility of intelligent transmission reduction in IoT systems

---

## Future Work

Potential improvements include:

- TinyML deployment on embedded hardware
- Dynamic threshold optimisation
- Real-time visualization dashboard
- Azure IoT Hub integration
- Cloud-edge collaborative prediction
- Multi-sensor adaptive forecasting
- Advanced deep learning architectures

---

## Learning Outcomes

This project provided practical experience in:

- IoT Architecture Design
- Edge Computing Systems
- Machine Learning Model Development
- Sensor Data Analytics
- Time-Series Forecasting
- Energy-Aware Communication Strategies
- Raspberry Pi Deployment
- Real-World IoT Engineering

---

## Conclusion

This project demonstrates the effectiveness of adaptive dual prediction techniques for reducing communication overhead in IoT environments. By combining machine learning with edge computing and threshold-based synchronization, the system minimizes unnecessary transmissions while maintaining accurate monitoring performance. The approach offers a scalable and energy-efficient solution for next-generation IoT deployments.

---

## Author

**Inas Mohideen**

Summer Intern | IoT, AI & Automation Enthusiast

Interested in Machine Learning, Edge Computing, Intelligent Systems, Cloud Technologies, and Data-Driven Automation.
