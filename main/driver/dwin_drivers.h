#pragma once 

#include "dwin_commands.h"
#include "dwin_common.h"

/*data identifier from dwin*/
#define KEY_READ_COMMAND (0x78)
#define KEY_GET_CLOCK (155)

/*index byte in RX buffer*/
#define INDEX_IDENTIF_DATA_IN_RX    (0)
#define INDEX_START_DATA_IN_RX      (2)

#define init_update_dwin()			uart_write_bytes(UART_DWIN, INIT_UPDATE, sizeof(INIT_UPDATE))

#define send_hello()  				uart_write_bytes(UART_DWIN, HELLO_COMMAND, sizeof(HELLO_COMMAND))
#define print_end()					uart_write_bytes(UART_DWIN, END, sizeof(END));

#define dwin_buzer(loud) 			do{ 													\
										FULL_COMMAND_BUZZER[INDEX_VARIABLE_VALUE] = loud; 	\
										uart_write_bytes(UART_DWIN, FULL_COMMAND_BUZZER, sizeof(FULL_COMMAND_BUZZER)); \
									}while(0)

#define dwin_set_brightness(brightness) 	while((brightness) >= 10 && (brightness) <= 100){ 					\
												brightness = (brightness) / 2 + 13; 							\
												SET_BRIGHT[INDEX_VARIABLE_VALUE] = convert_to_hex(brightness); 	\
												uart_write_bytes(UART_DWIN, SET_BRIGHT, sizeof(SET_BRIGHT));												  \
												break; 															\
											}
#define dwin_set_pic(picture_id)    															\
									while((picture_id) < END_LIST_PIC){ 						\
										SET_PIC[INDEX_VARIABLE_VALUE] = picture_id; 			\
										uart_write_bytes(UART_DWIN, SET_PIC, sizeof(SET_PIC));  \
										break; 													\
									}
#define hide_rect() 				uart_write_bytes(UART_DWIN, RECTANGLE_OFF, sizeof(RECTANGLE_OFF))
#define cancel_text_box()			uart_write_bytes(UART_DWIN, CANCEL_TEXT_BOX, sizeof(CANCEL_TEXT_BOX))
#define clear_screen()				uart_write_bytes(UART_DWIN, CLEAR_SCREEN, sizeof(CLEAR_SCREEN))

void print_start(uint16_t row, uint16_t column, 
					const uint16_t text_color, 
					const uint8_t font) ;
void save_pic(const uint8_t pic);
void set_text_box(const uint16_t x_s, 
                    const uint16_t y_s, 
                    const uint16_t x_e, 
                    const uint16_t y_e);
void set_color(const uint16_t foreground, const uint16_t background);
void print_circle(const uint16_t x, 
                    const uint16_t y, 
                    const uint16_t radius, 
                    const bool fill);
void print_lines(const uint16_t *points, 
                const int number_point, 
                bool foreground);
void print_rect(const uint16_t x_s, 
				const uint16_t y_s, 
				const uint16_t x_e, 
				const uint16_t y_e);
void print_broken_line(const uint16_t *y_points, 
                const int points_number,
                const uint16_t x_start,
                const uint16_t x_end);
void send_chunc(const char *data, const size_t data_len);
void dwin_clock_on(const uint16_t row, 
							const uint16_t column, 
							const uint16_t text_color, 
							const uint8_t font);
#define dwin_clock_get() 	uart_write_bytes(UART_DWIN, GET_TIME, sizeof(GET_TIME))
void dwin_clock_set(struct tm *tmptr);