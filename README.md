# XOLED

![XOLED Showcase](https://makerworld.bblmw.com/makerworld/model/US3a976399c751ad/design/2024-08-03_c5c91ad4dcc5d.jpeg?x-oss-process=image/resize,w_1920/format,webp)

XOLED is a firmware project designed for the ESP32 microcontroller. It connects to a Bambu Lab 3D printer using MQTT to display the print progress on a LED strip. The project includes a 3D-printed enclosure designed by [ortoPilot](https://twitch.tv/ortopilot).

## Table of Contents

- [Features](#features)
- [Installation](#installation)
- [Usage](#usage)
- [Hardware Requirements](#hardware-requirements)
- [Enclosure](#enclosure)
- [License](#license)
- [Acknowledgements](#acknowledgements)

## Features

- Connects to Bambu Lab 3D printers using MQTT protocol.
- Displays real-time print progress on a LED strip.
- User-friendly 3D-printed enclosure for easy mounting.
- Experimental support for an I2C display

## Installation

The easiest way to install the firmware is using the web installer, which works on Chrome browsers. Visit the installer [here](https://michaelowens.github.io/XOLED). Instructions on connecting the hardware can be found on the [enclosure](#enclosure) page.

- Visit the [web installer](https://michaelowens.github.io/XOLED) in your Chrome browser.
- Connect your ESP32 to your computer using a USB cable.
- Follow the on-screen instructions to upload the firmware to your ESP32.
- During the installation process, you will be prompted to enter your WiFi credentials.

## Usage

Once the firmware is uploaded and the ESP32 is powered on, it will:

1. Connect to the configured WiFi network.
2. Connect to the specified MQTT server.
3. Subscribe to the MQTT topic that receives print progress updates from your Bambu Lab printer.
4. Display the print progress on the LED strip in real-time.

## Hardware Requirements

- ESP32 microcontroller
- LED strip (e.g., WS2812B)
- 3D-printed enclosure (see below)

## Enclosure

The 3D-printed enclosure designed by [ortoPilot](https://twitch.tv/ortopilot) provides a neat and secure housing for the ESP32 and LED strip. You can download the STL files for the enclosure from [this link](https://makerworld.com/en/models/570064#profileId-489970) and print it using your 3D printer.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for more details.

## Acknowledgements

- **[ortoPilot](https://twitch.tv/ortopilot)** for designing the 3D-printed enclosure.
- **Bambu Lab** for their awesome 3D printers.
- **FastLED** for the FastLED library
- **knolleary** for the PubSubClient library
- **bblanchon** for the ArduinoJson library
- **Aircoookie** for the ESPAsyncWebServer library
- **jnthas** for the Improv WiFi Library
- **Adafruit** for the GFX Library and SSD1306 libraries.
