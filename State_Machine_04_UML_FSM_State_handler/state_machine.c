
#include "main.h"
#include "boards.h"
#include "nrf_delay.h"
#include <stdio.h>


// Function prototypes for state handlers
static event_status_t fsm_state_handler_IDLE(app_t *const myApp, event_t const *const e);
static event_status_t fsm_state_handler_LED_SET(app_t *const myApp, event_t const *const e);
static event_status_t fsm_state_handler_BLINK(app_t *const myApp, event_t const *const e);
static event_status_t fsm_state_handler_PAUSE(app_t *const myApp, event_t const *const e);
static void display_leds(app_t *const myApp);
static void display_message(char *msg);
static void display_clear(app_t *const myApp);
static void blink_leds(app_t *const myApp);

#define IDLE         &fsm_state_handler_IDLE
#define LED_SET      &fsm_state_handler_LED_SET
#define BLINK        &fsm_state_handler_BLINK
#define PAUSE        &fsm_state_handler_PAUSE

uint8_t LED_GROUP[] = {LED_ONE, LED_TWO, LED_THREE, LED_FOUR};

static void display_leds(app_t *const myApp)
{
  printf("Current LEDs: %d\r\n", myApp->curr_leds);
  for(uint8_t i = 0; i<myApp->curr_leds; i++)
  {
    nrf_gpio_pin_clear(LED_GROUP[i]);//to turn on the led
    nrf_delay_ms(50);
  }
}

static void display_message(char *msg)
{
    printf("%s\r\n", msg);
}

static void display_clear(app_t *const myApp)
{
  printf("Clear led\r\n");
  for(uint8_t i = 0; i<4; i++)
  {
    nrf_gpio_pin_set(LED_GROUP[i]);//to turn off the led
    nrf_delay_ms(50);
  }
}

void blink_leds(app_t *const myApp)
{
  for(int i=0; i<100; i++)
  {
    for (int i = 0; i < myApp->curr_leds; i++) 
    {
      nrf_gpio_pin_clear(LED_GROUP[i]);//to turn on the led
    }
    nrf_delay_ms(100); // Wait for the specified time

    for (int i = 0; i < myApp->curr_leds; i++) 
    {
      nrf_gpio_pin_set(LED_GROUP[i]);//to turn off the led
    }
    nrf_delay_ms(100); // Wait for the specified time
  }
}

static event_status_t fsm_state_handler_IDLE(app_t *const myApp, event_t const *const e)
{
   switch(e->sig)
   {
      case ENTRY:
      {
        display_leds(myApp);
        display_message("IDLE STATE\r\n");
        return EVENT_HANDLED;
      }

      case EXIT:
      {
        display_clear(myApp);
        display_message("Exit from: IDLE");           
        return EVENT_HANDLED;
      }

      case INC_LED:
      { 
        myApp->active_state = LED_SET;
        return EVENT_TRANSITION;
      }

      case DEC_LED:
      {  
        myApp->active_state = LED_SET;
        return EVENT_TRANSITION;
      }

      case START_PAUSE:
      {        
        myApp->active_state = BLINK;
        return EVENT_TRANSITION;
      }

      case ABRT:
      {
        return EVENT_IGNORED;
      }

      return EVENT_IGNORED;
   }
}

static event_status_t fsm_state_handler_LED_SET(app_t *const myApp, event_t const *const e)
{
    switch(e->sig)
   {
      case ENTRY:
      {
        display_leds(myApp);
        display_message("Set Leds");
        return EVENT_HANDLED;
      }

      case EXIT:
      {
        display_clear(myApp);
        display_message("Exit from: LED_SET");        
        return EVENT_HANDLED;
      }

      case INC_LED:
      {
        if(myApp->curr_leds < 4)
        {
          myApp->curr_leds += 1;
          display_clear(myApp);
          display_leds(myApp);
          return EVENT_HANDLED;
        }
        else
        {
          return EVENT_IGNORED;
        }
      }

      case DEC_LED:
      { 
        if(myApp->curr_leds > 0)
        {
          myApp->curr_leds -= 1;
          display_clear(myApp);
          display_leds(myApp);
          return EVENT_HANDLED;
        }
        else
        {
          return EVENT_IGNORED;
        }
      }

      case START_PAUSE:
      {
        myApp->active_state = BLINK;
        return EVENT_TRANSITION;
      }

      case ABRT:
      {
        myApp->active_state = IDLE;
        return EVENT_TRANSITION;
      }

      return EVENT_IGNORED;
   }
}

static event_status_t fsm_state_handler_BLINK(app_t *const myApp, event_t const *const e)
{
    switch(e->sig)
   {
      case ENTRY:
      {
         if(myApp->curr_leds > 0)
        {          
          display_leds(myApp);
          display_message("APPLICATION is blinking LEDs\r\n");
          blink_leds(myApp);
          return EVENT_TRANSITION;
        }
        else
        { 
          return EVENT_IGNORED;
        }
      }

      case EXIT:
      {
        display_clear(myApp);        
        myApp->active_state = PAUSE;
        display_message("Exit from: BLINK");
        return EVENT_HANDLED;
      }

      case INC_LED:
      { 
        return EVENT_IGNORED;
      }

      case DEC_LED:
      {  
        return EVENT_IGNORED;
      }

      case START_PAUSE:
      {
        myApp->active_state = PAUSE;
        return EVENT_TRANSITION;
      }

      case ABRT:
      {
        myApp->active_state = IDLE;
        return EVENT_TRANSITION;
      }

      return EVENT_IGNORED;
   }
}

static event_status_t fsm_state_handler_PAUSE(app_t *const myApp, event_t const *const e)
{
    switch(e->sig)
   {
      case ENTRY:
      {
        display_leds(myApp);
        display_message("PAUSE Leds");
        return EVENT_HANDLED;
      }

      case EXIT:
      {
        display_clear(myApp); 
        display_message("Exit from: PAUSE");
        return EVENT_HANDLED;
      }

      case INC_LED:
      { 
        return EVENT_IGNORED;
      }

      case DEC_LED:
      {  
        return EVENT_IGNORED;
      }
   
      case START_PAUSE:
      {
        myApp->active_state = BLINK;
        return EVENT_TRANSITION;
      }

      case ABRT:
      {
        myApp->active_state = IDLE;
        return EVENT_TRANSITION;
      }

      return EVENT_IGNORED;
   }
}


void fsm_init(app_t *myApp)
{
  event_t ee;
  ee.sig = ENTRY;
  myApp->active_state = IDLE;
  myApp->curr_leds = 0;

  for(uint8_t i = 0; i<10; i++)
  {
    bsp_board_leds_on();
    nrf_delay_ms(50);
    bsp_board_leds_off();
    nrf_delay_ms(50);
  }
  (*myApp->active_state)(myApp, &ee); //Jump to the handler
}
