//
//  RelayTask 
//  Library header
//  ----------------------------------
//  Developed with embedXcode
//
//  
//  Created by jeroenjonkman on 13-06-15
//


#ifndef RelayTask_h
#define RelayTask_h

//#define RelayTask_debug
//#define RelayTask_debug_time
//#define RelayTask_debug_setTime
//#define RelayTask_debug_checkTime

#if ARDUINO >= 100
    #include <Arduino.h>
#else
    #include <WProgram.h>
#endif

#include <time.h>
#include <ByteBuffer.h>


#define RELAYTASK_RELAY_NONE  0x0000
#define RELAYTASK_RELAY_01    0x0001
#define RELAYTASK_RELAY_02    0x0002
#define RELAYTASK_RELAY_03    0x0004
#define RELAYTASK_RELAY_04    0x0008
#define RELAYTASK_RELAY_05    0x0010
#define RELAYTASK_RELAY_06    0x0020
#define RELAYTASK_RELAY_07    0x0040
#define RELAYTASK_RELAY_08    0x0080
#define RELAYTASK_RELAY_09    0x0100
#define RELAYTASK_RELAY_10    0x0200
#define RELAYTASK_RELAY_11    0x0400
#define RELAYTASK_RELAY_12    0x0800
#define RELAYTASK_RELAY_13    0x1000
#define RELAYTASK_RELAY_14    0x2000
#define RELAYTASK_RELAY_15    0x4000
#define RELAYTASK_RELAY_16    0x8000
#define RELAYTASK_RELAY_ALL   0xFFFF

#define RELAYTASK_STATUS_SETUP     0x01 // 1 bit  .......1
#define RELAYTASK_STATUS_ON        0x02 // 1 bit  ......1.
#define RELAYTASK_STATUS_TASK      0xF0 // 4 bits 1111....


#define RELAYTASK_STATUS_SETUP_BIT 0
#define RELAYTASK_STATUS_ON_BIT    1

#define RELAYTASK_TASK_NONE        0
#define RELAYTASK_TASK_TIME        1
#define RELAYTASK_TASK_TEMPERATURE 2
#define RELAYTASK_TASK_HUMIDITY    3
#define RELAYTASK_TASK_LIGHT       4
#define RELAYTASK_TASK_EXTRA       5
#define RELAYTASK_TASK_TIMER       6
#define RELAYTASK_TASK_TRIGGER     7

#define RELAYTASK_OPERATOR_NONE             0
#define RELAYTASK_OPERATOR_EQUAL_TO         1
#define RELAYTASK_OPERATOR_GREATER_THAN     2
#define RELAYTASK_OPERATOR_LESS_THAN        3

#define RELAYTASK_TIME_MINUTE_ALL           60
#define RELAYTASK_TIME_MINUTE_5             61
#define RELAYTASK_TIME_MINUTE_15            62
#define RELAYTASK_TIME_MINUTE_30            63
#define RELAYTASK_TIME_HOUR_ALL             24
#define RELAYTASK_TIME_HOUR_2               25
#define RELAYTASK_TIME_HOUR_3               26
#define RELAYTASK_TIME_HOUR_4               27
#define RELAYTASK_TIME_HOUR_5               28
#define RELAYTASK_TIME_HOUR_6               29
#define RELAYTASK_TIME_HOUR_8               30
#define RELAYTASK_TIME_HOUR_12              31
#define RELAYTASK_TIME_WEAKDAY_ALL          0
#define RELAYTASK_TIME_MONTHDAY_ALL         0
#define RELAYTASK_TIME_MONTH_ALL            0

#define RELAYTASK_DATA_VALUETYPE_A          0   // -256 to +256
#define RELAYTASK_DATA_VALUETYPE_B          1   // -1024 to +1024 interval 4
#define RELAYTASK_DATA_VALUETYPE_C          2   // -4096 to +4096 interval 16
#define RELAYTASK_DATA_VALUETYPE_D          3   // -16384 to +16384 interval 64

#define RELAYTASK_DATA_PRECISION_A          1
#define RELAYTASK_DATA_PRECISION_B          4
#define RELAYTASK_DATA_PRECISION_C          16
#define RELAYTASK_DATA_PRECISION_D          64

#define RELAYTASK_DATA_DELAYTYPE_NONE       0
#define RELAYTASK_DATA_DELAYTYPE_SECONDS    1
#define RELAYTASK_DATA_DELAYTYPE_MINUTES    2
#define RELAYTASK_DATA_DELAYTYPE_HOURS      3
#define RELAYTASK_DATA_DELAYTYPE_DAYS       4

#define RELAYTASK_SIGNED_POSITIVE           TRUE
#define RELAYTASK_SIGNED_NEGETIVE           FALSE

#define RELAYTASK_DATA_TIME_MASK            0xFFFFFFFF  //         11111111 11111111 11111111 11111111
#define RELAYTASK_DATA_OPERATOR_MASK        0x0000FFFF  //         ........ ........ 11111111 11111111
#define RELAYTASK_DATA_TIMEOUT_MASK         0xFFFF0000  //         11111111 11111111 ........ ........

#define RELAYTASK_DATA_TIME_MINUTE          0x0000003F  // 6 bits  ........ ........ ........ ..111111
#define RELAYTASK_DATA_TIME_HOUR            0x000007C0  // 5 bits  ........ ........ .....111 11......
#define RELAYTASK_DATA_TIME_DAY_OF_WEEK     0x00007800  // 4 bits  ........ ........ .1111... ........
#define RELAYTASK_DATA_TIME_DAY_OF_MONTH    0x001F8000  // 6 bits  ........ ...11111 1....... ........
#define RELAYTASK_DATA_TIME_MONTH           0x01E00000  // 4 bits  .......1 111..... ........ ........
#define RELAYTASK_DATA_TIME_DELAY           0xFE000000  // 7 bits  1111111. ........ ........ ........

#define RELAYTASK_DATA_OPERATOR_OPERATOR    0x00000007  // 3 bits  ........ ........ ........ .....111
#define RELAYTASK_DATA_OPERATOR_SIGNED      0x00000008  // 1 bit   ........ ........ ........ ....1...
#define RELAYTASK_DATA_OPERATOR_VALUETYPE   0x00000030  // 2 bits  ........ ........ ........ ..11....
#define RELAYTASK_DATA_OPERATOR_VALUE       0x00003FC0  // 8 bits  ........ ........ ..111111 11......
#define RELAYTASK_DATA_OPERATOR_MODE        0x0000C000  // 2 bits  ........ ........ 11...... ........
#define RELAYTASK_DATA_OPERATOR_TIMEOUT     0x00010000  // 1 bit   ........ .......1 ........ ........
#define RELAYTASK_DATA_OPERATOR_DELAYTYPE   0x000E0000  // 3 bits  ........ ....111. ........ ........
#define RELAYTASK_DATA_OPERATOR_DELAY       0xFFF00000  // C bits  11111111 1111.... ........ ........

class RelayTask
{
public:
    void setup();

    void setTime(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute);
    
    void setTemperature(uint8_t operatortype, int value);
    
    void setHumidity(uint8_t operatortype, int value);

    void setLight(uint8_t operatortype, int value);

    void setTrigger(uint8_t delayType, uint16_t delay);

    void setRelay(uint16_t relay);
    
    bool setTimeout(uint8_t delayType, uint16_t delay);

    void setOn(bool on);

    bool isOn();

    bool isSetup();
    
    bool isTimeoutTask();

    bool isTimeoutSet();
    
    uint8_t getTimerType();
    
    uint16_t getTimerDelay();

    uint8_t getTask();
    
    uint16_t getRelay();
    
    /* checkOperatorOnValue
     return true if value matched on operator. Not the relay on/off value!
     */
    bool checkOperatorOnValue(int value);
    
    bool checkTime(time_t time);
    
    bool checkTime(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute);

#ifdef RelayTask_debug
    
    void print();
    
    void println();

#endif
    
private:
    uint8_t     _status     = 0x00;         // RELAYTASK_STATUS_*
    uint16_t    _relay      = 0x0000;       // RELAYTASK_RELAY_*
    uint32_t    _data       = 0x00000000;

    uint32_t timeToData(uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute);
    
    uint32_t operatorToData(uint8_t operatortype, int value);
    
    uint32_t timeoutToData(uint8_t delayType, uint16_t delay);
    
    void setTask(uint8_t task);
    
    uint16_t yearday(time_t time);
    
#ifdef RelayTask_debug

    void printTimeData();
    
    void printOperatorData();

    void printTimeoutData();
    
#endif
};

#endif
