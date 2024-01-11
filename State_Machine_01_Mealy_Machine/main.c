/**
 * Copyright (c) 2014 - 2022, Nordic Semiconductor ASA
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * 1. Redistributions of source code must retain the above copyright notice, this
 *    list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form, except as embedded into a Nordic
 *    Semiconductor ASA integrated circuit in a product or a software update for
 *    such product, must reproduce the above copyright notice, this list of
 *    conditions and the following disclaimer in the documentation and/or other
 *    materials provided with the distribution.
 *
 * 3. Neither the name of Nordic Semiconductor ASA nor the names of its
 *    contributors may be used to endorse or promote products derived from this
 *    software without specific prior written permission.
 *
 * 4. This software, with or without modification, must only be used with a
 *    Nordic Semiconductor ASA integrated circuit.
 *
 * 5. Any software provided in binary form under this license must not be reverse
 *    engineered, decompiled, modified and/or disassembled.
 *
 * THIS SOFTWARE IS PROVIDED BY NORDIC SEMICONDUCTOR ASA "AS IS" AND ANY EXPRESS
 * OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY, NONINFRINGEMENT, AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL NORDIC SEMICONDUCTOR ASA OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
 * GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
 * OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 */
/** @file
 *
 * @defgroup blinky_example_main main.c
 * @{
 * @ingroup blinky_example
 * @brief Blinky Example Application main file.
 *
 * This file contains the source code for a sample application to blink LEDs.
 *
 */

#include <stdbool.h>
#include <stdint.h>
#include "nrf_delay.h"
#include "nrf_gpio.h"

// LED pins
#define LED_ONE   13
#define LED_TWO   14
#define LED_THREE 15
#define LED_FOUR  16

// Button pins
#define BUTTON_UP   11
#define BUTTON_DOWN 12

enum Event {
  UP,
  DOWN
};


typedef enum {
    LIGHT_ZERO,
    LIGHT_ONE,
    LIGHT_TWO,
    LIGHT_THREE,
    LIGHT_FOUR
} LEDState;

LEDState curr_state = LIGHT_ZERO;

void light_state_machine(enum Event event) {
    switch (curr_state) {
        case LIGHT_ZERO:
            switch (event) {
                case UP:
                    nrf_gpio_pin_clear(LED_ONE);
                    curr_state = LIGHT_ONE;
                    break;
                case DOWN:
                    break;
            }
            break;

        case LIGHT_ONE:
            switch (event) {
                case UP:
                    nrf_gpio_pin_clear(LED_TWO);
                    curr_state = LIGHT_TWO;
                    break;
                case DOWN:
                    nrf_gpio_pin_set(LED_ONE);
                    curr_state = LIGHT_ZERO;
                    break;
            }
            break;

        case LIGHT_TWO:
            switch (event) {
                case UP:
                    nrf_gpio_pin_clear(LED_THREE);
                    curr_state = LIGHT_THREE;
                    break;
                case DOWN:
                    nrf_gpio_pin_set(LED_TWO);
                    curr_state = LIGHT_ONE;
                    break;
            }
            break;

        case LIGHT_THREE:
            switch (event) {
                case UP:
                    nrf_gpio_pin_clear(LED_FOUR);
                    curr_state = LIGHT_FOUR;
                    break;
                case DOWN:
                    nrf_gpio_pin_set(LED_THREE);
                    curr_state = LIGHT_TWO;
                    break;
            }
            break;

        case LIGHT_FOUR:
            switch (event) {
                case UP:
                    break;
                case DOWN:
                    nrf_gpio_pin_set(LED_FOUR);
                    curr_state = LIGHT_THREE;
                    break;
            }
            break;
    }
}

int main(void) {
    nrf_gpio_cfg_output(LED_ONE);
    nrf_gpio_pin_set(LED_ONE);
    nrf_gpio_cfg_output(LED_TWO);
    nrf_gpio_pin_set(LED_TWO);
    nrf_gpio_cfg_output(LED_THREE);
    nrf_gpio_pin_set(LED_THREE);
    nrf_gpio_cfg_output(LED_FOUR);
    nrf_gpio_pin_set(LED_FOUR);

    nrf_gpio_cfg_input(BUTTON_UP, NRF_GPIO_PIN_PULLUP);
    nrf_gpio_cfg_input(BUTTON_DOWN, NRF_GPIO_PIN_PULLUP);

    while (true) {
        enum Event event;
        if (nrf_gpio_pin_read(BUTTON_UP) == false) 
        {
            event = UP;
            light_state_machine(event);
            while (nrf_gpio_pin_read(BUTTON_UP) == false); // Wait for button release
            nrf_delay_ms(100); // Debounce delay
        } 
        else if (nrf_gpio_pin_read(BUTTON_DOWN) == false) 
        {
            event = DOWN;
            light_state_machine(event);
            while (nrf_gpio_pin_read(BUTTON_DOWN) == false); // Wait for button release
            nrf_delay_ms(100); // Debounce delay
        }
    }
}


/**
 *@}
 **/
