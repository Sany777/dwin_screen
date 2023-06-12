#include "dwin_events.h"

esp_event_loop_handle_t 
                direct_loop, 
                show_loop, 
                fast_service_loop,
                slow_service_loop;
                
ESP_EVENT_DEFINE_BASE(EVENTS_SHOW);
ESP_EVENT_DEFINE_BASE(EVENTS_SERVICE);
ESP_EVENT_DEFINE_BASE(EVENTS_DIRECTION);
ESP_EVENT_DEFINE_BASE(EVENTS_MANAGER);
ESP_EVENT_DEFINE_BASE(WIFI_SET);
ESP_EVENT_DEFINE_BASE(ESPNOW_SET);


void init_dwin_events(main_data_t *main_data) 
{
    const esp_event_loop_args_t my_loop_args = {
        .queue_size = TASK_EVENT_SIZE,
        .task_name = NULL
    };
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &direct_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &show_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &fast_service_loop));
    ESP_ERROR_CHECK(esp_event_loop_create(&my_loop_args, &slow_service_loop));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                ESPNOW_SET,
                                ESP_EVENT_ANY_ID,
                                set_espnow_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                direct_loop,
                                EVENTS_MANAGER,
                                ESP_EVENT_ANY_ID,
                                set_screen_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET,
                                INIT_SNTP,
                                init_sntp_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET,
                                STOP_SNTP,
                                init_sntp_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                slow_service_loop,
                                WIFI_SET,
                                GET_WEATHER,
                                get_weather_handler,
                                (void *)main_data,
                                NULL
                            ));
    ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                fast_service_loop,
                                WIFI_SET,
                                ESP_EVENT_ANY_ID,
                                set_mode_wifi_handler,
                                (void *)main_data,
                                NULL
                            ));

    
    for(int i=0; i<SIZE_LIST_TASKS; i++){
        xTaskCreate(
            sevice_tasks[i].pTask, 
            "service",
            sevice_tasks[i].stack, 
            (void * const)main_data, 
            sevice_tasks[i].priority,
            NULL
        );
    }
    EventBits_t xEventGroup;
    do{
        send_hello();
        xEventGroup = xEventGroupWaitBits(dwin_event_group, BIT_DWIN_RESPONSE_OK, false, false, 1000);
    }while(!(xEventGroup&BIT_DWIN_RESPONSE_OK));
    // dwin_set_pic(NO_WEATHER_PIC);
    set_color(GREEN, WHITE);

// clear_screen();
// uart_write_bytes(UART_DWIN, send, 20);
// send_str_dwin(send);
    // vTaskDelay(1500/portTICK_PERIOD_MS);
    int8_t t[] = {0, 0, 0, 0, 0, 0};
    // get_y_points(t, 5, 50);
    uint16_t  points[]= {10, 13, 40, 14, 10,45, 15,13, 25,22};

    uint16_t *tu = get_y_points(t, 6, 100);
    print_lines(tu, 6, 50, 470, 260);
    // print_broken_line(points, 10, 10, 100);
    // print_histogram(points, 7, 10, 400, 260);
    // set_text_box(10, 10, 475, 267);
    // fill_area(20, 20, RED);

//  uart_write_bytes(UART_DWIN, points, 4);


    // xEventGroupSetBits(dwin_event_group, BIT_SSID_FOUND|BIT_IS_TIME|BIT_CON_STA_OK|BIT_SEN_2);
    // start_ap();
    // start_espnow();
    // esp_event_post_to(slow_service_loop, ESPNOW_SET, STOP_ESPNOW, NULL, 0, WAIT_SERVICE);
    // vTaskDelay(5000/portTICK_PERIOD_MS);
    // start_espnow();
    // esp_event_post_to(slow_service_loop, ESPNOW_SET, PAUSE_ESPNOW, NULL, 0, WAIT_SERVICE);
    // vTaskDelay(10000/portTICK_PERIOD_MS);
    // start_espnow();

    // esp_event_post_to(
    //         direct_loop,
    //         EVENTS_MANAGER,
    //         MAIN_TASK,
    //         NULL,
    //         0,
    //         TIMEOUT_PUSH_KEY
    //     );

}





void check_net_data_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)
{
    EventBits_t xEventGroup = xEventGroupGetBits(dwin_event_group);
    if(xEventGroup&BIT_CON_STA_OK){
        write_memory(main_data, DATA_PWD);
        write_memory(main_data, DATA_SSID);
    } else {
        read_memory(main_data, DATA_PWD);
        read_memory(main_data, DATA_SSID);
    }
}

void test_clock_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)                                                     
{
    send_str_dwin(asctime(get_time_tm()));
}

void set_screen_handler(void* main_data, esp_event_base_t base, int32_t new_screen, void* event_data)
{
    static esp_event_handler_instance_t
                        direction_handler, 
                        show_handler, 
                        service_handler;
    if(direction_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        direct_loop, 
                        EVENTS_DIRECTION, 
                        ESP_EVENT_ANY_ID,  
                        &direction_handler
                    ));
        direction_handler = NULL;
    }
    if(show_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        show_loop, 
                        EVENTS_SHOW, 
                        ESP_EVENT_ANY_ID,  
                        show_handler
                    ));
        show_handler = NULL;
    }
    if(service_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_unregister_with(
                        slow_service_loop, 
                        EVENTS_SERVICE, 
                        ESP_EVENT_ANY_ID,  
                        service_handler)
                    );
        service_handler = NULL;
    }
    vTaskDelay(TIMEOUT_SEND_EVENTS);
    if(screens_handlers[new_screen].main_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                    direct_loop, 
                                    EVENTS_DIRECTION, 
                                    ESP_EVENT_ANY_ID, 
                                    screens_handlers[new_screen].main_handler, 
                                    main_data,
                                    &direction_handler
                                ));
    }
    if(screens_handlers[new_screen].show_handler) {
        ESP_ERROR_CHECK(esp_event_handler_instance_register_with(
                                    show_loop, 
                                    EVENTS_SHOW, 
                                    ESP_EVENT_ANY_ID, 
                                    screens_handlers[new_screen].show_handler, 
                                    main_data,
                                    &show_handler
                                ));
    }
    esp_event_post_to(direct_loop, EVENTS_DIRECTION, KEY_INIT, NULL, 0, TIMEOUT_SEND_EVENTS);
}

void direction_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(direct_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_FAST_LOOP);
    }
}


 void show_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(show_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_FAST_LOOP);
    }
}

 void fast_services_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(fast_service_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_FAST_LOOP);
    }
}


void slow_services_task(void *pv)
{
    vTaskDelay(DEALAY_START_TASK);
    while(1) {
        esp_event_loop_run(slow_service_loop, TICKS_TO_RUN_LOOP);
        vTaskDelay(DELAY_SLOW_LOOP);
    }
}

void vApplicationIdleHook( void )
{ 
    TickType_t us_time_sleep = TIMER_WAKEUP_LONG_TIME_US;
    while (1) {
        sleep_dwin(us_time_sleep);
    }
}
