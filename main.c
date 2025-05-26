#include "games.h"
/**
 * Configure RCC registers for 8MHz HSE and 80MHz output.
 * I'm using STM32F103 which supports maximum 72 MHz, so I'm overclocking it
 */
void rcc_clock_setup_in_hse_8mhz_out_custom(void);
bool is_button_pressed(void);
void button_setup();

/* Set STM32 to 80 MHz. (overclocking for faster performance) */
static void clock_setup(void)
{
    rcc_clock_setup_in_hse_8mhz_out_custom();
}

int main(void)
{
    // Hardware config
    clock_setup();
    button_setup();
    systick_set_clocksource(STK_CSR_CLKSOURCE_AHB_DIV8);
	systick_set_reload(9999);
	systick_interrupt_enable();
	systick_counter_enable();

    //initializing the ili9341 display driver
    st_init();

    //rotating display to default mode
    st_rotate_display(2);

    //start screen
    st_fill_screen(ST_COLOR_BLACK);
    st_draw_string(15, 30, "Press any button to play", ST_COLOR_WHITE, &font_ubuntu_mono_24);
   
    while(1){
        if(is_right_pressed() || is_left_pressed() || is_down_pressed() || is_up_pressed()){
            snake_game();
        }
    }    
}

bool is_left_pressed(void) {
    // Return true if button is pressed (pin is low)
    // Buttons typically connect to ground when pressed
    return gpio_get(GPIOB, GPIO12) == 0;
}
bool is_down_pressed(void) {
    // Return true if button is pressed (pin is low)
    // Buttons typically connect to ground when pressed
    return gpio_get(GPIOB, GPIO13 ) == 0;
}
bool is_right_pressed(void) {
    // Return true if button is pressed (pin is low)
    // Buttons typically connect to ground when pressed
    return gpio_get(GPIOB, GPIO14) == 0;
}

bool is_up_pressed(void) {
    // Return true if button is pressed (pin is low)
    // Buttons typically connect to ground when pressed
    return gpio_get(GPIOB, GPIO15) == 0;
}
void button_setup(void) {
    // Enable clock for the GPIO port where your button is connected
    // For example, if your button is on GPIOB
    rcc_periph_clock_enable(RCC_GPIOB);
    
    // Configure the GPIO pin as input with pull-up
    // Assuming button is connected to PB12
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO12);
    
    // Enable internal pull-up (button connects to ground when pressed)
    gpio_set(GPIOB, GPIO12);


    // Assuming button is connected to PB12
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO13);
    
    // Enable internal pull-up (button connects to ground when pressed)
    gpio_set(GPIOB, GPIO13);

    
    // Assuming button is connected to PB12
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO14);
    
    // Enable internal pull-up (button connects to ground when pressed)
    gpio_set(GPIOB, GPIO14);


    // Assuming button is connected to PB12
    gpio_set_mode(GPIOB, GPIO_MODE_INPUT, GPIO_CNF_INPUT_PULL_UPDOWN, GPIO15);
    
    // Enable internal pull-up (button connects to ground when pressed)
    gpio_set(GPIOB, GPIO15);
}

/**
 * Overclock stm32 to 128MHz. HSE 8MHz multiplied by PLL 16
 * Performance may become unstable
 */
void rcc_clock_setup_in_hse_8mhz_out_custom(void)
{
    /* Enable internal high-speed oscillator. */
    rcc_osc_on(RCC_HSI);
    rcc_wait_for_osc_ready(RCC_HSI);

    /* Select HSI as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSICLK);

    /* Enable external high-speed oscillator 8MHz. */
    rcc_osc_on(RCC_HSE);
    rcc_wait_for_osc_ready(RCC_HSE);
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_HSECLK);

    /*
     * Set prescalers for AHB, ADC, APB1, APB2.
     * Do this before touching the PLL (TODO: why?).
     */
    rcc_set_hpre(RCC_CFGR_HPRE_SYSCLK_NODIV);    /* Set. 72MHz Max. 128MHz */
    rcc_set_adcpre(RCC_CFGR_ADCPRE_PCLK2_DIV8);  /* Set.  9MHz Max. 16MHz */
    rcc_set_ppre1(RCC_CFGR_PPRE1_HCLK_NODIV);     /* Set. 36MHz Max. 128MHz */
    rcc_set_ppre2(RCC_CFGR_PPRE2_HCLK_NODIV);    /* Set. 72MHz Max. 128MHz */

    /*
     * Sysclk runs with 72MHz -> 2 waitstates.
     * 0WS from 0-24MHz
     * 1WS from 24-48MHz
     * 2WS from 48-72MHz
     */
    flash_set_ws(2);

    /*
     * Set the PLL multiplication factor to 128.
     * 8MHz (external) * 16 (multiplier) = 128MHz
     */
    rcc_set_pll_multiplication_factor(RCC_CFGR_PLLMUL_PLL_CLK_MUL10);

    /* Select HSE as PLL source. */
    rcc_set_pll_source(RCC_CFGR_PLLSRC_HSE_CLK);

    /*
     * External frequency undivided before entering PLL
     * (only valid/needed for HSE).
     */
    rcc_set_pllxtpre(RCC_CFGR_PLLXTPRE_HSE_CLK);

    /* Enable PLL oscillator and wait for it to stabilize. */
    rcc_osc_on(RCC_PLL);
    rcc_wait_for_osc_ready(RCC_PLL);

    /* Select PLL as SYSCLK source. */
    rcc_set_sysclk_source(RCC_CFGR_SW_SYSCLKSEL_PLLCLK);

    /* Set the peripheral clock frequencies used */
    rcc_ahb_frequency = 128000000;
    rcc_apb1_frequency = 128000000;
    rcc_apb2_frequency = 128000000;
}
