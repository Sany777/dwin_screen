#include "dwin_server.h"


#define BASE_RESPONSE_SAVE "Save image to position"
#define LEN_RESPONSE_SAVE (sizeof(BASE_RESPONSE_SAVE))
#define BASE_PATH_SAVE_PIC "/sevepic"
#define BASE_PATH_IMG "/img"


static esp_err_t index_redirect_handler(httpd_req_t *req)
{
    httpd_resp_set_status(req, "307 Temporary Redirect");
    httpd_resp_set_hdr(req, "Location", "/index.html");
    httpd_resp_send(req, NULL, 0);  // Response body can be empty
    return ESP_OK;
}

/*index*/
static esp_err_t get_index_handler(httpd_req_t *req)
{
    extern const unsigned char index_html_start[] asm( "_binary_index_html_start" );
    extern const unsigned char index_html_end[] asm( "_binary_index_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)index_html_start, index_html_end - index_html_start); 
    return ESP_OK;
}

static esp_err_t get_css_handler(httpd_req_t *req)
{
    extern const unsigned char style_css_start[] asm( "_binary_style_css_start" );
    extern const unsigned char style_css_end[] asm( "_binary_style_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)style_css_start, style_css_end - style_css_start ); 
    return ESP_OK;
}

static esp_err_t get_js_handler(httpd_req_t *req)
{
    extern const unsigned char script_js_start[] asm( "_binary_script_js_start" );
    extern const unsigned char script_js_end[] asm( "_binary_script_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)script_js_start, script_js_end - script_js_start ); 
    return ESP_OK;
}

/*aqua*/
static esp_err_t get_aqua_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_html_start[] asm( "_binary_aqua_html_start" );
    extern const unsigned char aqua_html_end[] asm( "_binary_aqua_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)aqua_html_start, aqua_html_end - aqua_html_start ); 
    return ESP_OK;
}

static esp_err_t get_aqua_css_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_css_start[] asm( "_binary_aqua_css_start" );
    extern const unsigned char aqua_css_end[] asm( "_binary_aqua_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)aqua_css_start, aqua_css_end - aqua_css_start); 
    return ESP_OK;
}

static esp_err_t get_aqua_js_handler(httpd_req_t *req)
{
    extern const unsigned char aqua_js_start[] asm( "_binary_aqua_js_start" );
    extern const unsigned char aqua_js_end[] asm( "_binary_aqua_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)aqua_js_start, aqua_js_end - aqua_js_start ); 
    return ESP_OK;
}

/*setting*/
static esp_err_t get_setting_css_handler(httpd_req_t *req)
{
    extern const unsigned char setting_css_start[] asm( "_binary_setting_css_start" );
    extern const unsigned char setting_css_end[] asm( "_binary_setting_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)setting_css_start, setting_css_end - setting_css_start ); 
    return ESP_OK;
}

static esp_err_t get_setting_js_handler(httpd_req_t *req)
{
    extern const unsigned char setting_js_start[] asm( "_binary_setting_js_start" );
    extern const unsigned char setting_js_end[] asm( "_binary_setting_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)setting_js_start, setting_js_end - setting_js_start ); 
    return ESP_OK;
}

/*dwin portal*/
static esp_err_t get_dwin_handler(httpd_req_t *req)
{
    extern const unsigned char portal_html_start[] asm( "_binary_dwin_html_start" );
    extern const unsigned char portal_html_end[] asm( "_binary_dwin_html_end" );
    httpd_resp_set_type(req, "text/html");
    httpd_resp_send(req, (const char *)portal_html_start, portal_html_end - portal_html_start); 
    return ESP_OK;
}

static esp_err_t get_dwin_css_handler(httpd_req_t *req)
{
    extern const unsigned char dwin_css_start[] asm( "_binary_dwin_css_start" );
    extern const unsigned char dwin_css_end[] asm( "_binary_dwin_css_end" );
    httpd_resp_set_type(req, "text/css");
    httpd_resp_send(req, (const char *)dwin_css_start, dwin_css_end - dwin_css_start); 
    return ESP_OK;
}

static esp_err_t get_dwin_js_handler(httpd_req_t *req)
{
    extern const unsigned char dwin_js_start[] asm( "_binary_dwin_js_start" );
    extern const unsigned char dwin_js_end[] asm( "_binary_dwin_js_end" );
    httpd_resp_set_type(req, "text/javascript");
    httpd_resp_send(req, (const char *)dwin_js_start, dwin_js_end - dwin_js_start); 
    return ESP_OK;
}

static esp_err_t get_ico_handler(httpd_req_t *req)
{
    extern const unsigned char favicon_ico_start[] asm( "_binary_favicon_ico_start" );
    extern const unsigned char favicon_ico_end[] asm( "_binary_favicon_ico_end" );
    httpd_resp_set_type(req, "image/x-icon");
    httpd_resp_send(req, (const char *)favicon_ico_start, favicon_ico_end - favicon_ico_start); 
    return ESP_OK;
}

static esp_err_t handler_update_dwin(httpd_req_t *req)
{
    int total_len = req->content_len;
    int cur_len = 0;
    int received = 0;
    if (total_len >= SCRATCH_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    init_update_dwin();
    /* check response */
    char * const server_buf = (char *)req->user_ctx;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, server_buf, SCRATCH_SIZE);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            return ESP_FAIL;
        }
        cur_len += received;
    }
    uart_write_bytes(UART_NUM_0, server_buf, total_len);
    return ESP_OK;
}


static esp_err_t handler_update_esp(httpd_req_t *req)
{
    const esp_partition_t *update_partition = NULL;
    esp_ota_handle_t update_handle = 0 ;
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    int cur_len = 0;
    int received = 0;
    if (total_len >= MAX_OTA_SIZE) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    };
    update_partition = esp_ota_get_next_update_partition(NULL);
    DWIN_CHECK(esp_ota_begin(update_partition, OTA_SIZE_UNKNOWN, &update_handle), "", _err_start);
    while (cur_len < total_len) {
        received = httpd_req_recv(req, server_buf, SCRATCH_SIZE-1);
        if(received <= 0 ){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR,"Failed to post control value");
            goto _err;
        }
        cur_len += received;
        DWIN_CHECK(esp_ota_write(update_handle, server_buf, received), "esp_ota_write", _err);
    }
    DWIN_CHECK(esp_ota_end(update_handle), "", _err);
    DWIN_CHECK(esp_ota_set_boot_partition(update_partition), "", _err);
    httpd_resp_sendstr(req, "Update seccessful");
    vTaskDelay(100);
    esp_restart();
    return ESP_OK;
_err:
    esp_ota_abort(update_handle);
_err_start:
    httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Fail update");
    return ESP_FAIL;
}



static esp_err_t handler_set_img(httpd_req_t *req)
{
    const int total_len = req->content_len;
    if (total_len > MAX_LEN_CHUNC_IMG || !total_len) {
        httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "format data is wrong");
        return ESP_FAIL;
    }
    ESP_LOGE(TAG, "str %s\n ", req->uri);
    char * const server_buf = (char *)req->user_ctx;
    int cur_len = 0;
    int received = 0;
    while (cur_len < total_len) {
        received = httpd_req_recv(req, server_buf, total_len);
        if (received <= 0) {
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Failed to post control value");
            send_chunc(NULL, 0);
            return ESP_FAIL;
        }
        cur_len += received;
    }
    send_chunc(server_buf, total_len);
    if(total_len < MAX_LEN_CHUNC_IMG){
        char *pos_pic_str = get_pos_data_str_from_uri(req->uri, BASE_PATH_SAVE_PIC);
        if(!pos_pic_str){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong value");
            return ESP_FAIL;
        }
        int pos_pic = atoi(pos_pic_str);
        if(pos_pic > 255 || pos_pic < 0){
            httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong value");
            return ESP_FAIL;
        }
        save_pic(pos_pic);
    }
    httpd_resp_sendstr(req, "ok");
    return ESP_OK;
}

#define SIZE_COLOR_DATA sizeof(uint16_t)
static esp_err_t handler_set_color(httpd_req_t *req)
{
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong format");
        return ESP_FAIL;
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "semething went wrong");
        return ESP_FAIL;
    }
    server_buf[total_len] = '\0';
    cJSON *root = cJSON_Parse(server_buf);
    char *bcolor = cJSON_GetObjectItem(root, "b")->valuestring;
    char *fcolor = cJSON_GetObjectItem(root, "f")->valuestring;
    if(!bcolor || !fcolor 
        || strnlen(bcolor, SIZE_COLOR_DATA) != SIZE_COLOR_DATA 
        || strnlen(fcolor, SIZE_COLOR_DATA) != SIZE_COLOR_DATA)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong format");
        return ESP_FAIL;
    }
    uint16_t bc_u = (uint16_t) GET_NUMBER(bcolor[0])<<8;
    bc_u |= GET_NUMBER(bcolor[1]);
    uint16_t fc_u = (uint16_t) GET_NUMBER(fcolor[0])<<8;
    fc_u |= GET_NUMBER(fcolor[1]);
    set_color(fc_u, bc_u);
    return ESP_OK;
}

static esp_err_t handler_save_pic(httpd_req_t *req)
{
    char *pos_pic_str = get_pos_data_str_from_uri(req->uri, BASE_PATH_SAVE_PIC);
    ESP_LOGI(TAG, "uri : %s", req->uri);
    if(!pos_pic_str){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong value");
        return ESP_FAIL;
    }
    int pos_pic = atoi(pos_pic_str);
    if(pos_pic > 255 || pos_pic < 0){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong value");
        return ESP_FAIL;
    }
    save_pic(pos_pic);
    char *buf_resp = calloc(1, LEN_RESPONSE_SAVE+5);
    if(buf_resp){
        sprintf(buf_resp,  "%s %d", BASE_RESPONSE_SAVE, pos_pic);
        httpd_resp_sendstr(req, buf_resp);
        free(buf_resp);
    }
    return ESP_OK;
}

static esp_err_t handler_clear_screen(httpd_req_t *req)
{
    clear_screen();
    httpd_resp_sendstr(req, "Screen cleaning");
    return ESP_OK;
}

static esp_err_t handler_send_hello(httpd_req_t *req)
{
    send_hello();
    /* check response */
    httpd_resp_sendstr(req, "Dwin ok");
    return ESP_OK;
}

static esp_err_t handler_close(httpd_req_t *req)
{
    httpd_resp_sendstr(req, "Goodbay!");
    vTaskDelay(100);
    esp_wifi_stop();
    return ESP_OK;
}

static esp_err_t handler_set_network(httpd_req_t *req)
{
    const size_t total_len = req->content_len;
    main_data_t *data_dwin = (main_data_t *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    char *server_buf = malloc(total_len+1);
    if(server_buf == NULL){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "not enough storage");
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "semething went wrong");
        return ESP_FAIL;
    }
    server_buf[received] = 0;
    cJSON *root = cJSON_Parse(server_buf);
    const cJSON *ssid_name_j = cJSON_GetObjectItemCaseSensitive(root, "SSID");
    const cJSON *pwd_wifi_j = cJSON_GetObjectItemCaseSensitive(root, "Password");
    const char *ssid_name, *pwd_wifi;
    size_t pwd_len = 0, ssid_len = 0;
    if(cJSON_IsString(ssid_name_j) && (ssid_name_j->valuestring != NULL)){
        ssid_name = ssid_name_j->valuestring;
        ssid_len = strnlen(ssid_name, SIZE_BUF);
    }
    if(cJSON_IsString(pwd_wifi_j) && (pwd_wifi_j->valuestring != NULL)){
        pwd_wifi = pwd_wifi_j->valuestring;
        pwd_len = strnlen(pwd_wifi, SIZE_BUF);
    }

    if((!pwd_len && !ssid_len) 
        || ssid_len >= MAX_STR_LEN 
        || pwd_len >= MAX_STR_LEN)
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong format data");
        free(server_buf);
        cJSON_Delete(root);
        return ESP_FAIL;
    }
    if(ssid_len){
        memcpy(name_SSID, ssid_name, ssid_len);
        name_SSID[ssid_len] = 0;
        write_memory(data_dwin, DATA_SSID);
    }
    if(pwd_len){
        memcpy(pwd_WIFI, pwd_wifi, pwd_len);
        pwd_WIFI[pwd_len] = 0;
        write_memory(data_dwin, DATA_PWD);
    }
    free(server_buf);
    cJSON_Delete(root);
    httpd_resp_sendstr(req, "Successful");
    return ESP_OK;
}

static esp_err_t handler_set_api(httpd_req_t *req)
{
    const int total_len = req->content_len;
    main_data_t *data_dwin = (main_data_t *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    char *server_buf = malloc(total_len+1);
    if(server_buf == NULL){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "not enough storage");
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "semething went wrong");
        return ESP_FAIL;
    }
    server_buf[received] = '\0';
    cJSON *root = cJSON_Parse(server_buf);
    const cJSON *city_j = cJSON_GetObjectItemCaseSensitive(root, "City");
    const cJSON *key_j = cJSON_GetObjectItemCaseSensitive(root, "Key");
    const char *key = NULL, *city_name = NULL;
    size_t key_len = 0, city_len = 0;
    if(cJSON_IsString(city_j) && (city_j->valuestring != NULL)){
        city_name = city_j->valuestring;
        city_len = strnlen(city_name, SIZE_BUF);
    }
    if(cJSON_IsString(key_j) && (key_j->valuestring != NULL)){
        key = key_j->valuestring;
        key_len = strnlen(key, SIZE_BUF);
    }
    if((city_len == 0 && key_len == 0) 
        || city_len > MAX_STR_LEN 
        || (key_len && key_len != SIZE_API))
    {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "Wrong format input");
        cJSON_Delete(root);
        free(server_buf);
        return ESP_FAIL;
    }
    if(key_len){
        memcpy(api_KEY, key, key_len);
        api_KEY[key_len] = 0;
        write_memory(data_dwin, DATA_API);
    }
    if(city_len){
        memcpy(name_CITY, city_name, city_len); 
        name_CITY[city_len] = 0;
        write_memory(data_dwin, DATA_CITY);
    }
    cJSON_Delete(root);
    free(server_buf);
    httpd_resp_sendstr(req, "Successful");
    return ESP_OK;
}



static esp_err_t handler_get_info(httpd_req_t *req)
{
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    esp_chip_info_t chip_info;
    esp_chip_info(&chip_info);
    cJSON_AddStringToObject(root, "version", IDF_VER);
    cJSON_AddStringToObject(root, "chip", get_chip(chip_info.model));
    cJSON_AddNumberToObject(root, "revision", chip_info.revision);
    const char *sys_info = cJSON_Print(root);
    httpd_resp_sendstr(req, sys_info);
    free((void *)sys_info);
    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t handler_set_time(httpd_req_t *req)
{
    const int total_len = req->content_len;
    char * const server_buf = (char *)req->user_ctx;
    if(total_len > SCRATCH_SIZE){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "semething went wrong");
        return ESP_FAIL;
    }
    server_buf[total_len] = '\0';
    long *time = malloc(sizeof(long));
    if(!time){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "not enough storage");
        return ESP_FAIL;
    }
    *time = atol(server_buf);
    // esp_event_post_to(loop_direct, EVENTS_SET_TIME, UPDATE_TIME_FROM_MS, time, sizeof(long), TIMEOUT_SEND_EVENTS);
    httpd_resp_sendstr(req, "Set time successfully");
    return ESP_OK;
}

#define NOTIF_SEND_EXAMPLE "16:00,16:00,16:00,16:00,16:00,10:00,10:00"
#define SIZE_NOTIF_SEND sizeof(NOTIF_SEND_EXAMPLE)

static esp_err_t handler_get_data(httpd_req_t *req)
{
    main_data_t * data_dwin = (main_data_t *)req->user_ctx;
    char *notif_send = malloc(SIZE_NOTIF_SEND+1);
    if(notif_send == NULL){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "not enough storage");
        return ESP_FAIL;
    }
    httpd_resp_set_type(req, "application/json");
    cJSON *root = cJSON_CreateObject();
    cJSON_AddStringToObject(root, "SSID", name_SSID);
    cJSON_AddStringToObject(root, "Password", pwd_WIFI);
    cJSON_AddStringToObject(root, "Key", api_KEY);
    cJSON_AddStringToObject(root, "City", name_CITY);
    for(size_t day=0, s_i=0; day<SIZE_WEEK; day++){
        notif_send[s_i++] = (GET_NOTIF_HOUR(0, day)%100)/10+'0';
        notif_send[s_i++] = GET_NOTIF_HOUR(0, day)%10+'0';
        notif_send[s_i++] = ':';
        notif_send[s_i++] = (GET_NOTIF_MIN(0, day)%100)/10+'0';
        notif_send[s_i++] = GET_NOTIF_MIN(0, day)%10+'0';
        if(s_i == SIZE_NOTIF_SEND){
            notif_send[s_i] = '\0';
        } else {
            notif_send[s_i++] = ',';
        }   
    }
    cJSON_AddStringToObject(root, "Notification", notif_send);
    const char *data_info = cJSON_Print(root);
    httpd_resp_sendstr(req, data_info);
    free((void *)data_info);
    free(notif_send);
    cJSON_Delete(root);
    return ESP_OK;
}

static esp_err_t notif_handler(httpd_req_t *req)
{
    const int total_len = req->content_len;
    if(total_len > SCRATCH_SIZE){
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "content too long");
        return ESP_FAIL;
    }
    char * const server_buf = malloc(total_len+1);
    const int received = httpd_req_recv(req, server_buf, total_len);
    if (received != total_len) {
        httpd_resp_send_err(req, HTTPD_500_INTERNAL_SERVER_ERROR, "semething went wrong");
        return ESP_FAIL;
    }
    server_buf[total_len] = '\0';
    httpd_resp_sendstr(req, server_buf);
    free(server_buf);
    return ESP_OK;

}


esp_err_t set_run_webserver(const bool start)
{ 
    static main_data_t *data_dwin;
    static char *server_buf;
    static httpd_handle_t server;
    
    if(start){
        if(data_dwin == NULL)data_dwin = malloc(sizeof(main_data_t));
        assert(data_dwin);
        if(server_buf == NULL)server_buf = malloc(SCRATCH_SIZE);
        assert(server_buf);
        read_all_memory(data_dwin);
        httpd_config_t config = HTTPD_DEFAULT_CONFIG();
        config.max_uri_handlers = 27;
        config.uri_match_fn = httpd_uri_match_wildcard;
        esp_err_t e = httpd_start(&server, &config);
        if ( e != ESP_OK){
            return e;
        }
    } else {
        if(server_buf){
           free(server_buf); 
           server_buf = NULL;
        }
        if(data_dwin){
            free(data_dwin);
            data_dwin = NULL;
        }
        return httpd_stop(server);
    } 
     
    httpd_uri_t get_info = {
        .uri      = "/getinfo",
        .method   = HTTP_GET,
        .handler  = handler_get_info,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_info);
    httpd_uri_t savepic = {
        .uri      = "/savepic/*",
        .method   = HTTP_POST,
        .handler  = handler_save_pic,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &savepic);
    httpd_uri_t clear = {
        .uri      = "/clear",
        .method   = HTTP_POST,
        .handler  = handler_clear_screen,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &clear);

    httpd_uri_t get_setting = {
        .uri      = "/data?",
        .method   = HTTP_GET,
        .handler  = handler_get_data,
        .user_ctx = data_dwin
    };
    httpd_register_uri_handler(server, &get_setting);
     httpd_uri_t dwin_uri = {
        .uri      = "/updatedwin",
        .method   = HTTP_POST,
        .handler  = handler_update_dwin,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &dwin_uri);
     httpd_uri_t img_uri = {
        .uri      = "/img/*",
        .method   = HTTP_POST,
        .handler  = handler_set_img,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &img_uri);
     httpd_uri_t set_color_uri = {
        .uri      = "/color",
        .method   = HTTP_POST,
        .handler  = handler_set_color,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &set_color_uri);
     httpd_uri_t hello_uri = {
        .uri      = "/hello",
        .method   = HTTP_POST,
        .handler  = handler_send_hello,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &hello_uri);
     httpd_uri_t close_uri = {
        .uri      = "/close",
        .method   = HTTP_POST,
        .handler  = handler_close,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &close_uri);
     httpd_uri_t net_uri = {
        .uri      = "/Network",
        .method   = HTTP_POST,
        .handler  = handler_set_network,
        .user_ctx = data_dwin
    };
    httpd_register_uri_handler(server, &net_uri);
     httpd_uri_t api_uri = {
        .uri      = "/API",
        .method   = HTTP_POST,
        .handler  = handler_set_api,
        .user_ctx = data_dwin
    };
    httpd_register_uri_handler(server, &api_uri);
     httpd_uri_t time_uri = {
        .uri      = "/time",
        .method   = HTTP_POST,
        .handler  = handler_set_time,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &time_uri);
     httpd_uri_t update_uri = {
        .uri      = "/Update",
        .method   = HTTP_POST,
        .handler  = handler_update_esp,
        .user_ctx = server_buf
    };
    httpd_register_uri_handler(server, &update_uri);

     httpd_uri_t index_uri = {
        .uri      = "/index.html",
        .method   = HTTP_GET,
        .handler  = get_index_handler ,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &index_uri);


     httpd_uri_t aqua_uri = {
        .uri      = "/aqua.html",
        .method   = HTTP_GET,
        .handler  = get_aqua_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &aqua_uri);
     httpd_uri_t get_ico = {
        .uri      = "/favicon.ico",
        .method   = HTTP_GET,
        .handler  = get_ico_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_ico);

     httpd_uri_t get_style = {
        .uri      = "/style.css",
        .method   = HTTP_GET,
        .handler  = get_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_style);

     httpd_uri_t get_script = {
        .uri      = "/script.js",
        .method   = HTTP_GET,
        .handler  = get_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_script);
    httpd_uri_t get_aqua_css = {
        .uri      = "/aqua.css",
        .method   = HTTP_GET,
        .handler  = get_aqua_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_aqua_css);
    httpd_uri_t get_dwin_css = {
        .uri      = "/dwin.css",
        .method   = HTTP_GET,
        .handler  = get_dwin_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_dwin_css);

     httpd_uri_t get_aqua_js = {
        .uri      = "/aqua.js",
        .method   = HTTP_GET,
        .handler  = get_aqua_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_aqua_js);
     httpd_uri_t get_setting_style = {
        .uri      = "/setting.css",
        .method   = HTTP_GET,
        .handler  = get_setting_css_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_setting_style);
     httpd_uri_t get_setting_script = {
        .uri      = "/setting.js",
        .method   = HTTP_GET,
        .handler  = get_setting_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_setting_script);
     httpd_uri_t get_dwin_js = {
        .uri      = "/dwin.js",
        .method   = HTTP_GET,
        .handler  = get_dwin_js_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &get_dwin_js);
     httpd_uri_t get_portal_style = {
            .uri      = "/dwin.html",
            .method   = HTTP_GET,
            .handler  = get_dwin_handler,
            .user_ctx = NULL
        };
    httpd_register_uri_handler(server, &get_portal_style);

     httpd_uri_t notif_uri = {
        .uri      = "/Notification",
        .method   = HTTP_POST,
        .handler  = notif_handler,
        .user_ctx = data_dwin
    };
    httpd_register_uri_handler(server, &notif_uri);
     httpd_uri_t redir_uri = {
        .uri      = "/*",
        .method   = HTTP_GET,
        .handler  = index_redirect_handler,
        .user_ctx = NULL
    };
    httpd_register_uri_handler(server, &redir_uri);


    return ESP_OK;
}