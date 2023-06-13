#pragma once

#include "dwin_common.h"



#define PRIORITY_DIRECTION      5
#define PRIORITY_FAST_SERVICE   (PRIORITY_DIRECTION-1)
#define PRIORITY_SHOW           (PRIORITY_DIRECTION-2)
#define PRIORITY_SLOW_SERVICE   (PRIORITY_DIRECTION-3)


#define sleep_dwin(time_us)                                                                     \
                                do{                                                             \
                                    ESP_ERROR_CHECK(esp_sleep_enable_timer_wakeup(time_us));    \
                                    uart_wait_tx_idle_polling(UART_DWIN);                       \
                                    ESP_ERROR_CHECK(esp_light_sleep_start());                   \
                                }while(0)
                                    //stop_event_timer();                                     

typedef struct {
    TaskFunction_t pTask;
    UBaseType_t priority;
    uint32_t stack;
} task_dwin_t;

typedef struct {
    dwin_handler_t *main_handler;
    dwin_handler_t *show_handler;
} handlers_dwin_t;


extern  dwin_handler_t  wifi_set_mode_handler;
extern  dwin_handler_t  search_screen_handler;
extern  dwin_handler_t  ap_screen_handler;
extern  dwin_handler_t  setting_screen_handler;
extern  dwin_handler_t  main_screen_handler;
extern  dwin_handler_t  clock_handler;
extern  dwin_handler_t  state_screen_handler;
extern  dwin_handler_t  notifications_screen_handler;
extern  dwin_handler_t  timer_screen_handler;
extern  dwin_handler_t  set_color_screen_handler;

extern  dwin_handler_t show_main_handler;
extern  dwin_handler_t show_clock_handler;
extern  dwin_handler_t show_ssid_handler;
extern  dwin_handler_t show_settings_handler;
extern  dwin_handler_t show_color_screen_handler;
extern  dwin_handler_t show_notify_handler;
extern  dwin_handler_t show_ap_handler;
extern  dwin_handler_t show_state_handler;
extern  dwin_handler_t show_timer_stop_handler;

/* events structure */
#define SIZE_LIST_TASKS       END_KEY_SCREEN_TASK
static handlers_dwin_t screens_handlers[SIZE_LIST_TASKS] = {
    {
        .main_handler    = main_screen_handler,
        .show_handler    = show_main_handler,
    },
    {
        .main_handler    = clock_handler,
        .show_handler    = show_clock_handler,
    },
    {
        .main_handler    = search_screen_handler,
        .show_handler    = show_ssid_handler,
    },
    {
        .main_handler    = setting_screen_handler,
        .show_handler    = show_settings_handler,
    },
    {
        .main_handler    = set_color_screen_handler,
        .show_handler    = show_color_screen_handler,
    },
    {
        .main_handler    = notifications_screen_handler,
        .show_handler    = show_notify_handler,
    },
    {
        .main_handler    = ap_screen_handler,
        .show_handler    = show_ap_handler,
    },

    {
        .main_handler    = state_screen_handler,
        .show_handler    = show_state_handler,
    },
    {
        .main_handler    = timer_screen_handler,
        .show_handler    = show_timer_stop_handler,
    },
};

 

/* loop task */
void show_task(void*);
void direction_task(void *);
void slow_services_task(void *pv);
void fast_services_task(void *pv);
void uart_event_task(void *);


#define send_direct(key, data)                                                           \
    do{                                                                            \
        esp_event_post_to(direct_loop, EVENTS_DIRECTION, key, data, sizeof(data), TIMEOUT_SEND_EVENTS);       \
    }while(0) 

#define send_menager(key)                                                          \
    do{                                                                            \
        esp_event_post_to(direct_loop, EVENTS_MANAGER, key, NULL, 0, 400);         \
    }while(0) 

#define show_screen(key, dt)                                                       \
    do{                                                                            \
        esp_event_post_to(show_loop, EVENTS_SHOW, key, dt, sizeof(dt), 300);       \
    }while(0) 

#define start_ap()                                                                 \
    do{                                                                            \
        esp_event_post_to(fast_service_loop, WIFI_SET, INIT_AP, NULL, 0, 200);     \
    }while(0)    

#define get_weather()                                                                       \
    do{                                                                                     \
        esp_event_post_to(slow_service_loop, WIFI_SET, GET_WEATHER, NULL, 0, WAIT_SERVICE); \
    }while(0)                                                                               \

#define start_sta()                                                                         \
    do{                                                                                     \
        esp_event_post_to(fast_service_loop, WIFI_SET, START_STA, NULL, 0, 200);            \
    }while(0) 

#define stop_espnow()                                                                           \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, ESPNOW_SET, STOP_ESPNOW, NULL, 0, WAIT_SERVICE);   \
    }while(0)

#define start_espnow()                                                                          \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, ESPNOW_SET, START_ESPNOW, NULL, 0, 200);           \
    }while(0) 
#define start_sntp()                                                                            \
    do{                                                                                         \
        esp_event_post_to(slow_service_loop, WIFI_SET, INIT_SNTP, NULL, 0, WAIT_SERVICE);       \
    }while(0) 

#define stop_sntp()                                                                            \
    do{                                                                                        \
        esp_event_post_to(slow_service_loop, WIFI_SET, STOP_SNTP, NULL, 0, WAIT_SERVICE);      \
    }while(0) 

void init_dwin_events(main_data_t*);
void check_net_data_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data);
void set_screen_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data);
 static task_dwin_t sevice_tasks[SIZE_LIST_TASKS] = {
    {
        .pTask = direction_task,
        .priority = PRIORITY_DIRECTION,
        .stack = 2048
    },
    {
        .pTask = show_task,
        .priority = PRIORITY_SHOW,
        .stack = 2048
    },
    {
        .pTask = fast_services_task,
        .priority = PRIORITY_FAST_SERVICE,
        .stack = 10000
    },
    {
        .pTask = slow_services_task,
        .priority = PRIORITY_SLOW_SERVICE,
        .stack = 10000
    },
    {
        .pTask = uart_event_task,
        .priority = 10,
        .stack = 4048
    },
};




void vApplicationIdleHook(void );
