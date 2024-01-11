#ifndef MAIN_H
#define MAIN_H
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>



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
  INC_LED,
  DEC_LED,
  START_PAUSE,
  ABRT,

  /* Internal activity signals */
  ENTRY,
  EXIT
}fsm_signal_t;


typedef enum
{
  EVENT_HANDLED,
  EVENT_IGNORED,
  EVENT_TRANSITION

}event_status_t;
 
//forward decleration
struct app_tag;
struct event_tag;

typedef event_status_t (*app_state_t)(struct app_tag *const, struct event_tag const *const); 

/* Main application structure */

typedef struct app_tag
{
  uint8_t curr_leds;
  app_state_t active_state;
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
event_status_t fsm_state_machine(app_t *const myApp, event_t const *const e);






#endif
