//
//  Relays 
//  Header
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
//  Modified by jeroenjonkman on 31-07-19
// 


#ifndef Relays_h

#define Relays_h

#define Relays_power

#define Relays_trigger
//#define Relays_Timer
#define Relays_reset

//#define Relays_debug
//#define Relays_set_debug
//#define Relays_time_debug
//#define Relays_print


//#define _RELAYS_digitalReadOutputPin

#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#ifdef RelayTask_debug
#define _RELAYS_MAX_TASK_SIZE       8
#else
#define _RELAYS_MAX_TASK_SIZE       24
#endif

#ifdef RelayStatus_debug
#define _RELAYS_MAX_STATUS_SIZE     4
#else
#define _RELAYS_MAX_STATUS_SIZE     12
#endif

#include <RelayStatus.h>
#include <RelayTask.h>
#ifdef Relays_at24
#include <AT24.h>
#define RELAYS_AT24_RETRY           5
#endif Relays_at24
#include <JRTC.h>
#include <Time.h>
#include <ByteBuffer.h>

#ifdef Relay_debug
#define RELAYS_LOOP_CHECK           2500
#else
#define RELAYS_LOOP_CHECK           500
#endif

#define RELAYS_OFF                  false
#define RELAYS_ON                   true

#define _RELAYS_POWER_MIN           5
#define _RELAYS_MAX                 150
#define _RELAYS_READ_COUNT          100

#define _RELAYS_TEMPERATURE_NOSET   1024
#define _RELAYS_HUMIDITY_NOSET      1024
#define _RELAYS_LIGHT_NOSET         -1

#define XBEE_TIME_HEADER            0x10
#define XBEE_RELAY_HEADER           0x20
#define XBEE_POWER_HEADER           0x80

#define _RELAYS_SAVE_DEFAULT        0xFFFF

class Relays
{
public:
    void setup();
    
    void loop();

    void loopTask();
    
    void loopStatus();

#ifdef Relays_power
    void loopPower();
#endif
    
    int addRelay( uint8_t relayPin, int16_t powerPin, bool defaultOn, uint16_t defaultMode);
    int addRelay( uint8_t relayPin, int16_t powerPin, bool defaultOn, uint16_t defaultMode, bool setOn);
#ifdef Relays_Basis_TaskTime
    int addTaskTimeLock( uint16_t relays, bool on, uint8_t hour, uint8_t minute);
    int addTaskTimeUnlock( uint16_t relays, bool on, uint8_t hour, uint8_t minute);
    int addTaskTimeSensors( uint16_t relays, bool on, uint8_t hour, uint8_t minute);
    int addTaskTime( uint16_t relays, bool on, uint8_t hour, uint8_t minute);
#else
    int addTaskTime( uint16_t relays, bool on, uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute);
#endif Relays_Basis_TaskTime
    int addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value);

    int addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode);
    
#ifdef RelayTask_Humidity
    int addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value);

    int addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode);
#endif RelayTask_Humidity

    int addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value);

    int addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode);

#ifdef Relays_trigger
    int addTaskTrigger( uint16_t relays, bool on, uint8_t delayType, uint16_t delay);
#endif Relays_trigger
#ifdef Relays_Timer
    bool setTaskDelayInSeconds(uint16_t taskID, uint16_t delay);

    bool setTaskDelayInMinutes(uint16_t taskID, uint16_t delay);

    bool setTaskDelayInHours(uint16_t taskID, uint16_t delay);
    
    bool setTaskDelayInDays(uint16_t taskID, uint16_t delay);
#endif

    uint8_t getPowerPin( uint8_t relayPin );
    
    uint16_t powerRelay( uint8_t relayPin );
    
    uint16_t power();
    
    void relaysOn();
    
    void relayOn(uint8_t relayPin);
    
    void relayOn(uint8_t relayPin, uint8_t mode);
    
    void relayOn(int relayID);

    void relayOn(int relayID, uint8_t mode);
    
    void relaysOff();

    void relayOff(int relayID);

    void relayOff(int relayID, uint8_t mode);
    
    void relayOff(uint8_t relayPin);

    void relayOff(uint8_t relayPin, uint8_t mode);

    void relaysLock(bool locked);
    
    void relayLock(int relayID, bool locked);
    
    void relayLock(uint8_t relayPin, bool locked);
    
    void powerOffset(uint8_t powerPin, int8_t offset);
    
    uint8_t relayPin();
    
    uint8_t powerPin();
    
    void setPowerPin(uint8_t powerPin);
    
    void setMaxRelay(int maxRelay);
    
    bool isSetup();
    
    void setTemperature(int temperature);
    
    int getTemperature();
    
    void setHumidity(int humidity);
    
    int getHumidity();

    void setLight(int light);
    
    int getLight();
    
    void trigger();

    void putXBeeData(ByteBuffer *buffer);
    
    void putXBeeTime(ByteBuffer *buffer);
    
    void putXBeeStatus(ByteBuffer *buffer);
    
    void putXBeePower(ByteBuffer *buffer);
    
    void resetStatus();
    
private:
    RelayStatus     _status[_RELAYS_MAX_STATUS_SIZE];
    RelayTask       _task[_RELAYS_MAX_TASK_SIZE];
    uint8_t         _statusSize     = 0;
    uint16_t        _statusID       = 0; //
    uint8_t         _taskSize       = 0;
    uint16_t        _taskID         = 0;
    uint16_t        _looper         = 1;
    bool            _setup          = false;
    uint8_t         _maxRelay       = _RELAYS_MAX;
    unsigned long   _last_run       = 0;
    int             _temperature    = _RELAYS_TEMPERATURE_NOSET;
    int             _humidity       = _RELAYS_HUMIDITY_NOSET;
    int             _light          = _RELAYS_LIGHT_NOSET;
    time_t          _lastTime       = 0;
#ifdef Relays_save
    jrtcElements_t  _save;
#endif
#ifdef Relays_at24
    AT24            at24;
    at24Elements_t  _save;
#endif Relays_at24
    
#ifdef _RELAYS_digitalReadOutputPin
    int             digitalReadOutputPin(uint8_t pin);
#endif
    
    bool            checkRelay(uint8_t relayPin);
    
    uint8_t         getRelayStatusIndex(uint8_t relayPin);
    uint8_t         getPowerStatusIndex(uint8_t powerPin);
#ifdef Relays_save
    void            saveStatus();
#endif Relays_save
#ifdef Relays_at24
    void            restoreStatusAt24();
    void            saveStatusAt24();
#endif Relays_at24
#ifdef Relays_print
    String          loopStringStatus();
#endif
};

#endif
