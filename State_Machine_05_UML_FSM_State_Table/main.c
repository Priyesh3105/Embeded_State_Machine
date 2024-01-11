

#include <stdbool.h>
#include "nrf.h"
#include "nrf_drv_gpiote.h"
#include "app_error.h"
#include "boards.h"
#include "main.h"
#include "nrf_delay.h"


#define BUTTON_COUNT 4

static app_t fsm_App;
uint8_t btn_pad_value;

static uint8_t button_pins[BUTTON_COUNT] = {BUTTON_ONE, BUTTON_TWO, BUTTON_THREE, BUTTON_FOUR}; // Define your button pins


void fsm_led_init()
{
  nrf_gpio_cfg_output(LED_ONE);
  nrf_gpio_cfg_output(LED_TWO);
  nrf_gpio_cfg_output(LED_THREE);
  nrf_gpio_cfg_output(LED_FOUR);
}

static void fsm_event_dispatcher(app_t *const myApp, event_t const *const e)
{
    event_status_t status;
    app_state_t source, target;
    e_handler_t ehandler;
     
    source = myApp->active_state;
    ehandler = (e_handler_t) myApp->state_table[(myApp->active_state * MAX_SIGNALS) + e->sig];
    status = (*ehandler)(myApp, e);
    if(status == EVENT_TRANSITION)
    {
      target = myApp->active_state;
      event_t ee;

      //1. run exit action for source state
      ee.sig = EXIT;
      ehandler = (e_handler_t) myApp->state_table[(source * MAX_SIGNALS) + EXIT];
      (*ehandler)(myApp, &ee);
      
      //2. run entry action for source state
      ee.sig = ENTRY;
      ehandler = (e_handler_t) myApp->state_table[(target * MAX_SIGNALS) + ENTRY];
      (*ehandler)(myApp, &ee);
    }
}

static void fsm_state_table_init(app_t *const myApp)
{
    static e_handler_t fsm_state_table[MAX_STATE][MAX_SIGNALS] = {
      [IDLE] = {&IDLE_ENTRY, &IDLE_EXIT, &IDLE_INC_LED, &IDLE_DEC_LED, &IDLE_START_PAUSE, &IDLE_ABRT},
      [LED_SET] = {&LED_SET_ENTRY, &LED_SET_EXIT, &LED_SET_INC_LED, &LED_SET_DEC_LED, &LED_SET_START_PAUSE, &LED_SET_ABRT},
      [BLINK] = {&BLINK_ENTRY, &BLINK_EXIT, &BLINK_INC_LED, &BLINK_DEC_LED, &BLINK_START_PAUSE, &BLINK_ABRT},
      [PAUSE] = {&PAUSE_ENTRY, &PAUSE_EXIT, &PAUSE_INC_LED, &PAUSE_DEC_LED, &PAUSE_START_PAUSE, &PAUSE_ABRT}   
    };
    
    myApp->state_table = (uintptr_t *) &fsm_state_table[0][0];
}

void in_pin_handler(nrfx_gpiote_pin_t pin, nrf_gpiote_polarity_t action)
{
  app_user_event_t ue;

  /* 2. Make an event */
  if(action)
  {
    if(pin == BUTTON_ONE)
    {
      ue.super.sig = INC_LED;
      printf("Signal: INC_LED\r\n");
    }
    else if (pin == BUTTON_TWO)
    {
      ue.super.sig = DEC_LED;  
      printf("Signal: DEC_LED\r\n");      
    }
    else if (pin == BUTTON_THREE)
    {
      ue.super.sig = START_PAUSE;
      printf("Signal: START_PAUSE\r\n");        
    }
    else if (pin == BUTTON_FOUR)
    {
      ue.super.sig = ABRT;  
      printf("Signal: ABRT\r\n");      
    }
    /* 3. Send it to an event dispatcher */
    fsm_event_dispatcher(&fsm_App, &ue.super);
  }
}


/**
 * @brief Function for configuring: button pin for input, PIN_OUT pin for output,
 * and configures GPIOTE to give an interrupt on pin change.
 */
static void gpio_init(void)
{
    ret_code_t err_code;

    err_code = nrf_drv_gpiote_init();
    APP_ERROR_CHECK(err_code);

    nrf_drv_gpiote_in_config_t in_config = GPIOTE_CONFIG_IN_SENSE_HITOLO(true);
    in_config.pull = NRF_GPIO_PIN_PULLUP;

    for (int i = 0; i < BUTTON_COUNT; ++i) {
        err_code = nrf_drv_gpiote_in_init(button_pins[i], &in_config, in_pin_handler);
        APP_ERROR_CHECK(err_code);

        nrf_drv_gpiote_in_event_enable(button_pins[i], true);
    }
}

/**@brief Function starting the internal LFCLK oscillator.
 *
 * @details This is needed by RTC1 which is used by the Application Timer
 *          (When SoftDevice is enabled the LFCLK is always running and this is not needed).
 */
static void lfclk_request(void)
{
    ret_code_t err_code = nrf_drv_clock_init();
    APP_ERROR_CHECK(err_code);
    nrf_drv_clock_lfclk_request(NULL);
}


/**
 * @brief Function for application main entry.
 */
int main(void)
{
    bsp_board_init(BSP_INIT_LEDS);
    fsm_led_init();
    fsm_state_table_init(&fsm_App);
    fsm_init(&fsm_App);
    gpio_init();
    lfclk_request();
    app_timer_init();

    while (true)
    {
        // Do nothing.
    }
}


/** @} */
