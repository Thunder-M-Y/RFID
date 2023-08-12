#include <stdio.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"

#include "wifi_sta.h"
#include "my_mqtt.h"

#include "rc522.h"




void app_main(void)
{
   
    wifi_set_connected_callback(esp_mqtt_start);
    // 启动 WiFi
    wifi_sta_init(); // 初始化 Wi-Fi 并连接到热点
    // esp_rc522_start();
    vTaskDelete(NULL);
}
