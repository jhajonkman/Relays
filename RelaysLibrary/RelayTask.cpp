//
//  RelayTask
//  C++ code
//  ----------------------------------
//  Developed with embedXcode
//
//  
//  Created by jeroenjonkman on 13-06-15
//


#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include "RelayTask.h"

void RelayTask::setup()
{
    bitWrite(_status,RELAYTASK_STATUS_SETUP_BIT,true);
}

void RelayTask::setTime(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute)
{
#ifdef RelayTask_debug_setTime
    Serial.print("RT:sT mo=");
    Serial.print(month);
    Serial.print(", dom=");
    Serial.print(day_of_month);
    Serial.print(", dow=");
    Serial.print(day_of_week);
    Serial.print(", h=");
    Serial.print(hour);
    Serial.print(", mi=");
    Serial.print(minute);
    Serial.print(", _d=");
    Serial.print(_data);
#endif
    setTask(RELAYTASK_TASK_TIME);
    _data = timeToData(month,day_of_month,day_of_week,hour,minute);
#ifdef RelayTask_debug_setTime
    Serial.print(", _d=");
    Serial.print(_data);
    Serial.println();
#endif
}

void RelayTask::setTemperature(uint8_t operatortype, int value)
{
    setTask(RELAYTASK_TASK_TEMPERATURE);
    _data = operatorToData(operatortype, value);
}

void RelayTask::setHumidity(uint8_t operatortype, int value)
{
    setTask(RELAYTASK_TASK_HUMIDITY);
    _data = operatorToData(operatortype, value);
}

void RelayTask::setLight(uint8_t operatortype, int value)
{
    setTask(RELAYTASK_TASK_LIGHT);
    _data = operatorToData(operatortype, value);
}

void RelayTask::setTrigger(uint8_t delayType, uint16_t delay)
{
    setTask(RELAYTASK_TASK_TRIGGER);
    setTimeout(delayType, delay);
}

void RelayTask::setRelay(uint16_t relay)
{
    _relay = relay & RELAYTASK_RELAY_ALL;
}

bool RelayTask::setTimeout(uint8_t delayType, uint16_t delay)
{
    if (isTimeoutTask()) {
#ifdef RelayTask_debug
        Serial.print("RT:sTo dT=");
        Serial.print(delayType);
        Serial.print(", d=");
        Serial.print(delay);
        Serial.print(", ");
#endif
        _data = ( _data & RELAYTASK_DATA_OPERATOR_MASK ) + ( timeoutToData(delayType, delay) & RELAYTASK_DATA_TIMEOUT_MASK);
#ifdef RelayTask_debug
        printOperatorData();
        Serial.print(", ");
        printTimeoutData();
        Serial.println();
#endif
        return isTimeoutSet();
    }
    return false;
}

void RelayTask::setOn(bool on)
{
    bitWrite(_status, RELAYTASK_STATUS_ON_BIT, on);
}

bool RelayTask::isSetup()
{
    return bitRead(_status,RELAYTASK_STATUS_SETUP_BIT);
}

bool RelayTask::isOn()
{
    return bitRead(_status,RELAYTASK_STATUS_ON_BIT);
}

bool RelayTask::isTimeoutTask()
{
    if (getTask() == RELAYTASK_TASK_TEMPERATURE ||
        getTask() == RELAYTASK_TASK_HUMIDITY ||
        getTask() == RELAYTASK_TASK_LIGHT ||
        getTask() == RELAYTASK_TASK_TRIGGER )
        return true;
    return false;
}

bool RelayTask::isTimeoutSet()
{
    if (!isTimeoutTask())
        return false;
    return (_data & RELAYTASK_DATA_OPERATOR_TIMEOUT) >> 16;
}

uint8_t RelayTask::getTimerType()
{
    if (isTimeoutSet()) {
        return (_data & RELAYTASK_DATA_OPERATOR_DELAYTYPE) >> 17;
    }
    return 0;
}

uint16_t RelayTask::getTimerDelay()
{
    if (isTimeoutSet()) {
        return (_data & RELAYTASK_DATA_OPERATOR_DELAY) >> 20 ;
    }
    return 0;
}

uint32_t RelayTask::timeToData(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute)
{
    uint32_t data = 0x00000000;
    data += month & 0x000F;             // only 4 bits
    data = data << 6;
    data += day_of_month & 0x003F;      // only 6 bits
    data = data << 4;
    data += day_of_week & 0x000F;       // only 4 bits
    data = data << 5;
    data += hour & 0x001F;              // only 5 bits
    data = data << 6;
    data += minute & 0x003F;            // only 6 bits
    return data;
}

uint32_t RelayTask::operatorToData(uint8_t operatortype, int value)
{
    uint8_t mode = 1;
    bool positive = true;
    uint8_t byteValue = 0;
    uint8_t valueType = RELAYTASK_DATA_VALUETYPE_A;
    if (value < 0) {
        positive = false;
        value *= -1; // make positive
    }
    if (value < 256 * RELAYTASK_DATA_PRECISION_A ) {
        valueType = RELAYTASK_DATA_VALUETYPE_A;
        byteValue = value / RELAYTASK_DATA_PRECISION_A;
    } else if (value < 256 * RELAYTASK_DATA_PRECISION_B ) {
        valueType = RELAYTASK_DATA_VALUETYPE_B;
        byteValue = value / RELAYTASK_DATA_PRECISION_B;
    } else if (value < 256 * RELAYTASK_DATA_PRECISION_C ) {
        valueType = RELAYTASK_DATA_VALUETYPE_C;
        byteValue = value / RELAYTASK_DATA_PRECISION_C;
    } else if (value < 256 * RELAYTASK_DATA_PRECISION_D ) {
        valueType = RELAYTASK_DATA_VALUETYPE_D;
        byteValue = value / RELAYTASK_DATA_PRECISION_D;
    }
    uint32_t data = 0x00000000;
    data += mode & 0x03;            // only 2 bits
    data = data << 8;
    data += byteValue & 0xFF;       // only 8 bits
    data = data << 2;
    data += valueType & 0x03;       // only 2 bits
    data = data << 1;
    data += (positive) ? 1 : 0;     // only 1 bit
    data = data << 3;
    data += operatortype & 0x07;    // only 3 bits
    return data;
}

uint32_t RelayTask::timeoutToData(uint8_t delayType, uint16_t delay)
{
    uint32_t data = 0x00000000;
    data += delay & 0x0FFF;         // only 12 bits
    data = data << 3;
    data += delayType & 0x7;        // only 3 bits
    data = data << 1;
    data += 1;                      // only 1 bit
    data = data << 16;
    return data;
}

uint8_t RelayTask::getTask()
{
    return _status >> 4;
}

uint16_t RelayTask::getRelay()
{
    return _relay;
}

void RelayTask::setTask(uint8_t task)
{
    task = task << 4;
    _status = (_status & ( RELAYTASK_STATUS_SETUP | RELAYTASK_STATUS_ON) ) + (task & RELAYTASK_STATUS_TASK);
}

bool RelayTask::checkOperatorOnValue(int checkValue)
{
    if (isTimeoutTask()) { // TimeoutTask is a operator task.
        uint8_t operatorType    = (_data & RELAYTASK_DATA_OPERATOR_OPERATOR);
        bool    positive        = (_data & RELAYTASK_DATA_OPERATOR_SIGNED) >> 3;
        uint8_t valueType       = (_data & RELAYTASK_DATA_OPERATOR_VALUETYPE) >> 4;
        uint8_t value           = (_data & RELAYTASK_DATA_OPERATOR_VALUE) >> 6;
        uint8_t mode            = (_data & RELAYTASK_DATA_OPERATOR_MODE) >> 14;
        int operatorValue = 0;
        switch (valueType) {
            case RELAYTASK_DATA_VALUETYPE_A:
                operatorValue = value * RELAYTASK_DATA_PRECISION_A;
                break;
            case RELAYTASK_DATA_VALUETYPE_B:
                operatorValue = value * RELAYTASK_DATA_PRECISION_B;
                break;
            case RELAYTASK_DATA_VALUETYPE_C:
                operatorValue = value * RELAYTASK_DATA_PRECISION_C;
                break;
            case RELAYTASK_DATA_VALUETYPE_D:
                operatorValue = value * RELAYTASK_DATA_PRECISION_D;
                break;
            default:
                operatorValue = value;
                break;
        }
        if (!positive) {
            operatorValue *= -1;
        }
#ifdef RelayTask_debug
        Serial.print("RT:cOOV cV=");
        Serial.print(checkValue);
        Serial.print(", oV=");
        Serial.print(operatorValue);
        Serial.print(", ");
        printOperatorData();
        Serial.println();
#endif
        switch (operatorType) {
            case RELAYTASK_OPERATOR_EQUAL_TO:
                return checkValue == operatorValue;
                break;
            case RELAYTASK_OPERATOR_GREATER_THAN:
                return checkValue > operatorValue;
                break;
            case RELAYTASK_OPERATOR_LESS_THAN:
                return checkValue < operatorValue;
                break;
            default:
                return false;
                break;
        }
    }
    return false;
}

bool RelayTask::checkTime(time_t time)
{
    return checkTime(month(time),day(time),weekday(time),hour(time),minute(time));
}

bool RelayTask::checkTime(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute)
{
#ifdef RelayTask_debug_checkTime
    Serial.print("RT:cT mo=");
    Serial.print(month);
    Serial.print(",dom=");
    Serial.print(day_of_month);
    Serial.print(",dow=");
    Serial.print(day_of_week);
    Serial.print(",h=");
    Serial.print(hour);
    Serial.print(",mi=");
    Serial.print(minute);
#endif
    // MINUTE
    switch ((_data & RELAYTASK_DATA_TIME_MINUTE)) {
        case RELAYTASK_TIME_MINUTE_ALL:
            // Default not false
            break;
        case RELAYTASK_TIME_MINUTE_5:
            // */5
            if (minute%5 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!M_5");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_MINUTE_15:
            // */15
            if (minute%15 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!M_15");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_MINUTE_30:
            // */30
            if (minute%30 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!M_30");
#endif
                return false;
            }
            break;
        default:
            if ((_data & RELAYTASK_DATA_TIME_MINUTE) != minute) {
#ifdef RelayTask_debug_checkTime
                Serial.print(",_mi!=");
                Serial.println((_data & RELAYTASK_DATA_TIME_MINUTE));
#endif
                return false;
            }
            break;
    }
    // HOUR
    switch ((_data & RELAYTASK_DATA_TIME_HOUR)>>6) {
        case RELAYTASK_TIME_HOUR_ALL:
            // Default not false
            break;
        case RELAYTASK_TIME_HOUR_2:
            // */2
            if (hour%2 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_2");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_3:
            // */3
            if (hour%3 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_3");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_4:
            // */4
            if (hour%4 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_4");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_5:
            // */5
            if (hour%5 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_5");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_6:
            // */6
            if (hour%6 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_6");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_8:
            // */8
            if (hour%8 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_8");
#endif
                return false;
            }
            break;
        case RELAYTASK_TIME_HOUR_12:
            // */12
            if (hour%12 != 0) {
#ifdef RelayTask_debug_checkTime
                Serial.println(",!H_12");
#endif
                return false;
            }
            break;
        default:
            if (((_data & RELAYTASK_DATA_TIME_HOUR)>>6) != hour) {
#ifdef RelayTask_debug_checkTime
                Serial.print(",_h!=");
                Serial.println(((_data & RELAYTASK_DATA_TIME_HOUR)>>6));
#endif
                return false;
            }
            break;
    }
    // DAY_OF_WEEK
    switch ((_data & RELAYTASK_DATA_TIME_DAY_OF_WEEK)>>11) {
        case RELAYTASK_TIME_WEAKDAY_ALL:
            // Default not false
            break;
        default:
            if (((_data & RELAYTASK_DATA_TIME_DAY_OF_WEEK)>>11) != day_of_week) {
#ifdef RelayTask_debug_checkTime
                Serial.print(",_dow!=");
                Serial.println(((_data & RELAYTASK_DATA_TIME_DAY_OF_WEEK)>>11));
#endif
                return false;
            }
            break;
    }
    // DAY_OF_MONTH
    switch ((_data & RELAYTASK_DATA_TIME_DAY_OF_MONTH)>>15) {
        case RELAYTASK_TIME_MONTHDAY_ALL:
            // Default not false
            break;
        default:
            if (((_data & RELAYTASK_DATA_TIME_DAY_OF_MONTH)>>15) != day_of_month) {
#ifdef RelayTask_debug_checkTime
                Serial.print(",_dom!=");
                Serial.println(((_data & RELAYTASK_DATA_TIME_DAY_OF_MONTH)>>15));
#endif
                return false;
            }
            break;
    }
    // MONTH
    switch ((_data & RELAYTASK_DATA_TIME_MONTH)>>21) {
        case RELAYTASK_TIME_MONTH_ALL:
            // Default not false
            break;
        default:
            if (((_data & RELAYTASK_DATA_TIME_MONTH)>>21) != month) {
#ifdef RelayTask_debug_checkTime
                Serial.print(",_mo!=");
                Serial.println(((_data & RELAYTASK_DATA_TIME_MONTH)>>21));
#endif
                return false;
            }
            break;
    }
#ifdef RelayTask_debug_checkTime
    Serial.println();
#endif
    return true;
}

#ifdef RelayTask_debug

void RelayTask::print()
{
    Serial.print("RT::p ");
    if (!bitRead(_status,RELAYTASK_STATUS_SETUP_BIT)) {
        Serial.print("setup not runned.");
        return;
    }
    Serial.print(" r=");
    if (_relay == RELAYTASK_RELAY_ALL) {
        Serial.print("A");
    } else if (_relay == RELAYTASK_RELAY_NONE) {
        Serial.print("N");
    } else {
        Serial.print("(");
        Serial.print(_relay,BIN);
        Serial.print(")");
    }
    Serial.print(", ");
    Serial.print((bitRead(_status,RELAYTASK_STATUS_ON_BIT)) ? "ON" : "OFF");
    Serial.print(", TA=");
    switch (getTask()) {
        case RELAYTASK_TASK_TIME:
            Serial.print("TI ");
            printTimeData();
            break;
        case RELAYTASK_TASK_TEMPERATURE:
            Serial.print("TE ");
            printOperatorData();
            break;
        case RELAYTASK_TASK_HUMIDITY:
            Serial.print("HU ");
            printOperatorData();
            break;
        case RELAYTASK_TASK_LIGHT:
            Serial.print("LI ");
            printOperatorData();
            break;
        case RELAYTASK_TASK_EXTRA:
            Serial.print("EX ");
            break;
        case RELAYTASK_TASK_TIMER:
            Serial.print("TR ");
            break;
        case RELAYTASK_TASK_TRIGGER:
            Serial.print("TI ");
            break;
        default:
            Serial.print("N ");
            break;
    }
    Serial.print(", _s=");
    Serial.print(_status,BIN);
    Serial.print(", _d=");
    Serial.print((uint16_t)((_data & RELAYTASK_DATA_TIMEOUT_MASK) >> 16),BIN);
    Serial.print(" ");
    Serial.print((uint16_t)((_data & RELAYTASK_DATA_OPERATOR_MASK) >> 0),BIN);
}

void RelayTask::println()
{
    print();
    Serial.println();
}

void RelayTask::printTimeData() {
    uint8_t delay           = (_data & RELAYTASK_DATA_TIME_DELAY) >> 25;
    uint8_t month           = (_data & RELAYTASK_DATA_TIME_MONTH) >> 21;
    uint8_t day_of_month    = (_data & RELAYTASK_DATA_TIME_DAY_OF_MONTH) >> 15;
    uint8_t day_of_week     = (_data & RELAYTASK_DATA_TIME_DAY_OF_WEEK) >> 11;
    uint8_t hour            = (_data & RELAYTASK_DATA_TIME_HOUR) >> 6;
    uint8_t minute          = (_data & RELAYTASK_DATA_TIME_MINUTE);

    Serial.print("d=");
    Serial.print(delay);
    Serial.print(", mo=");
    Serial.print(month);
    Serial.print(", dom=");
    Serial.print(day_of_month);
    Serial.print(", dow=");
    Serial.print(day_of_week);
    Serial.print(", h=");
    Serial.print(hour);
    Serial.print(", mi=");
    Serial.print(minute);
}

void RelayTask::printOperatorData() {
    uint8_t operatorType    = (_data & RELAYTASK_DATA_OPERATOR_OPERATOR);
    bool    positive        = (_data & RELAYTASK_DATA_OPERATOR_SIGNED) >> 3;
    uint8_t valueType       = (_data & RELAYTASK_DATA_OPERATOR_VALUETYPE) >> 4;
    uint8_t value           = (_data & RELAYTASK_DATA_OPERATOR_VALUE) >> 6;
    uint8_t mode            = (_data & RELAYTASK_DATA_OPERATOR_MODE) >> 14;
   
    int operatorValue = 0;
    Serial.print("Operation: valueType=");
    switch (valueType) {
        case RELAYTASK_DATA_VALUETYPE_A:
            operatorValue = value * RELAYTASK_DATA_PRECISION_A;
            Serial.print("A,");
            break;
        case RELAYTASK_DATA_VALUETYPE_B:
            operatorValue = value * RELAYTASK_DATA_PRECISION_B;
            Serial.print("B,");
            break;
        case RELAYTASK_DATA_VALUETYPE_C:
            operatorValue = value * RELAYTASK_DATA_PRECISION_C;
            Serial.print("C,");
            break;
        case RELAYTASK_DATA_VALUETYPE_D:
            operatorValue = value * RELAYTASK_DATA_PRECISION_D;
            Serial.print("D,");
            break;
        default:
            operatorValue = value;
            Serial.print("NONE,");
            break;
    }
    switch (operatorType) {
        case RELAYTASK_OPERATOR_EQUAL_TO:
            Serial.print("x=");
            break;
        case RELAYTASK_OPERATOR_GREATER_THAN:
            Serial.print("x>");
            break;
        case RELAYTASK_OPERATOR_LESS_THAN:
            Serial.print("x<");
            break;
        default:
            Serial.print("NONE,");
            Serial.print(operatorType);
            break;
    }
    Serial.print((positive) ? "+" : "-");
    Serial.print(operatorValue);
    if (isTimeoutSet()) {
        Serial.print(" ");
        printTimeoutData();
    }
}

void RelayTask::printTimeoutData() {
    bool        timeout     = (_data & RELAYTASK_DATA_OPERATOR_TIMEOUT) >> 16;
    uint8_t     delayType   = (_data & RELAYTASK_DATA_OPERATOR_DELAYTYPE) >> 17;
    uint16_t    delay       = (_data & RELAYTASK_DATA_OPERATOR_DELAY) >> 20;
    Serial.print("t=");
    Serial.print(timeout);
    Serial.print(", dT=");
    Serial.print(delayType);
    Serial.print(", d=");
    Serial.print(delay);
}

#endif



