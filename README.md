# Reality to Virtual

A real-time hand motion capture system that replicates hand gestures into a virtual environment using Unity game engine and Arduino Uno.

## Overview

This project captures hand motion data from Arduino hardware and translates it into real-time hand animations within a Unity game environment.

## Project Structure

- **Assets/** - Unity project files, scenes, scripts, and materials
- **Arduino/** - Arduino Uno firmware code for hand motion capture
- **ProjectSettings/** - Unity project configuration

## Requirements

### Software
- Unity (version specified in ProjectSettings)

### Hardware
- Arduino Uno
- Flex Sensors (for finger motion detection)
- MPU6050 (6-axis accelerometer/gyroscope for hand orientation)

## Setup

1. Import the project into Unity
2. Load the main scene from `Assets/Scenes/`
3. Upload `Arduino/Arduino.ino` to your Arduino Uno device
4. Connect Arduino to your system and configure the connection settings

## Author

**smuhammadkahaf**

---

For more details, refer to the documentation in `Assets/TutorialInfo/`
