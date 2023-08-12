#include "my_mqtt.h"


static const char *TAG = "MQTT";

// 证书
const char *server_cert =
    "-----BEGIN CERTIFICATE-----\n"
    "MIIDrzCCApegAwIBAgIQCDvgVpBCRrGhdWrJWZHHSjANBgkqhkiG9w0BAQUFADBh\n"
    "MQswCQYDVQQGEwJVUzEVMBMGA1UEChMMRGlnaUNlcnQgSW5jMRkwFwYDVQQLExB3\n"
    "d3cuZGlnaWNlcnQuY29tMSAwHgYDVQQDExdEaWdpQ2VydCBHbG9iYWwgUm9vdCBD\n"
    "QTAeFw0wNjExMTAwMDAwMDBaFw0zMTExMTAwMDAwMDBaMGExCzAJBgNVBAYTAlVT\n"
    "MRUwEwYDVQQKEwxEaWdpQ2VydCBJbmMxGTAXBgNVBAsTEHd3dy5kaWdpY2VydC5j\n"
    "b20xIDAeBgNVBAMTF0RpZ2lDZXJ0IEdsb2JhbCBSb290IENBMIIBIjANBgkqhkiG\n"
    "9w0BAQEFAAOCAQ8AMIIBCgKCAQEA4jvhEXLeqKTTo1eqUKKPC3eQyaKl7hLOllsB\n"
    "CSDMAZOnTjC3U/dDxGkAV53ijSLdhwZAAIEJzs4bg7/fzTtxRuLWZscFs3YnFo97\n"
    "nh6Vfe63SKMI2tavegw5BmV/Sl0fvBf4q77uKNd0f3p4mVmFaG5cIzJLv07A6Fpt\n"
    "43C/dxC//AH2hdmoRBBYMql1GNXRor5H4idq9Joz+EkIYIvUX7Q6hL+hqkpMfT7P\n"
    "T19sdl6gSzeRntwi5m3OFBqOasv+zbMUZBfHWymeMr/y7vrTC0LUq7dBMtoM1O/4\n"
    "gdW7jVg/tRvoSSiicNoxBN33shbyTApOB6jtSj1etX+jkMOvJwIDAQABo2MwYTAO\n"
    "BgNVHQ8BAf8EBAMCAYYwDwYDVR0TAQH/BAUwAwEB/zAdBgNVHQ4EFgQUA95QNVbR\n"
    "TLtm8KPiGxvDl7I90VUwHwYDVR0jBBgwFoAUA95QNVbRTLtm8KPiGxvDl7I90VUw\n"
    "DQYJKoZIhvcNAQEFBQADggEBAMucN6pIExIK+t1EnE9SsPTfrgT1eXkIoyQY/Esr\n"
    "hMAtudXH/vTBH1jLuG2cenTnmCmrEbXjcKChzUyImZOMkXDiqw8cvpOp/2PV5Adg\n"
    "06O/nVsJ8dWO41P0jmP6P6fbtGbfYmbW0W5BjfIttep3Sp+dWOIrWcBAI+0tKIJF\n"
    "PnlUkiaY4IBIqDfv8NZ5YBberOgOzW6sRBc4L0na4UU+Krk2U886UAb3LujEV0ls\n"
    "YSEY1QSteDwsOoBrp+uvFRTp2InBuThs4pFsiv9kuXclVzDAGySj4dzp30d8tbQk\n"
    "CAUw7C29C79Fv1C5qfPrmAESrciIxpg0X40KPMbp1ZWVbd4=\n"
    "-----END CERTIFICATE-----\n";

/**
 * @brief MQTT 事件环函数
 */
static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;      // 获得 MQTT 事件句柄
    esp_mqtt_client_handle_t client = event->client; // 从 MQTT 事件句柄中获得客户端句柄
    int msg_id;

    switch ((esp_mqtt_event_id_t)event_id)
    {
    case MQTT_EVENT_CONNECTED: // 连接代理成功事件
        ESP_LOGI(TAG, "MQTT_EVENT_CONNECTED");
        // MQTT 订阅主题为 /topic/test 的消息
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_SUB, 0);
        ESP_LOGI(TAG, "发送订阅成功, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, "led/control", 0);
        ESP_LOGI(TAG, "发送订阅成功, msg_id=%d", msg_id);
        break;
    case MQTT_EVENT_DISCONNECTED: // 连接断开事件
        ESP_LOGI(TAG, "MQTT_EVENT_DISCONNECTED");
        break;
    case MQTT_EVENT_SUBSCRIBED: // 成功订阅事件
        ESP_LOGI(TAG, "MQTT_EVENT_SUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_UNSUBSCRIBED: // 取消订阅事件
        ESP_LOGI(TAG, "MQTT_EVENT_UNSUBSCRIBED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_PUBLISHED: // 发布成功事件
        ESP_LOGI(TAG, "MQTT_EVENT_PUBLISHED, msg_id=%d", event->msg_id);
        break;
    case MQTT_EVENT_DATA: // 收到数据
        ESP_LOGI(TAG, "MQTT_EVENT_DATA");
        ESP_LOGI(TAG, "TOPIC=%.*s\r", event->topic_len, event->topic);
        ESP_LOGI(TAG, "Qos=%d\r", event->qos);
        ESP_LOGI(TAG, "DATA=%.*s\r", event->data_len, event->data);
        if (strncmp(event->topic, "led/control", event->topic_len) == 0)
        {
            // 收到控制LED的消息
            if (strncmp(event->data, "on", event->data_len) == 0)
            {
                ESP_LOGI(TAG, "Turning LED ON");
                // 在这里控制LED点亮的操作
                gpio_set_level(LED1_PIN, 1);
            }
            else if (strncmp(event->data, "off", event->data_len) == 0)
            {
                ESP_LOGI(TAG, "Turning LED OFF");
                // 在这里控制LED关闭的操作
                gpio_set_level(LED1_PIN, 0);
            }
        }
        break;
    case MQTT_EVENT_ERROR: // MQTT 连接发生错误
        ESP_LOGI(TAG, "MQTT_EVENT_ERROR");
        break;
    case MQTT_EVENT_BEFORE_CONNECT: // 断开链接之前的事件
        ESP_LOGI(TAG, "MQTT_EVENT_BEFORE_CONNECT");
        break;
    case MQTT_EVENT_DELETED: // 通知删除内部发件箱中的一条消息
        ESP_LOGI(TAG, "MQTT_EVENT_DELETED");
        break;
    default: // 其他未定义事件
        ESP_LOGI(TAG, "Other event id:%d", event->event_id);
        break;
    }
}

/**
 * @brief 启动 MQTT 客户端
 */
void esp_mqtt_start()
{
    gpio_config_t gpio_cnf = {
        .mode = GPIO_MODE_INPUT_OUTPUT,
        .pin_bit_mask = 1ULL << LED1_PIN,
    };
    gpio_config(&gpio_cnf);
    gpio_set_level(LED1_PIN, 1);
    // 配置 MQTT URI
    // esp_mqtt_client_config_t mqtt_cfg = {
    //     .uri = MQTT_BROKER_URL,
    //     .client_id = MQTT_CLIENTID,
    //     .port = MQTT_BROKER_PORT,
    //     .username = MQTT_USERNAME,
    //     .password = MQTT_PASSWORD,
    // };

    esp_mqtt_client_config_t mqtt_cfg = {
        // .event_handle = mqtt_event_handler,              //MQTT事件
        .uri = "mqtts://t700b9e0.ala.cn-hangzhou.emqxsl.cn",
        //.host = "192.168.236.130", // MQTT服务器IP
        //.host = "192.168.172.116", // MQTT服务器IP
        .port = 8883,         // 端口
        .username = "admin",  // 用户名
        .password = "123456", // 密码
        .cert_pem = server_cert,
    };
    // 初始化 MQTT 客户端
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // 注册事件
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // 启动 MQTT 客户端
    esp_mqtt_client_start(client);

    // 发布-测试
    // esp_mqtt_client_publish(client, TOPIC_POST, "{params:{Humidity:55,temperature:34,LED1Switch:0}}", 0, 1, 0);
}
