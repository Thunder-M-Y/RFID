#pragma once
#include "esp_log.h"
#include "mqtt_client.h"




// #define TOPIC_GET "/" CONFIG_MQ_PRODUCTKEY "/" CONFIG_MQ_DEVICENAME "/user/get"                  // 状态获取
// #define TOPIC_update "/" CONFIG_MQ_PRODUCTKEY "/" CONFIG_MQ_DEVICENAME "/user/update" 
// #define TOPIC_REPLY "/sys/" CONFIG_MQ_PRODUCTKEY "/" CONFIG_MQ_DEVICENAME "/thing/event/property/post_reply" // 设置回复
// #define TOPIC_POST "/sys/" CONFIG_MQ_PRODUCTKEY "/" CONFIG_MQ_DEVICENAME "/thing/event/property/post"    // 上报数据

#define TOPIC_SUB   "testtopic/12"
#define TOPIC_POST  "testtopic/1"

#define MQTT_BROKER_URL CONFIG_MQ_BROKER_URL
#define MQTT_BROKER_PORT CONFIG_MQ_BROKER_PORT
#define MQTT_USERNAME CONFIG_MQ_BROKER_USER
#define MQTT_PASSWORD CONFIG_MQ_BROKER_PASSWD
#define MQTT_CLIENTID CONFIG_MQ_CLIENTID



void esp_mqtt_start();