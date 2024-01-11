#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "app_timer.h"
#include "nrf_drv_clock.h"



APP_TIMER_DEF(m_repeated_timer_id);     /**< Handler for repeated timer used to blink LED 1. */


/* define leds */
#define LED_ONE   13
#define LED_TWO   14
#define LED_THREE 15
#define LED_FOUR  16
    
/* define button group */
extern uint8_t LED_GROUP[]; // Declare LED_GROUP as an external variable

// Button pins
#define BUTTON_ONE   11
#define BUTTON_TWO   12
#define BUTTON_THREE 24
#define BUTTON_FOUR  25


/* define buttons state */
typedef enum
{
  NOT_PRESSED,
  BOUNCE,
  PRESSED
}button_state_t; 

/* signals of the application */
typedef enum
{
  /* Internal activity signals */
  ENTRY,
  EXIT,

  INC_LED,
  DEC_LED,
  START_PAUSE,
  ABRT,

  MAX_SIGNALS
}fsm_signal_t;


typedef enum
{
  EVENT_HANDLED,
  EVENT_IGNORED,
  EVENT_TRANSITION

}event_status_t;
 
/* Various states of the application */

typedef  enum
{
  IDLE,
  LED_SET,
  BLINK,
  PAUSE,
  MAX_STATE
}app_state_t;   

//forward decleration
struct app_tag;
struct event_tag;

typedef event_status_t (*e_handler_t)(struct app_tag *const, struct event_tag const *const); 

/* Main application structure */

typedef struct app_tag
{
  uint8_t curr_leds;
  app_state_t active_state;
  uintptr_t *state_table;
}app_t; 

typedef struct event_tag
{
  fsm_signal_t sig;
}event_t;

typedef struct
{
  event_t super;
}app_user_event_t; 

typedef struct
{
  event_t super;
  uint8_t ss;
}app_blink_event_t; 


void fsm_init(app_t *myApp);
void fsm_button_init();
void fsm_led_init();
static void fsm_state_table_init(app_t *const myApp);


/* IDLE state events and their functions */
event_status_t IDLE_ENTRY(app_t *const myApp, event_t const *const e);
event_status_t IDLE_EXIT(app_t *const myApp, event_t const *const e);
event_status_t IDLE_INC_LED(app_t *const myApp, event_t const *const e);
event_status_t IDLE_DEC_LED(app_t *const myApp, event_t const *const e);
event_status_t IDLE_START_PAUSE(app_t *const myApp, event_t const *const e);
event_status_t IDLE_ABRT(app_t *const myApp, event_t const *const e);


/* LED_SET state events and their functions */
event_status_t LED_SET_ENTRY(app_t *const myApp, event_t const *const e);
event_status_t LED_SET_EXIT(app_t *const myApp, event_t const *const e);
event_status_t LED_SET_INC_LED(app_t *const myApp, event_t const *const e);
event_status_t LED_SET_DEC_LED(app_t *const myApp, event_t const *const e);
event_status_t LED_SET_START_PAUSE(app_t *const myApp, event_t const *const e);
event_status_t LED_SET_ABRT(app_t *const myApp, event_t const *const e);


/* BLINK state events and their functions */
event_status_t BLINK_ENTRY(app_t *const myApp, event_t const *const e);
event_status_t BLINK_EXIT(app_t *const myApp, event_t const *const e);
event_status_t BLINK_INC_LED(app_t *const myApp, event_t const *const e);
event_status_t BLINK_DEC_LED(app_t *const myApp, event_t const *const e);
event_status_t BLINK_START_PAUSE(app_t *const myApp, event_t const *const e);
event_status_t BLINK_ABRT(app_t *const myApp, event_t const *const e);


/* PAUSE state events and their functions */
event_status_t PAUSE_ENTRY(app_t *const myApp, event_t const *const e);
event_status_t PAUSE_EXIT(app_t *const myApp, event_t const *const e);
event_status_t PAUSE_INC_LED(app_t *const myApp, event_t const *const e);
event_status_t PAUSE_DEC_LED(app_t *const myApp, event_t const *const e);
event_status_t PAUSE_START_PAUSE(app_t *const myApp, event_t const *const e);
event_status_t PAUSE_ABRT(app_t *const myApp, event_t const *const e);


#endif
