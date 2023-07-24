#pragma once
#include "nvs_flash.h"
#include "esp_wifi.h"
#include "esp_netif.h"
#include "esp_log.h"

#define WIFI_SSID "3203"            // WiFi SSID
#define WIFI_PASSWORD "32033203" // WiFi 密码
//#define WIFI_SSID CONFIG_MWF_SSID
//#define WIFI_PASSWORD CONFIG_MWF_PASSWROD
#define ESP_MAXIMUM_RETRY 5 // WiFi 连接重试次数

#define WIFI_CONNECTED_BIT BIT0 // 连接成功
#define WIFI_FAIL_BIT BIT1      // 连接失败

typedef void (*wifi_callback_func)(void);    // 回调函数

/**
 * @brief 以 STA 模式初始化 WIFI
 * 使用默认的 SSID 和密码初始化
*/
void wifi_sta_init(void);

/**
 * @brief 连接成功后的回调函数
*/
void wifi_set_connected_callback(wifi_callback_func callback);
