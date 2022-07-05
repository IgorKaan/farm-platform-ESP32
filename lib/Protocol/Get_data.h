#include <Arduino.h>
#define ERR_ID 30013        //неверный ID при передаче по CAN-шине

enum Symbol{
    US_MAX = 1,             //Максмальное число символов полезных данных в принимаемом с МК пакете для УЗ-датчика
    WHEEL_MAX = 2,          //Максмальное число символов полезных данных в принимаемом с МК пакете для ИК-датчика
    SERIAL_US = 4,          //Количество символов, отведенных под служебные поля для УЗ-датчика
    SERIAL_W = 5,           //Количество символов, отведенных под служебные поля для двигателей
};
enum ID_CAN{
    ID_WHEELS_OUT = 1,      //CAN-ID исходящий для двигателей 
    ID_US_F = 11,           //CAN-ID для переднего УЗ
    ID_US_FR = 12,          //CAN-ID для левого УЗ
    ID_US_FL = 13,          //CAN-ID для правого УЗ
    ID_US_L = 14,           //CAN-ID для переднего правого УЗ
    ID_US_R = 15,           //CAN-ID для переднего левого УЗ
    ID_US_B = 16,           //CAN-ID для заднего УЗ
    ID_US_BR = 17,          //CAN-ID для заднего правого УЗ
    ID_US_BL = 18,          //CAN-ID для заднего левого УЗ
    ID_WHEEL_LF = 26,       //CAN-ID входящий для левого переднего двигателя 
    ID_WHEEL_RF = 27,       //CAN-ID входящий для правого переднего двигателя
    ID_WHEEL_RB = 28,       //CAN-ID входящий для правого заднего двигателя
    ID_WHEEL_LB = 29,       //CAN-ID входящий для левого заднего двигателя
};
/*
    Поля и методы данного класса принимают данные с МК, обрабатывают их и передают в виде пакета пользователю на ПК.
*/
class Get_data{
    private: 
    char* tmp;
    public:
    Get_data();
    ~Get_data();
    //Функция 
    void print(uint8_t *data, uint32_t ID);
    void print(int32_t data, uint32_t ID);
    //Функция обработки данных для 
    void format_byte(uint8_t *data, uint8_t len, uint8_t size);
    void format_byte(int32_t data, uint8_t len, uint8_t size);
};