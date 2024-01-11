
#include "main.h"
#include "boards.h"
#include "nrf_delay.h"
#include <stdio.h>


// Function prototypes
static void display_leds(app_t *const myApp);
static void display_message(char *msg);
static void display_clear(app_t *const myApp);
static void blink_leds(app_t *const myApp);

uint8_t LED_GROUP[] = {LED_ONE, LED_TWO, LED_THREE, LED_FOUR};


/**@brief Timeout handler for the repeated timer.
 */
static void repeated_timer_handler(void * p_context)
{
    app_t * myApp = (app_t *)p_context;
    blink_leds(myApp);
}

/**@brief Create timers.
 */
static void create_timers(app_t *const myApp)
{
    ret_code_t err_code;

    // Create timers
    err_code = app_timer_create(&m_repeated_timer_id,
                                APP_TIMER_MODE_REPEATED,
                                repeated_timer_handler);
    APP_ERROR_CHECK(err_code);

    // Start timer
    err_code = app_timer_start(m_repeated_timer_id,
                                APP_TIMER_TICKS(200),
                                (void*)myApp );
    APP_ERROR_CHECK(err_code);
}


/**@brief Stop timers.
 */
static void stop_timers()
{
  // Stop the repeated timer (stop blinking LED).
  ret_code_t err_code;
  err_code = app_timer_stop(m_repeated_timer_id);
  APP_ERROR_CHECK(err_code);
}

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
  for (int i = 0; i < myApp->curr_leds; i++) 
  {
    nrf_gpio_pin_toggle(LED_GROUP[i]);//to turn on the led
  }
}

/* IDLE state events and their functions */
event_status_t IDLE_ENTRY(app_t *const myApp, event_t const *const e)
{
  printf("IDLE_ENTRY\r\n");
  display_leds(myApp);
  display_message("IDLE STATE\r\n");
  myApp->active_state = IDLE;
  return EVENT_HANDLED;
}

event_status_t IDLE_EXIT(app_t *const myApp, event_t const *const e)
{
  printf("IDLE_EXIT\r\n");
  display_clear(myApp);
  display_message("Exit from: IDLE");           
  return EVENT_HANDLED;
}

event_status_t IDLE_INC_LED(app_t *const myApp, event_t const *const e)
{
  printf("IDLE_INC_LED\r\n");
  myApp->active_state = LED_SET;
  return EVENT_TRANSITION;
}

event_status_t IDLE_DEC_LED(app_t *const myApp, event_t const *const e)
{
  printf("IDLE_DEC_LED\r\n");  
  myApp->active_state = LED_SET;
  return EVENT_TRANSITION;
}

event_status_t IDLE_START_PAUSE(app_t *const myApp, event_t const *const e)
{ 
  if(myApp->curr_leds > 0)
  {
    printf("IDLE_START_PAUSE\r\n"); 
    myApp->active_state = BLINK;
    return EVENT_TRANSITION;  
  }
  else
  {
    printf("IDLE_START_PAUSE\r\n");
    display_message("EVENT_IGNORED\r\n");
    return EVENT_IGNORED;  
  }
}

event_status_t IDLE_ABRT(app_t *const myApp, event_t const *const e)
{
  printf("IDLE_ABRT\r\n");
  display_message("EVENT_IGNORED\r\n");
  return EVENT_IGNORED;
}

/* LED_SET state events and their functions */
event_status_t LED_SET_ENTRY(app_t *const myApp, event_t const *const e)
{
  printf("LED_SET_ENTRY\r\n");
  display_leds(myApp);
  display_message("Set Leds");
  return EVENT_HANDLED;
}

event_status_t LED_SET_EXIT(app_t *const myApp, event_t const *const e)
{
  printf("LED_SET_EXIT\r\n");
  display_clear(myApp);
  display_message("Exit from: LED_SET");        
  return EVENT_HANDLED;
}

event_status_t LED_SET_INC_LED(app_t *const myApp, event_t const *const e)
{
  printf("LED_SET_INC_LED\r\n");
  if(myApp->curr_leds < 4)
  {
    myApp->curr_leds += 1;
    display_clear(myApp);
    display_leds(myApp);
    return EVENT_HANDLED;
  }
  else
  {
    display_message("EVENT_IGNORED\r\n");
    return EVENT_IGNORED;
  }
}

event_status_t LED_SET_DEC_LED(app_t *const myApp, event_t const *const e)
{ 
  printf("LED_SET_DEC_LED\r\n");
  if(myApp->curr_leds > 0)
  {
    myApp->curr_leds -= 1;
    display_clear(myApp);
    display_leds(myApp);
    return EVENT_HANDLED;
  }
  else
  {
    display_message("EVENT_IGNORED\r\n");
    return EVENT_IGNORED;
  }
}

event_status_t LED_SET_START_PAUSE(app_t *const myApp, event_t const *const e)
{
  printf("LED_SET_START_PAUSE\r\n");
  myApp->active_state = BLINK;
  return EVENT_TRANSITION;
}

event_status_t LED_SET_ABRT(app_t *const myApp, event_t const *const e)
{
  printf("LED_SET_ABRT\r\n");
  myApp->active_state = IDLE;
  return EVENT_TRANSITION;
}


/* BLINK state events and their functions */
event_status_t BLINK_ENTRY(app_t *const myApp, event_t const *const e)
{
  printf("BLINK_ENTRY\r\n");
  if(myApp->curr_leds > 0)
  { 
    display_leds(myApp);
    display_message("APPLICATION is blinking LEDs\r\n");
    create_timers(myApp);
    return EVENT_TRANSITION;
  }
  else
  {
    display_message("EVENT_IGNORED\r\n"); 
    return EVENT_IGNORED;
  }
}

event_status_t BLINK_EXIT(app_t *const myApp, event_t const *const e)
{
  printf("BLINK_EXIT\r\n");
  display_clear(myApp);        
  myApp->active_state = PAUSE;
  display_message("Exit from: BLINK");
  return EVENT_HANDLED;
}

event_status_t BLINK_INC_LED(app_t *const myApp, event_t const *const e)
{ 
  printf("BLINK_INC_LED\r\n");
  display_message("EVENT_IGNORED\r\n");
  return EVENT_IGNORED;
}

event_status_t BLINK_DEC_LED(app_t *const myApp, event_t const *const e)
{ 
  printf("BLINK_DEC_LED\r\n");
  display_message("EVENT_IGNORED\r\n");
  return EVENT_IGNORED;
}

event_status_t BLINK_START_PAUSE(app_t *const myApp, event_t const *const e)
{
  printf("BLINK_START_PAUSE\r\n");
  myApp->active_state = PAUSE;
  return EVENT_TRANSITION;
}

event_status_t BLINK_ABRT(app_t *const myApp, event_t const *const e)
{ 
  if(myApp->curr_leds > 0)
  {
    printf("BLINK_ABRT\r\n");       
    stop_timers();
    myApp->active_state = IDLE;
    return EVENT_TRANSITION; 
  }
  else
  {
    printf("BLINK_ABRT\r\n"); 
    myApp->active_state = IDLE;
    return EVENT_TRANSITION; 
  }
  
}


/* PAUSE state events and their functions */
event_status_t PAUSE_ENTRY(app_t *const myApp, event_t const *const e)
{
  if(myApp->curr_leds > 0)
  {
    printf("PAUSE_ENTRY\r\n");
    stop_timers();
    display_leds(myApp);
    display_message("PAUSE Leds");
    return EVENT_HANDLED;  
  }
  else
  {
    printf("PAUSE_ENTRY\r\n");
    display_message("EVENT_IGNORED\r\n");
    return EVENT_IGNORED;  
  }
}

event_status_t PAUSE_EXIT(app_t *const myApp, event_t const *const e)
{
  printf("PAUSE_EXIT\r\n");
  display_clear(myApp); 
  display_message("Exit from: PAUSE");
  return EVENT_HANDLED;
}

event_status_t PAUSE_INC_LED(app_t *const myApp, event_t const *const e)
{ 
  printf("PAUSE_INC_LED\r\n");
  display_message("EVENT_IGNORED\r\n");
  return EVENT_IGNORED;
}

event_status_t PAUSE_DEC_LED(app_t *const myApp, event_t const *const e)
{  
  printf("PAUSE_DEC_LED\r\n");
  display_message("EVENT_IGNORED\r\n");
  return EVENT_IGNORED;
}

event_status_t PAUSE_START_PAUSE(app_t *const myApp, event_t const *const e)
{ 
  printf("PAUSE_START_PAUSE\r\n");
  myApp->active_state = BLINK;
  return EVENT_TRANSITION;
}

event_status_t PAUSE_ABRT(app_t *const myApp, event_t const *const e)
{
   
  if(myApp->curr_leds > 0)
  { 
    printf("PAUSE_ABRT\r\n");        
    stop_timers();
    myApp->active_state = IDLE;
    return EVENT_TRANSITION;
  }
  else
  { 
    printf("PAUSE_ABRT\r\n");   
    myApp->active_state = IDLE;
    return EVENT_TRANSITION;
  }
}


void fsm_init(app_t *myApp)
{
  event_t ee;
  ee.sig = ENTRY;
  e_handler_t ehandler;
  myApp->active_state = IDLE;
  myApp->curr_leds = 0;
  ehandler = (e_handler_t) myApp->state_table[(myApp->active_state * MAX_SIGNALS) + ee.sig];

  for(uint8_t i = 0; i<10; i++)
  {
    bsp_board_leds_on();
    nrf_delay_ms(50);
    bsp_board_leds_off();
    nrf_delay_ms(50);
  }
  (*ehandler)(myApp, &ee); //Jump to the handler
}
