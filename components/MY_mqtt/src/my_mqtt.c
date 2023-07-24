#include "my_mqtt.h"

static const char *TAG = "MQTT";

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
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_GET, 0);
        ESP_LOGI(TAG, "发送订阅成功, msg_id=%d", msg_id);
        msg_id = esp_mqtt_client_subscribe(client, TOPIC_REPLY, 0);
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
    // 配置 MQTT URI
    esp_mqtt_client_config_t mqtt_cfg = {
        .uri = MQTT_BROKER_URL,
        .client_id = MQTT_CLIENTID,
        .port = MQTT_BROKER_PORT,
        .username = MQTT_USERNAME,
        .password = MQTT_PASSWORD,
    };
    // 初始化 MQTT 客户端
    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);

    // 注册事件
    esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL);

    // 启动 MQTT 客户端
    esp_mqtt_client_start(client);

    esp_mqtt_client_publish(client, TOPIC_POST, "{params:{CurrentHumidity:55,CurrentTemperature:34.5,PowerSwitch:0}}", 0, 1, 0);
}
