# Demo

<iframe width="560" height="315" src="https://www.youtube.com/embed/h04urzrEfU4?si=V7YgSBaAuW-LtUow" title="YouTube video player" frameborder="0" allow="accelerometer; autoplay; clipboard-write; encrypted-media; gyroscope; picture-in-picture; web-share" referrerpolicy="strict-origin-when-cross-origin" allowfullscreen></iframe>

# STM32 and ST7789 Games

This project is a personal project for experimenting with the STM32F103C8T6 microcontroller and a display with a st7789 driver. The goal is to create a simple, handheld game console. This repository contains all the necessary code, libraries, and instructions to build your own. Currently only has snake game.

## Hardware

To build this project, you will need the following hardware components:

*   **Microcontroller:** STM32F103C8 (Blue Pill)
*   **Display:** Display Module with ST7789 driver. I am using **[this](https://www.amazon.com/2inch-IPS-LCD-Display-Module/dp/B082GFTZQD?crid=2IX6GI59INLIT&dib=eyJ2IjoiMSJ9.s3-hl7a5ue_GjWCifla5J8apm1oCu0YHjZy0uU0FWFWi5ZvsyW5i_QkCFqnsooiDFiwwC9GZJomNftnrOI6A-TAiV-z3WZB4YX5tg15HqZLWhLFl0q72AlWEKmm7nBH_lUtsRSPbYgJ4TZZECdkJljX_q1FraQLkVlkxCi_1InuLO_BvVklPGrPKvpK3BLxIPP_K91C3gRex_n1iyZl03v_J9SzTk62eExP8jyXHo4BZCnDfmIqHBNx6Uj3W2athYzmiCPf9zufb5hb6mlYbLKIGG4BA3-3HJE4s3hfcwrQ.auTAsXPaRt5ie3zBVxLuBJusdl3diSsWXDf4GxaGv90&dib_tag=se&keywords=st7789&qid=1748296073&s=industrial&sprefix=st7789%2Cindustrial%2C188&sr=1-1&th=1)** screen
*   **Buttons:** Any normal breadboard connector type buttons should work. I am using **[these](https://www.amazon.com/OCR-180PcsTactile-Momentary-Switches-Assortment/dp/B01MRP025V?crid=1YE0NK31AGPFC&dib=eyJ2IjoiMSJ9.ik_w5KInwh5rzyP-17_EWzy9taVbw4UHS79WAf3vwicSkih2PCBWMgurp5zSJIiZCln4egxdN7SkUdZXyjIlvpB76MfbYyy0Gnawzk-x3WGwEgQINlLYBfCkDPop65blfi7wA7SJFxbsH12tSIjswc69XHw2NGZ9E0UWhiUyJFJi5-yNxxwbdtC2xIoD4DLBnqY-KuZjxp93sQ73KS4d6l_e5510MGy9qHSRSwsfXWknkiBNV8saoZU7gUldsaW1K8cA7TvStc4XQFyKonx7wxL9UVAx1bGK6d3RMvlwjF4.4dqdnOjfq9xQetDMx8CEgrTiy54nHC_hF_KTPIp5rMk&dib_tag=se&keywords=electronic%2Bbuttons%2Blinear&qid=1749705341&s=industrial&sprefix=electronic%2Bbuttons%2Blinear%2Cindustrial%2C137&sr=1-6&th=1)** buttons
*   **Breadboard and Jumper Wires:** Any
*   **ST-Link V2:** To flash the code onto the microcontroller

## Software

This project relies on the following software:

*   **[libopencm3](https://github.com/libopencm3/libopencm3):** open-source library for ARM Cortex-M microcontrollers.
*   **[st7789 driver](https://github.com/abhra0897/stm32f1_st7789_spi):** A driver for the ST7789 display.
*   **[ARM GCC Toolchain](https://developer.arm.com/tools-and-software/open-source-software/developer-tools/gnu-toolchain/gnu-rm/downloads):**
    *   **Linux (Debian/Ubuntu):** `sudo apt install gcc-arm-none-eabi binutils-arm-none-eabi`
    *   **Linux (Arch):** `sudo pacman -S arm-none-eabi-gcc arm-none-eabi-binutils`
    *   **macOS:** `brew install --cask gcc-arm-embedded`
    *   **Windows** good luck to you, maybe try wsl2?
*   **Make:** 
    *   **Linux (Debian/Ubuntu):** `sudo apt install build-essential`
    *   **Linux (Arch):** `sudo pacman -S make`
    *   **macOS:** Install Xcode Command Line Tools. `xcode-select --install`
*   **[st-flash](https://github.com/stlink-org/stlink):** For flashing code to the device. Follow the installation instructions in the official repository. You will likely need `libusb-1.0-0-dev` (`sudo apt install libusb-1.0-0-dev` on debian).

## Wiring

### Screen

| Pin | STM32F103C8 |
|-----|-------------|
| VCC | 5V          |
| GND | G           |
| DIN | GPIO A7     |
| CLK | GPIO A5     |
| CS  | GPIO A6     |
| DC  | GPIO A2     |
| RST | GPIO A4     |
| BL  | GPIO A3     |

### Buttons

| Button | STM32F103C8 |
|--------|-------------|
| LEFT   | GPIO B12    |
| UP     | GPIO B13    |
| RIGHT  | GPIO B14    |
| DOWN   | GPIO B15    |

## Building

1.  **Clone the repository:**
    ```bash
    git clone https://github.com/your-username/your-repository.git
    cd your-repository
    ```

2.  **Clone the required libraries:**
    ```bash
    git clone https://github.com/libopencm3/libopencm3
    ```

3.  **Build libopencm3:**
    ```bash
    cd libopencm3
    make
    cd ..
    ```

4.  **Build the project:**
    ```bash
    make
    ```

## Flashing

To flash the firmware onto the STM32F103C8, you will need to have the `st-flash` utility installed. You can install it by following the instructions **[here](https://github.com/stlink-org/stlink)**.

Once `st-flash` is installed, connect the ST-Link V2 to your computer and the STM32F103C8, then run the following command:

```bash
make burn
```

## TODO

*   **Tetris Game:** work on making Tetris
*   **Sound:** Add a buzzer for sound effects
*   **3D Printed Case:** Design and print a case to house the components for true handheld-ness.

## Contributing

Contributions are welcome! If you have any ideas, suggestions, or improvements, please feel free to open an issue or submit a pull request.

## License

This project is licensed under the MIT License. See the `LICENSE` file for more details.
                                            
