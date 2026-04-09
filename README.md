# IoT Lab Final Project

## Overview

Monitoring system for patients using TI CC1350 hardware.
The system detects events (falls, temperature, humidity) and displays alerts via a web interface.

## System Architecture

PC (web interface + Python script) <-> Launchpad (receiver) <-> SensorTag (transmitter)

## Features

* Fall detection (based on acceleration)
* Pill reminder system
* Temperature monitoring
* Humidity monitoring

## Technologies

* Embedded C (TI RTOS)
* Python
* Web interface
* RF communication

## Hardware

* LAUNCHXL-CC1350
* CC1350STK SensorTag

## Setup

1. Clone the repository
2. Open `CCS_workspace/` in Code Composer Studio
3. Import projects and build/flash

## Repository Structure

* `CCS_workspace/` embedded code
* `webpage/` web interface
* `report/` project documentation

## Notes

Developed as part of an IoT Lab course project.
