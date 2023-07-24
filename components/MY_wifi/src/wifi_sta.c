#include "wifi_sta.h"

static const char *TAG = "WIFI";
static TaskHandle_t app_task;  // 事件处理任务
static esp_netif_t *netif_sta; // Wi-Fi netif 句柄
static int s_retry_num = 0;    // 重连次数
static esp_event_handler_instance_t instance_any_id;
static esp_event_handler_instance_t instance_got_ip;
static wifi_callback_func callback_fn = NULL;  // 回调函数


/**
 * @brief 事件处理回调函数
 */
static void event_handler(void *arg, esp_event_base_t event_base, int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
    {
        ESP_LOGI(TAG, "WiFi 启动成功，准备连接到 AP");
        esp_wifi_connect();
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_CONNECTED)
    {
        ESP_LOGI(TAG, "WiFi 成功连接到 AP");
    }
    else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
    {
        if (s_retry_num < ESP_MAXIMUM_RETRY)
        {
            esp_wifi_connect();
            s_retry_num++;
            ESP_LOGI(TAG, "重新连接到 AP");
        }
        else
        {
            xTaskNotify(app_task, WIFI_FAIL_BIT, eSetBits);
        }
        ESP_LOGI(TAG, "连接 AP 失败");
    }
    else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP)
    {
        ip_event_got_ip_t *event = (ip_event_got_ip_t *)event_data;
        ESP_LOGI(TAG, "获取IP:" IPSTR, IP2STR(&event->ip_info.ip));
        s_retry_num = 0;
        xTaskNotify(app_task, WIFI_CONNECTED_BIT, eSetBits);
    }
}

/**
 * @brief 事件处理任务入口
 */
static void task_evnet_app_entry(void *param)
{
    while (1)
    {
        uint32_t notify_value;
        xTaskNotifyWait(0x00, 0x00, &notify_value, portMAX_DELAY);
        if (notify_value & WIFI_CONNECTED_BIT)
        {
            // 这里的成功指的是连接 AP 成功，并且成功获取了 IP地址
            ESP_LOGI(TAG, "Wi-Fi 成功连接到热点 SSID:%s", WIFI_SSID); // Wi-Fi 连接成功
            // 创建服务器
            if(callback_fn != NULL){
                callback_fn();
            }
            // start_tcp_server();
        }
        else if (notify_value & WIFI_FAIL_BIT)
        {
            ESP_LOGE(TAG, "W-Fi 连接热点失败 SSID:%s", WIFI_SSID); // Wi-Fi 连接失败
        }
    }
}

// static int s_retry_num = 0;             // 重连次数
/**
 * @brief 第一阶段：初始化阶段
 * Wi-Fi STA 模式初始化阶段，连接到指定的 AP 中
 * 0. 初始化 NVS
 * 1. 创建和初始化 LWIP
 * 2. 创建和初始化 事件环
 * 3. 创建和初始化 Wi-Fi
 * 4. 创建应用程序任务
 */
void wifi_sta_init()
{
    // 0. 初始化 NVS
    ESP_ERROR_CHECK(nvs_flash_init());

    // 1. 调用 esp_netif_init() 创建 LwIP 核心任务，初始化LwIP相关工作
    ESP_ERROR_CHECK(esp_netif_init());

    // 2. 调用esp_event_loop_create()创建系统Event任务（事件环），初始化应用事件回调函数。
    ESP_ERROR_CHECK(esp_event_loop_create_default());
    // 注册相关事件
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    // 3.1. 调用esp_netif_create_default_wifi_ap()或esp_netif_create_default_wifi_sta()创建默认网络接口实例绑定站或具有 TCP/IP 堆栈的 AP。
    netif_sta = esp_netif_create_default_wifi_sta(); // 创建默认的Wifi连接

    // 3.2. 调用esp_wifi_init()创建Wi-Fi驱动任务和初始化Wi-Fi驱动。
    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // 创建默认的配置函数
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));
    ESP_LOGI(TAG, "WiFi 初始化完毕");

    // 4. 创建应用程序任务
    xTaskCreate(task_evnet_app_entry, "APP-Task", 1024 * 4, NULL, 10, &app_task);

    // 5. 配置 Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    wifi_config_t wifi_cfg = {
        .sta = {
            .ssid = WIFI_SSID,
            .password = WIFI_PASSWORD}};
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_cfg));

    // 6. 启动 Wi-Fi
    ESP_ERROR_CHECK(esp_wifi_start());
    ESP_LOGI(TAG, "Wi-Fi 启动成功...");
}

/**
 * 设置回调函数
*/
void wifi_set_connected_callback(wifi_callback_func cb){
    callback_fn = cb;
}