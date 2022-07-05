#include <Arduino.h>

#include "ros.h"
#include "stdio.h"
#include "esp_chatter.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include <Get_data.h>
#include <ESP32CAN.h>
#include <CAN_config.h>
//-----------------------------------------------------
extern ros::NodeHandle nh;

extern int8_t speed_lf;
extern int8_t speed_rf;
extern int8_t speed_rb;
extern int8_t speed_lb;

extern uint8_t speed_lf_pub;
extern uint8_t speed_rf_pub;
extern uint8_t speed_rb_pub;
extern uint8_t speed_lb_pub;

//-----------------------------------------------------

unsigned long timing;
unsigned long timing_CAN;
unsigned long timing_PUB;
void get_accum_charge();
void set_speed_CAN();
void work_with_CAN();

// --- CAN шина ---
CAN_device_t CAN_cfg;             // Конфигурация CAN
unsigned long previousMillis = 0; // Время последней отправки CAN сообщения
const int interval = 500;         // Интервал отправки CAN сообщений (мс)
const int rx_queue_size = 115;     // Размер буфера приёма
constexpr gpio_num_t CAN_TX_PIN = GPIO_NUM_19;
constexpr gpio_num_t CAN_RX_PIN = GPIO_NUM_23;

// --- Аккумулятор ---
constexpr uint8_t ACCUM_PIN = GPIO_NUM_14;
constexpr uint8_t JETSON_PIN = GPIO_NUM_4;
uint32_t charge = 0;
uint8_t charge_in_percent = 0;
uint32_t charge_send_time = 5000;     
uint32_t CAN_send_time = 100; 
uint32_t PUB_send_time = 10;                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                        


bool receive_flag = false;
uint32_t receive_time = 0;

void setup() {
    Serial.begin(460800);

    pinMode(ACCUM_PIN, INPUT);
    pinMode(JETSON_PIN, OUTPUT);

    digitalWrite(JETSON_PIN, HIGH);
    delay(3000);
    digitalWrite(JETSON_PIN, LOW);

    // Инициализация CAN шины
    CAN_cfg.tx_pin_id = CAN_TX_PIN;
    CAN_cfg.rx_pin_id = CAN_RX_PIN;
#if defined( WIFI_LoRa_32 ) || defined( WIFI_LoRa_32_V2 )
    // ESP типа heltec_wifi_lora_32 или heltec_wifi_lora_32_v2
    CAN_cfg.speed = CAN_SPEED_500KBPS;
#elif ~(defined( WIFI_LoRa_32 ) || defined( WIFI_LoRa_32_V2 ))
    // Другие ESP
    CAN_cfg.speed = CAN_SPEED_1000KBPS;
#endif
    CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
    ESP32Can.CANInit();

    rosserial_setup();  // rosserial initialisation
}


void loop() {

    if (millis() - timing > charge_send_time) { 
        timing = millis(); 
        get_accum_charge();
    }
    //work_with_CAN();
    if (millis() - timing_CAN > CAN_send_time) { 
        timing_CAN = millis(); 
        set_speed_CAN();
    }
    if (millis() - timing_PUB > PUB_send_time) { 
        //rosserial_publish();
        //vTaskDelay(1);
    }
    nh.spinOnce();
    vTaskDelay(10);
}
    
void set_speed_CAN() {
                                
        uint8_t dir_lf = 2;
        uint8_t dir_rf = 2;
        uint8_t dir_rb = 2;
        uint8_t dir_lb = 2;
        int8_t speed_lf_can = 0;
        int8_t speed_rf_can = 0;
        int8_t speed_rb_can = 0;
        int8_t speed_lb_can = 0;

        if (speed_lf > 0) {
            dir_lf = 1;
            speed_lf_can = speed_lf;
        }
        else if (speed_lf < 0) {
            dir_lf = 0;
            speed_lf_can = -speed_lf;
        }
        else {
            dir_lf = 2;
            speed_lf_can = speed_lf;
        }

        if (speed_rf > 0) {
            dir_rf = 1;
            speed_rf_can = speed_rf;
        }
        else if (speed_rf < 0) {
            dir_rf = 0;
            speed_rf_can = -speed_rf;
        }
        else {
            dir_rf = 2;
            speed_rf_can = speed_rf;
        }

        if (speed_rb > 0) {
            dir_rb = 1;
            speed_rb_can = speed_rb;
        }
        else if (speed_rb < 0) {
            dir_rb = 0;
            speed_rb_can = -speed_rb;
        }
        else {
            dir_rb = 2;
            speed_rb_can = speed_rb;
        }

        if (speed_lb > 0) {
            dir_lb = 1;
            speed_lb_can = speed_lb;
        }
        else if (speed_lb < 0) {
            dir_lb = 0;
            speed_lb_can = -speed_lb;
        }
        else {
            dir_lb = 2;
            speed_lb_can = speed_lb;
        }

        CAN_frame_t wheel_frame;
        wheel_frame.FIR.B.FF = CAN_frame_std;
        wheel_frame.MsgID = 0x1;
        wheel_frame.FIR.B.DLC = 8;
        wheel_frame.data.u8[0] = dir_lf;
        wheel_frame.data.u8[1] = speed_lf_can;
        wheel_frame.data.u8[2] = dir_rf;
        wheel_frame.data.u8[3] = speed_rf_can;
        wheel_frame.data.u8[4] = dir_rb;
        wheel_frame.data.u8[5] = speed_rb_can;
        wheel_frame.data.u8[6] = dir_lb;
        wheel_frame.data.u8[7] = speed_lb_can;
        ESP32Can.CANWriteFrame(&wheel_frame);                
}

void get_accum_charge() {
    long accum_temp = 0;
    for (int i = 0; i < 10; ++i) {
        accum_temp += analogRead(ACCUM_PIN);
    }
    long accum_adc = accum_temp / 10;
    const long out_max = 100;
    const long out_min = 0;
    const long in_max = 2888;
    const long in_min = 2400;
    const long dividend = out_max - out_min;
    const long divisor = in_max - in_min;
    if (accum_adc > in_max) {accum_adc = in_max;}
    if (accum_adc < in_min) {accum_adc = in_min;}

    const long delta = accum_adc - in_min;
    charge_in_percent = static_cast<uint8_t>((delta * dividend + (divisor / 2)) / divisor + out_min);
    //Serial.println(charge_in_percent);
}

 void work_with_CAN(){
    CAN_frame_t rx_frame;
    Get_data data;
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 3 * portTICK_PERIOD_MS) == pdTRUE) {
        if ((rx_frame.MsgID > ID_WHEELS_OUT) && (rx_frame.MsgID < ID_WHEEL_LF)) {
        switch(rx_frame.MsgID) {
            //US1
            case ID_US_F:
            break;
            //US2
            case ID_US_FR:
            break;
            //US3
            case ID_US_FL:
            break;
            //US4
            case ID_US_L:
            break;
            //US5
            case ID_US_R:
            break;
            //US6
            case ID_US_B:
            break;
            //US7
            case ID_US_BR:
            break;
            //US8
            case ID_US_BL:
            break;
            default: { 
                break;
            }
        }
    }
        else if (rx_frame.MsgID == ID_WHEEL_LF) {
            if (rx_frame.data.u8[1] == 1) {
                speed_lf_pub = rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 2) {
                speed_lf_pub = -rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 0) {
                speed_lf_pub = 0;
            }
        }
        else if (rx_frame.MsgID == ID_WHEEL_RF) {
            if (rx_frame.data.u8[1] == 1) {
                speed_rf_pub = rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 2) {
                speed_rf_pub = -rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 0) {
                speed_rf_pub = 0;
            }
        }
        else if (rx_frame.MsgID == ID_WHEEL_RB) {
            if (rx_frame.data.u8[1] == 1) {
                speed_rb_pub = rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 2) {
                speed_rb_pub = -rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 0) {
                speed_rb_pub = 0;
            }
        }
        else if (rx_frame.MsgID == ID_WHEEL_LB) {
            if (rx_frame.data.u8[1] == 1) {
                speed_lb_pub = rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 2) {
                speed_lb_pub = -rx_frame.data.u8[0];
            }
            else if (rx_frame.data.u8[1] == 0) {
                speed_lb_pub = 0;
            }
        }                                                                                                                                                                                                                                                                                                                                                                                                                               
    }
}
