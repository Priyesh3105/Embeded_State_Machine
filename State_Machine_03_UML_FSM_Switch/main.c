#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "main.h"
#include "nrf_delay.h"
#include "nrf_gpio.h"
#include "boards.h"


void fsm_button_init()
{
  
 nrf_gpio_cfg_input(BUTTON_ONE, NRF_GPIO_PIN_PULLUP);
 nrf_gpio_cfg_input(BUTTON_TWO, NRF_GPIO_PIN_PULLUP); 
 nrf_gpio_cfg_input(BUTTON_THREE, NRF_GPIO_PIN_PULLUP);
 nrf_gpio_cfg_input(BUTTON_FOUR, NRF_GPIO_PIN_PULLUP);
}

void fsm_led_init()
{
  nrf_gpio_cfg_output(LED_ONE);
  nrf_gpio_cfg_output(LED_TWO);
  nrf_gpio_cfg_output(LED_THREE);
  nrf_gpio_cfg_output(LED_FOUR);
}

static uint8_t process_btn_pad_value(uint8_t btn_pad_value)
{
    static button_state_t btn_sm_state = NOT_PRESSED;

    switch(btn_sm_state)
    {
      case NOT_PRESSED:
      {
        if(btn_pad_value)
        {
          btn_sm_state = BOUNCE;
          nrf_delay_ms(50);
        }
      }
      case BOUNCE:
      {
        if(btn_pad_value)
        {
          btn_sm_state = PRESSED;          
          printf("btn value: %d\r\n",btn_pad_value);
          return btn_pad_value;
        }
        else
        {
          btn_sm_state = NOT_PRESSED;
        }
      }
      case PRESSED:  
      {
        if(!btn_pad_value)
        {
          btn_sm_state = BOUNCE;
        }
      }
    }
    return 0;
}


static void fsm_event_dispatcher(app_t *const myApp, event_t const *const e)
{
    event_status_t status;
    app_state_t source, target;
     
    source = myApp->active_state;
    status = fsm_state_machine(myApp, e);
    if(status == EVENT_TRANSITION)
    {
      target = myApp->active_state;
      event_t ee;

      //1. run exit action for source state
      ee.sig = EXIT;
      myApp->active_state = source;
      fsm_state_machine(myApp, &ee);
      
      //2. run entry action for source state
      ee.sig = ENTRY;
      myApp->active_state = target;
      fsm_state_machine(myApp, &ee);
    }
}


int main(void) {

    static app_t fsm_App;
    bsp_board_init(BSP_INIT_LEDS);
    fsm_button_init();
    fsm_led_init();
    fsm_init(&fsm_App);

    uint8_t b1,b2,b3,b4,btn_pad_value;
    app_user_event_t ue;

    while(true)
    {
      /* 1. Read the button pad tatus*/
      b1 = nrf_gpio_pin_read(BUTTON_ONE);
      b2 = nrf_gpio_pin_read(BUTTON_TWO);
      b3 = nrf_gpio_pin_read(BUTTON_THREE);
      b4 = nrf_gpio_pin_read(BUTTON_FOUR);

      btn_pad_value = ((!b1)<<3)|((!b2)<<2)|((!b3)<<1)|(!b4);
      //software button debouncing
      btn_pad_value = process_btn_pad_value(btn_pad_value);

      /* 2. Make an event */
      if(btn_pad_value)
      {
        if(btn_pad_value == BUTTON_PAD_VALUE_INC_LED)
        {
          ue.super.sig = INC_LED;
        }
        else if (btn_pad_value == BUTTON_PAD_VALUE_DEC_LED)
        {
          ue.super.sig = DEC_LED;        
        }
        else if (btn_pad_value == BUTTON_PAD_VALUE_ABRT)
        {
          ue.super.sig = ABRT;        
        }
        else if (btn_pad_value == BUTTON_PAD_VALUE_SP)
        {
          ue.super.sig = START_PAUSE;        
        }
        /* 3. Send it to an event dispatcher */
        fsm_event_dispatcher(&fsm_App, &ue.super);
      }      
    }    
}


/**
 *@}
 **/
