//
//  RelayStatus 
//  Header
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
// 


#ifndef RelayStatus_h

#define RelayStatus_h

#include <Arduino.h>
//#include <Relays.h>
#include <RelayTask.h>

#define RelayStatus_power
//#define Relays_save
#define Relays_at24

#ifdef Relays_at24
#include <AT24.h>
#endif Relays_at24

//#define RelayStatus_power_turning


//#define RelayStatus_debug
//#define RelayStatus_debug_power
//#define RelayStatus_debug_onoff

#define _POWER_SAMPLE_SIZE 20  // 50Htz => 1 Htz 20ms
#define _DEFAULT_POWER_VALUE 512 
#define _POWER_TRIGGER_VALUE 2.0 // +/- 80 watt correction a 20 Amp

#define RELAYSTATUS_POWER_AMPS_5 9.765 // 2,25 watt at 230 volt
#define RELAYSTATUS_POWER_AMPS_20 39.0625 // 9 watt at 230 volt
#define RELAYSTATUS_POWER_AMPS_30 58.5938 // 14 watt at 230 volt
#define RELAYSTATUS_POWER_VOLTS 230

#define RELAYSTATUS_POWER_TYPE_0  0x0000
#define RELAYSTATUS_POWER_TYPE_5  0x0100
#define RELAYSTATUS_POWER_TYPE_20 0x0200
#define RELAYSTATUS_POWER_TYPE_30 0x0300

#define _RELAYSTATUS_ON                 LOW
#define _RELAYSTATUS_OFF                HIGH

#define _RELAYSTATUS_POWER_CYCLES       8

#define RELAYSTATUS_STATUS_SETUP        0x0001 // 1 bit  ........ .......1
#define RELAYSTATUS_STATUS_ON           0x0002 // 1 bit  ..............1.
#define RELAYSTATUS_STATUS_DEFAULT_ ON  0x0004 // 1 bit  ........ .....1..
#define RELAYSTATUS_STATUS_POWER_ON     0x0008 // 1 bit  ........ ....1...
#define RELAYSTATUS_STATUS_POWER_TYPE   0x0030 // 2 bit  ........ ..11....
#define RELAYSTATUS_STATUS_LOCKED       0x0040 // 1 bit  ........ .1......
#define RELAYSTATUS_STATUS_DISABLED     0x0080 // 1 bit  ........ 1.......
#define RELAYSTATUS_STATUS_NOSENSORS    0x0100 // 1 bit  .......1 ........


#define RELAYSTATUS_STATUS_SETUP_BIT        0
#define RELAYSTATUS_STATUS_ON_BIT           1
#define RELAYSTATUS_STATUS_DEFAULT_ON_BIT   2
#define RELAYSTATUS_STATUS_POWER_BIT        3
#define RELAYSTATUS_STATUS_A5_BIT           4
#define RELAYSTATUS_STATUS_A20_BIT          5
#define RELAYSTATUS_STATUS_LOCKED_BIT       6
#define RELAYSTATUS_STATUS_DISABLED_BIT     7
#define RELAYSTATUS_STATUS_NOSENSORS_BIT    8


#define RELAYSTATUS_MODE_NONE           0x00    // ........
#define RELAYSTATUS_MODE_MANUAL         0x01    // .......1
#define RELAYSTATUS_MODE_TIME           0x02    // ......1.
#define RELAYSTATUS_MODE_TEMPERATURE    0x04    // .....1..
#define RELAYSTATUS_MODE_HUMIDITY       0x08    // ....1...
#define RELAYSTATUS_MODE_LIGHT          0x10    // ...1....
#define RELAYSTATUS_MODE_EXTRA          0x20    // ..1.....
#define RELAYSTATUS_MODE_TIMER          0x40    // .1......
#define RELAYSTATUS_MODE_TRIGGER        0x80    // 1.......
#define RELAYSTATUS_MODE_SENSORS        0x3C    // ..1111..
#define RELAYSTATUS_MODE_ALL            0xFF    // 11111111
#define RELAYSTATUS_MODE_DEFAULT        RELAYSTATUS_MODE_ALL ^ ( RELAYSTATUS_MODE_MANUAL |RELAYSTATUS_MODE_EXTRA )
#define RELAYSTATUS_MODE_RESTORE        RELAYSTATUS_MODE_ALL ^ ( RELAYSTATUS_MODE_EXTRA )

#define RELAYSTATUS_TIMER_BIT           0

#define RELAYSTATUS_TIMER_NONE          0x0000  // 0 bit   ........ ........
#define RELAYSTATUS_TIMER_TIMEOUT       0x0001  // 1 bit   ........ .......1
#define RELAYSTATUS_TIMER_DELAYTYPE     0x000E  // 3 bits  ........ ....111.
#define RELAYSTATUS_TIMER_DELAY         0xFFF0  // C bits  11111111 1111....


#define RELAYSTATUS_DELAYTYPE_NONE      RELAYTASK_DATA_DELAYTYPE_NONE
#define RELAYSTATUS_DELAYTYPE_SECONDS   RELAYTASK_DATA_DELAYTYPE_SECONDS
#define RELAYSTATUS_DELAYTYPE_MINUTES   RELAYTASK_DATA_DELAYTYPE_MINUTES
#define RELAYSTATUS_DELAYTYPE_HOURS     RELAYTASK_DATA_DELAYTYPE_HOURS
#define RELAYSTATUS_DELAYTYPE_DAYS      RELAYTASK_DATA_DELAYTYPE_DAYS


class RelayStatus
{
public:
    void setup(uint8_t relayPin);
    
    void setup(uint8_t relayPin, uint16_t defaultMode);

    void setup(uint8_t relayPin, bool defaultOn);
    
    void setup(uint8_t relayPin, int powerPin);
    
    void setup(uint8_t relayPin, int powerPin, bool defaultOn);

    void setup(uint8_t relayPin, int powerPin, uint16_t defaultMode);

    void setup(uint8_t relayPin, int powerPin, uint16_t defaultMode, bool defaultOn);
    
    void loop();

#ifdef Relays_at24
    void restore(at24Element_t *relay);
    
    void backup(at24Element_t *relay);
#endif
    
    uint8_t getMode();
    
    uint8_t getDefaultMode();
    
    uint8_t relayPin();
    
    uint8_t powerPin();
    
    void setPowerPin(uint8_t powerPin);
    
    void relayOn();

    void relayOn(uint16_t mode);

    void relayOff();

    void relayOff(uint16_t mode);
    
    void relayLock(bool locked);
    
    bool isSetup();
    
    bool isOn();
    
    bool isDefaultOn();
    
    bool isTimer();

    bool isLocked();

    bool isDisabled();
    
    bool isSensorsOn();

    bool isPower();

    void setTimer(uint8_t delayType, uint16_t delay);
    
#ifdef RelayStatus_power
    uint16_t getPower();
    
    void setPowerOffset(int8_t offset);
    
#endif
    
    void setSensorsOn(bool on);
    
    uint8_t getTimerType();
    
    uint16_t getTimerDelay();
    
    uint8_t putXBeeStatus(ByteBuffer *buffer);

    uint8_t putXBeePower(ByteBuffer *buffer);

#ifdef RelayStatus_debug
    
    void print();
    
    void println();

#endif
    
private:
    uint16_t    _status         = 0;
    uint8_t     _relayPin       = 0;
    uint8_t     _powerPin       = 0;
    uint8_t     _mode           = RELAYSTATUS_MODE_DEFAULT;
    uint8_t     _defaultMode    = RELAYSTATUS_MODE_DEFAULT;
    uint16_t    _timer          = RELAYSTATUS_TIMER_NONE;
#ifdef RelayStatus_power
    uint16_t    _power          = 0;
    int8_t      _powerOffset    = 0;
#endif
    
    void setOn(bool on);

#ifdef RelayStatus_power
    uint16_t getPowerX(uint8_t count);
    uint16_t _getRawPower();
#endif
};

#endif
