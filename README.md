# Games with STM32 + ST7789 
This project is for my personal experimentation with stm32f103c8 microcontroller and some screen that uses a st7789 spi display. Currently only contains snake game, but im working on making tetris

# Requirements
This project uses an st7789 **[driver](https://github.com/abhra0897/stm32f1_st7789_spi)** which requires the **[libopencm3](https://github.com/libopencm3/libopencm3)** library to use. 

clone libopencm3 into the project root and use 'make' inside the libopencm3 directory to use

# WIRING CONNECTIONS

## SCREEN
I am using **[this](https://www.amazon.com/2inch-IPS-LCD-Display-Module/dp/B082GFTZQD?crid=2IX6GI59INLIT&dib=eyJ2IjoiMSJ9.s3-hl7a5ue_GjWCifla5J8apm1oCu0YHjZy0uU0FWFWi5ZvsyW5i_QkCFqnsooiDFiwwC9GZJomNftnrOI6A-TAiV-z3WZB4YX5tg15HqZLWhLFl0q72AlWEKmm7nBH_lUtsRSPbYgJ4TZZECdkJljX_q1FraQLkVlkxCi_1InuLO_BvVklPGrPKvpK3BLxIPP_K91C3gRex_n1iyZl03v_J9SzTk62eExP8jyXHo4BZCnDfmIqHBNx6Uj3W2athYzmiCPf9zufb5hb6mlYbLKIGG4BA3-3HJE4s3hfcwrQ.auTAsXPaRt5ie3zBVxLuBJusdl3diSsWXDf4GxaGv90&dib_tag=se&keywords=st7789&qid=1748296073&s=industrial&sprefix=st7789%2Cindustrial%2C188&sr=1-1&th=1)** screen, with wiring connections:

VCC --> 5V
GND --> G
DIN --> A7
CLK --> GPIO A5
CS --> GPIO A6
DC --> GPIO A2
RST --> GPIO A4
BL --> GPIO A3

# BUTTONS
I am using **[these](https://www.amazon.com/OCR-180PcsTactile-Momentary-Switches-Assortment/dp/B01MRP025V?crid=1YE0NK31AGPFC&dib=eyJ2IjoiMSJ9.ik_w5KInwh5rzyP-17_EWzy9taVbw4UHS79WAf3vwicSkih2PCBWMgurp5zSJIiZCln4egxdN7SkUdZXyjIlvpB76MfbYyy0Gnawzk-x3WGwEgQINlLYBfCkDPop65blfi7wA7SJFxbsH12tSIjswc69XHw2NGZ9E0UWhiUyJFJi5-yNxxwbdtC2xIoD4DLBnqY-KuZjxp93sQ73KS4d6l_e5510MGy9qHSRSwsfXWknkiBNV8saoZU7gUldsaW1K8cA7TvStc4XQFyKonx7wxL9UVAx1bGK6d3RMvlwjF4.4dqdnOjfq9xQetDMx8CEgrTiy54nHC_hF_KTPIp5rMk&dib_tag=se&keywords=electronic%2Bbuttons%2Blinear&qid=1749705341&s=industrial&sprefix=electronic%2Bbuttons%2Blinear%2Cindustrial%2C137&sr=1-6&th=1)** buttons, but any simple breadboard buttons should work.

Unless you have changed them inside main.c, the button wirings are:
LEFT --> GPIO B12
UP --> GPIO B13
RIGHT --> GPIO B14
DOWN --> GPIO B15
                                            
