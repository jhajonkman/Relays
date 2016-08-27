//
//  Relays 
//  Library C++ code
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
// 


#if ARDUINO >= 100
    #include "Arduino.h"
#else
    #include "WProgram.h"
#endif

#include <Relays.h>

void Relays::setup()
{
    _setup = true;
    _statusSize = 0;
    if( timeStatus() == timeSet ) {
        _lastTime = now();
    }
#ifdef Relays_save
    _save.lasttime = 0;
    if (RTC.getDataStatus()) {
        RTC.getData(&_save);
        RTC.setDataStatus(false);
    }
#endif Relays_save
#ifdef Relays_at24
    at24 = AT24();
    _save.lasttime = 0;
    if (at24.getDataStatus()) {
        at24.getData(&_save);
        at24.setDataStatus(false);
    }
#endif Relays_at24
    
}

void Relays::loop()
{
    unsigned long m_seconds = millis();
    if (_last_run < m_seconds) {
#ifdef Relays_debug
        Serial.println("Relays::loop next run.");
#endif
        if (_looper > 5) {
            loopTask();
            loopStatus();
#ifdef Relays_power
            loopPower();
#endif
        }
        if (_looper%15==0 && _setup) {
#ifdef Relays_print
            Serial.print(loopStringStatus());
#endif
#ifdef Relays_save
            saveStatus();
#endif
#ifdef Relays_at24
            saveStatusAt24();
#endif
        }
        _last_run += RELAYS_LOOP_CHECK;
        _looper ++;
    }
}

void Relays::loopTask()
{
    for (uint8_t i = 0; i < _taskSize; i++) {
        if (_task[i].isSetup()) {
            uint8_t task = _task[i].getTask();
            uint16_t relay = _task[i].getRelay();
#ifdef RelayTask_Sensors
            if( task == RELAYTASK_TASK_SENSORS) {
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    if (_task[i].checkTime(now())) {
                        if (_task[i].isOn()) {
                            _status[j].setSensorsOn(true);
                        } else {
                            if (_status[j].isDefaultOn()) {
                                _status[j].relayOn(_status[j].getMode());
                            } else {
                                _status[j].relayOff(_status[j].getMode());
                            }
                            _status[j].setSensorsOn(false);
                        }
                    }
                }
            }
#endif RelayTask_Sensors
            if( _temperature != _RELAYS_TEMPERATURE_NOSET && task == RELAYTASK_TASK_TEMPERATURE  ) {
                // Check temperature
#ifdef RelayTask_debug
                Serial.print("Relays::loopTask Temperature=");
                Serial.println(_temperature);
#endif
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    // Check defaultMode of the RelayStatus
                    uint8_t defaultMode = _status[j].getDefaultMode();
#ifdef Relays_debug
                    Serial.print("Relays::loopTask Temperature defaultMode=");
                    Serial.print(defaultMode,BIN);
                    Serial.print(", RELAYSTATUS_MODE_TEMPERATURE=");
                    Serial.println(RELAYSTATUS_MODE_TEMPERATURE,BIN);
#endif
                    if ( (defaultMode & RELAYSTATUS_MODE_TEMPERATURE) == RELAYSTATUS_MODE_TEMPERATURE ) {
                        // Relay and Task match
                        bool check = _task[i].checkOperatorOnValue(_temperature);
#ifdef RelayTask_debug
                        Serial.print("Relays::loopTask Temperature, relay=");
                        Serial.print(_status[j].relayPin());
                        if (!check) {
                            Serial.println(" checked=false");
                        }
#endif
                        if (check) {
                            if (_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayON");
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_TEMPERATURE);
                            }
                            if (!_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayOFF");
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_TEMPERATURE);
                                
                            }
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
#ifdef RelayTask_Humidity
            if (_humidity != _RELAYS_HUMIDITY_NOSET && task == RELAYTASK_TASK_HUMIDITY) {
                // Check humidity
#ifdef RelayTask_debug
                Serial.print("Relays::loopTask Humidity=");
                Serial.println(_humidity);
#endif
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check defaultMode of the RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    // Check relay task with RelayStatus
                    uint8_t defaultMode = _status[j].getDefaultMode();
#ifdef Relays_debug
                    Serial.print("Relays::loopTask Humidity defaultMode=");
                    Serial.print(defaultMode,BIN);
                    Serial.print(", RELAYSTATUS_MODE_HUMIDITY=");
                    Serial.println(RELAYSTATUS_MODE_HUMIDITY,BIN);
#endif
                    if ( ( defaultMode & RELAYSTATUS_MODE_HUMIDITY ) == RELAYSTATUS_MODE_HUMIDITY ) {
                        // Relay and Task match
                        bool check = _task[i].checkOperatorOnValue(_humidity);
#ifdef RelayTask_debug
                        Serial.print("Relays::loopTask Humidity, relay=");
                        if (!check) {
                            Serial.println(" checked=false");
                        }
                        Serial.print(_status[j].relayPin());
#endif
                        if (check) {
                            if (_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayON");
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_HUMIDITY);
                            }
                            if (!_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayOFF");
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_HUMIDITY);
                            }
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
#endif RelayTask_Humidity
            if (_light != _RELAYS_LIGHT_NOSET && task == RELAYTASK_TASK_LIGHT) {
                // Check light
#ifdef RelayStatus_debug
                Serial.print("Relays::loopTask Light=");
                Serial.println(_light);
#endif
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    // Check defaultMode of the RelayStatus
                    uint8_t defaultMode = _status[j].getDefaultMode();
#ifdef Relays_debug
                    Serial.print("Relays::loopTask Light defaultMode=");
                    Serial.print(defaultMode,BIN);
                    Serial.print(", RELAYSTATUS_MODE_LIGHT=");
                    Serial.println(RELAYSTATUS_MODE_LIGHT,BIN);
#endif
                    if ( ( defaultMode & RELAYSTATUS_MODE_LIGHT ) == RELAYSTATUS_MODE_LIGHT ) {
                        // Relay and Task match
                        bool check = _task[i].checkOperatorOnValue(_light);
#ifdef RelayTask_debug
                        Serial.print("Relays::loopTask Light relay=");
                        Serial.print(_status[j].relayPin());
                        if (!check) {
                            Serial.println(" checked=false");
                        }
#endif
                        if (check) {
                            if (_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayON");
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_LIGHT);
                            }
                            if (!_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayOFF");
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_LIGHT);
                            }
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
            if( task == RELAYTASK_TASK_TIME  ) {
                // Check time task
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    // Check defaultMode of the RelayStatus
                    uint8_t defaultMode = _status[j].getDefaultMode();
#ifdef Relays_time_debug
                    Serial.print("Relays::loopTask Time defaultMode=");
                    Serial.print(defaultMode,BIN);
                    Serial.print(", RELAYSTATUS_MODE_TIME=");
                    Serial.println(RELAYSTATUS_MODE_TIME,BIN);
#endif
                    if ( (defaultMode & RELAYSTATUS_MODE_TIME) == RELAYSTATUS_MODE_TIME) {
                        //defaultMode = _status[j].getDefaultMode();
                        bool check = _task[i].checkTime(now());
#ifdef RelayTask_debug
                        Serial.print("Relays::loopTask Time, relay=");
                        Serial.print(_status[j].relayPin());
                        if (!check) {
                            Serial.println(" checked=false");
                        }
#endif
                        if (check) {
                            if (_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayON");
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_TIME);
                            }
                            if (!_task[i].isOn()) {
#ifdef RelayTask_debug
                                Serial.println(" relayOFF");
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_TIME);
                            }
                        }
                    }
                }
            }
#ifdef RelayTask_Locker
            if( task == RELAYTASK_TASK_LOCKER || task == RELAYTASK_TASK_UNLOCKER ) {
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    bool check = _task[i].checkTime(now());
                    if (check) {
                        if (_task[i].isOn()) {
                            _status[j].relayOn(RELAYSTATUS_MODE_DEFAULT);
                        } else {
                            _status[j].relayOff(RELAYSTATUS_MODE_DEFAULT);
                        }
                        _status[j].relayLock(task == RELAYTASK_TASK_LOCKER);
                    }
                }
            }
#endif RelayTask_Locker
        }
    }
    
#ifdef RelayTask_debug
    Serial.println();
#endif
}

void Relays::loopStatus()
{
    String text = "";
    time_t nowTime = now();
    for (uint8_t j = 0 ; j < _statusSize; j++) {
        // Check relay task with RelayStatus
        if (_status[j].isTimer()) {
#ifdef Relays_time_debug
            text += "Relays::loopStatus relayPin=";
            text += _status[j].relayPin();
            text += ", timerType=";
            text += _status[j].getTimerType();
            text += ", timerDelay=";
            text += _status[j].getTimerDelay();
#endif
            uint8_t nowMinute = minute(nowTime);
            uint8_t lastMinute = minute(_lastTime);
            uint8_t nowSeconds = second(nowTime);
            uint8_t lastSeconds = second(_lastTime);
            uint16_t delay = _status[j].getTimerDelay();
            switch ((int)_status[j].getTimerType()) {
                case RELAYSTATUS_DELAYTYPE_DAYS:
                    break;
                case RELAYSTATUS_DELAYTYPE_HOURS:
                    break;
                case RELAYSTATUS_DELAYTYPE_MINUTES:
#ifdef Relays_time_debug
                    text += ", nowMinute=";
                    text += nowMinute;
                    text += ", lastMinute=";
                    text += lastMinute;
#endif
                    if (nowMinute != lastMinute) {
                        // One minute passed.
                        if (nowMinute < lastMinute)
                            lastMinute += 60;
                        uint8_t deltaMinute = nowMinute - lastMinute;
#ifdef Relays_time_debug
                        text += ", deltaMinute=";
                        text += deltaMinute;
#endif
                        delay -= deltaMinute;
#ifdef Relays_time_debug
                        text += ", delay=";
                        text += delay;
                        text += "\n";
#endif
                        if (delay <= 5) {
                            // Make seconds timer
                            delay = (delay>0)?delay*60:1;
                            _status[j].setTimer(RELAYSTATUS_DELAYTYPE_SECONDS,delay);
                        } else {
                            _status[j].setTimer(RELAYSTATUS_DELAYTYPE_MINUTES,delay);
                        }
#ifdef Relays_time_debug
                    } else {
                        text += "\n";
#endif
                    }
                    break;
#ifdef Relays_Timer
                case RELAYSTATUS_DELAYTYPE_SECONDS:
#ifdef Relays_time_debug
                    text += ", nowSeconds=";
                    text += nowSeconds;
                    text += ", lastSeconds=";
                    text += lastSeconds;
#endif
                    if (nowSeconds != lastSeconds) {
                        // Seconds passed.
                        if (nowSeconds < lastSeconds)
                            lastSeconds += 60;
                        uint8_t deltaSeconds = nowSeconds - lastSeconds;
#ifdef Relays_time_debug
                        text += ", deltaSeconds=";
                        text += deltaSeconds;
#endif
                        delay = (delay <= deltaSeconds)?0:delay - deltaSeconds;
#ifdef Relays_time_debug
                        text += ", delay=";
                        text += delay;
#endif
                        if (delay <= 1) {
#ifdef Relays_time_debug
                            text += " TIMER PASSED, ";
#endif
                            if (_status[j].isOn()) {
#ifdef Relays_time_debug
                                text += " SET OFF!\n";
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_TIMER);
                            } else {
#ifdef Relays_time_debug
                                text += " SET ON!\n";
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_TIMER);
                            }
                        } else {
#ifdef Relays_time_debug
                            text += "\n";
#endif
                            _status[j].setTimer(RELAYSTATUS_DELAYTYPE_SECONDS,delay);
                        }
                    }
                    break;
#endif Relays_Timer
                default:
                    break;
            }
        }
#ifdef RelayStatus_debug
        if (_status[j].isSetup()) {
            _status[j].println();
        }
        text += "\n";
#endif
    }
    _lastTime = nowTime;
#ifdef RelayStatus_debug
    Serial.print(text);
#endif
    //return text;
}

#ifdef Relays_power
void Relays::loopPower()
{
#ifdef Relays_debug
    Serial.println("Relays::loopPower");
#endif
    for( uint16_t i = 0 ; i < _statusSize ; i++ ) {
        if (_looper%(_statusSize) == i) {
            _status[i].loop();
        }
    }
}
#endif

int Relays::addRelay( uint8_t relayPin, int16_t powerPin, bool defaultOn, uint16_t defaultMode)
{
    if ( _statusSize < _RELAYS_MAX_STATUS_SIZE ) {
#ifdef Relays_debug
            Serial.print("Relays::add: relayPin=");
            Serial.print(relayPin);
            if ( powerPin != 0 ) {
                Serial.print(", powerPin=");
                Serial.print(powerPin);
            }
            Serial.println();
#endif Relays_debug
        RelayStatus status;
        status.setup(relayPin,powerPin,defaultMode,defaultOn);
#ifdef Relays_save
        if( _save.lasttime > 0 && _statusSize < AT24_STATUS_SIZE) {
#ifdef Relays_print
            Serial.print("value=");
            Serial.print(_save.value);
#endif Relays_print
            if (_save.value & (0x1 << _statusSize)) {
#ifdef Relays_print
                Serial.println(",on");
#endif Relays_print
                status.relayOn(RELAYSTATUS_MODE_RESTORE);
            } else {
#ifdef Relays_print
                Serial.println(",off");
#endif Relays_print
                status.relayOff(RELAYSTATUS_MODE_RESTORE);
            }
        }
#endif Relays_save
#ifdef Relays_at24
        if( _save.lasttime > 0 && _statusSize < AT24_STATUS_SIZE) {
            status.restore(&_save.relay[_statusSize]);
        }
#endif Relays_at24
        _status[_statusSize] = status;
    } else {
        Serial.println("R:E01");
        return -1;
    }
    _statusSize++;
    return _statusID++;
}



#ifdef Relays_Basis_TaskTime

int Relays::addTaskTimeLock( uint16_t relays, bool on, uint8_t hour, uint8_t minute)
{
    int task_id = addTaskTime(relays, on, hour, minute);
    _task[task_id].setTask(RELAYTASK_TASK_LOCKER);
    return task_id;
}

int Relays::addTaskTimeUnlock( uint16_t relays, bool on, uint8_t hour, uint8_t minute)
{
    int task_id = addTaskTime(relays, on, hour, minute);
    _task[task_id].setTask(RELAYTASK_TASK_UNLOCKER);
    return task_id;
}

int Relays::addTaskTimeSensors( uint16_t relays, bool on, uint8_t hour, uint8_t minute)
{
    int task_id = addTaskTime(relays, on, hour, minute);
    _task[task_id].setTask(RELAYTASK_TASK_SENSORS);
    return task_id;
}

int Relays::addTaskTime( uint16_t relays, bool on, uint8_t hour, uint8_t minute)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setTime(hour,minute);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}
#else
int Relays::addTaskTime( uint16_t relays, bool on, uint8_t month, uint8_t day_of_month, uint8_t day_of_week, uint8_t hour, uint8_t minute)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setTime(month,day_of_month,day_of_week,hour,minute);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}
#endif Relays_Basis_TaskTime

int Relays::addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    addTaskTemperature(relays,on,operatortype,value,RELAYTASK_OPERATOR_RELAY_DISABLED);
}

int Relays::addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setTemperature(operatortype,value, mode);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}

#ifdef RelayTask_Humidity
int Relays::addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    return addTaskHumidity(relays,on,operatortype,value,RELAYTASK_OPERATOR_RELAY_DISABLED);
}

int Relays::addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setHumidity(operatortype,value, mode);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}
#endif RelayTask_Humidity

int Relays::addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    return addTaskLight(relays,on,operatortype,value,RELAYTASK_OPERATOR_RELAY_DISABLED);
}

int Relays::addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value, uint8_t mode)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setLight(operatortype,value, mode);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}

#ifdef Relays_trigger
int Relays::addTaskTrigger( uint16_t relays, bool on, uint8_t delayType, uint16_t delay)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setTrigger(delayType,delay);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}
#endif Relays_trigger

#ifdef Relays_Timer
bool Relays::setTaskDelayInSeconds(uint16_t taskID, uint16_t delay)
{
    if (_task[taskID].isSetup() && _task[taskID].isTimeoutTask()) {
        return _task[taskID].setTimeout(RELAYTASK_DATA_DELAYTYPE_SECONDS,delay);
    }
    return false;
}

bool Relays::setTaskDelayInMinutes(uint16_t taskID, uint16_t delay)
{
    if (_task[taskID].isSetup() && _task[taskID].isTimeoutTask()) {
        return _task[taskID].setTimeout(RELAYTASK_DATA_DELAYTYPE_MINUTES,delay);
    }
    return false;
}

bool Relays::setTaskDelayInHours(uint16_t taskID, uint16_t delay)
{
    if (_task[taskID].isSetup() && _task[taskID].isTimeoutTask()) {
        return _task[taskID].setTimeout(RELAYTASK_DATA_DELAYTYPE_HOURS,delay);
    }
    return false;
}

bool Relays::setTaskDelayInDays(uint16_t taskID, uint16_t delay)
{
    if (_task[taskID].isSetup() && _task[taskID].isTimeoutTask()) {
        return _task[taskID].setTimeout(RELAYTASK_DATA_DELAYTYPE_DAYS,delay);
    }
    return false;
}
#endif Relays_Timer

uint8_t Relays::getPowerPin( uint8_t relayPin ) {
    uint8_t powerPin = 0;
    for( int i = 0 ; i < _statusSize ; i++ )
        if ( _status[i].relayPin() == relayPin )
            return _status[i].powerPin();
    return powerPin;
}

uint16_t Relays::powerRelay( uint8_t relayPin )
{
    uint8_t powerPin = 0;
    for( int i = 0 ; i < _statusSize ; i++ )
        if ( _status[i].relayPin() == relayPin )
            return _status[i].getPower();
    return 0;
}

uint16_t Relays::power() {
    long totalPower = 0;
    int count = 0;
    for( int i = 0 ; i < _statusSize ; i++ ) {
        int power = powerRelay(_status[i].relayPin());
        if( power > _RELAYS_POWER_MIN ) {
            count++;
            totalPower += power;
        }
    }
#ifdef Relays_debug
        Serial.print("Relay::power totalPower=");
        Serial.print(totalPower);
        Serial.print(", count=");
        Serial.println(count);
#endif
    return max(totalPower / count,0);
}

void Relays::setTemperature(int temperature)
{
#ifdef Relays_set_debug
    Serial.print("Relay::setTemperature _temperature=");
    Serial.print(_temperature);
    Serial.print(", temperature=");
    Serial.println(temperature);
#endif
    _temperature = temperature;
}

int Relays::getTemperature()
{
    return _temperature;
}

void Relays::setHumidity(int humidity)
{
#ifdef Relays_set_debug
    Serial.print("Relay::setHumidity _humidity=");
    Serial.print(_humidity);
    Serial.print(", humidity=");
    Serial.println(humidity);
#endif
    _humidity = humidity;
}

int Relays::getHumidity()
{
    return _humidity;
}

void Relays::setLight(int light)
{
#ifdef Relays_set_debug
    Serial.print("Relay::setLight _light=");
    Serial.print(_light);
    Serial.print(", light=");
    Serial.println(light);
#endif
    _light = light;
}

int Relays::getLight()
{
    return _light;
}

void Relays::trigger()
{
    //Serial.println("Relays::trigger.");
    for (uint8_t i = 0; i < _taskSize; i++) {
        if (_task[i].isSetup()) {
            uint8_t task = _task[i].getTask();
            uint16_t relay = _task[i].getRelay();
            if( task == RELAYTASK_TASK_TRIGGER  ) {
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
#ifdef Relays_time_debug
                    Serial.print("Relays::trigger relayPin=");
                    Serial.print(_status[j].relayPin());
                    Serial.print(", timerType=");
                    Serial.print(_status[j].getTimerType());
                    Serial.print(", timerDelay=");
                    Serial.print(_status[j].getTimerDelay());
#endif
                    if (_task[i].isOn()) {
#ifdef Relays_time_debug
                        Serial.println(" SET ON!");
#endif
                        _status[j].relayOn(RELAYSTATUS_MODE_TRIGGER);
                        if (_task[i].isTimeoutSet()) {
                            _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                        }
                    } else {
#ifdef Relays_time_debug
                        Serial.println(" SET OFF!");
#endif
                        _status[j].relayOff(RELAYSTATUS_MODE_TRIGGER);
                        if (_task[i].isTimeoutSet()) {
                            _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                        }
                    }
                }
            }
        }
    }
}

void Relays::putXBeeData(ByteBuffer *buffer)
{
    putXBeeTime(buffer);
    putXBeeStatus(buffer);
#ifdef Relays_power
    putXBeePower(buffer);
#endif Relays_power
}

void Relays::putXBeeTime(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= 5) {
        buffer->put(XBEE_TIME_HEADER);
        buffer->putTime(_lastTime);
    }
}

void Relays::putXBeeStatus(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= (2 + _statusSize + 1)) {
        buffer->put(XBEE_RELAY_HEADER);
        buffer->put(_statusSize);
        for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
            _status[i].putXBeeStatus(buffer);
        }
    }
}

#ifdef Relays_power
void Relays::putXBeePower(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= (2 + (_statusSize + 1) * 2)) {
        uint8_t count = 0;
        for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
            if(_status[i].isPower() > 0) {
                count++;
            }
        }
        if (count > 0) {
            buffer->put(XBEE_POWER_HEADER);
            buffer->put(count);
            for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
                if(_status[i].isPower()) {
                    _status[i].putXBeePower(buffer);
                }
            }
        }
    }
}
#endif Relays_power

void Relays::relaysOn() {
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        relayOn(_status[i].relayPin() );
}

void Relays::relayOn(uint8_t relayPin) {
    relayOn(relayPin,RELAYSTATUS_MODE_MANUAL);
}

void Relays::relayOn(uint8_t relayPin, uint8_t mode) {
    int index = getRelayStatusIndex(relayPin);
    relayOn(index, mode);
}

void Relays::relayOn(int relayID) {
    relayOn(relayID,RELAYSTATUS_MODE_MANUAL);
}

void Relays::relayOn(int relayID, uint8_t mode) {
    if (relayID <= _RELAYS_MAX_STATUS_SIZE && _status[relayID].isSetup()) {
        _status[relayID].relayOn(mode);
    }
#ifdef Relays_save
    saveStatus();
#endif
#ifdef Relays_at24
    saveStatusAt24();
#endif
}

void Relays::relaysOff() {
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        relayOff(_status[i].relayPin() );
}

void Relays::relayOff(uint8_t relayPin) {
    relayOff(relayPin, RELAYSTATUS_MODE_MANUAL);
}

void Relays::relayOff(uint8_t relayPin, uint8_t mode) {
    int index = getRelayStatusIndex(relayPin);
    relayOff(index, mode);
}

void Relays::relayOff(int relayID) {
    relayOff(relayID,RELAYSTATUS_MODE_MANUAL);
}

void Relays::relayOff(int relayID, uint8_t mode) {
    if (relayID <= _RELAYS_MAX_STATUS_SIZE && _status[relayID].isSetup()) {
        _status[relayID].relayOff(mode);
    }
#ifdef Relays_save
    saveStatus();
#endif
#ifdef Relays_at24
    saveStatusAt24();
#endif
}

void Relays::relaysLock(bool locked) {
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        relayLock(_status[i].relayPin(),locked);
}

void Relays::relayLock(uint8_t relayPin, bool locked) {
    uint8_t index = getRelayStatusIndex(relayPin);
    if (index <= _RELAYS_MAX_STATUS_SIZE && _status[index].isSetup()) {
        _status[index].relayLock(locked);
    }
#ifdef Relays_at24
    saveStatusAt24();
#endif
}

void Relays::relayLock(int relayID, bool locked) {
    if (relayID <= _RELAYS_MAX_STATUS_SIZE && _status[relayID].isSetup()) {
        _status[relayID].relayLock(locked);
    }
#ifdef Relays_at24
    saveStatusAt24();
#endif
}

void Relays::powerOffset(uint8_t powerPin, int8_t offset) {
    uint8_t index = getPowerStatusIndex(powerPin);
    if (index <= _RELAYS_MAX_STATUS_SIZE && _status[index].isSetup()) {
        _status[index].setPowerOffset(offset);
    }
}

#ifdef _RELAYS_digitalReadOutputPin
int Relays::digitalReadOutputPin(uint8_t pin)
{
    uint8_t bit = digitalPinToBitMask(pin);
    uint8_t port = digitalPinToPort(pin);
#ifdef Relays_debug
    Serial.print("digitalReadOutputPin pin=");
    Serial.print(pin);
    Serial.print(", bit=");
    Serial.print(bit);
    Serial.print(", port=");
    Serial.print(port);
#endif
    if (port == NOT_A_PIN) {
#ifdef Relays_debug
            Serial.println("");
#endif
        return LOW;
    }
#ifdef Relays_debug
        Serial.print(" => ");
        Serial.println( (*portOutputRegister(port) & bit) ? "HIGH" : "LOW" );
#endif
    return (*portOutputRegister(port) & bit) ? HIGH : LOW;
}
#endif

boolean Relays::checkRelay(uint8_t relayPin)
{
    return false;
}

void Relays::setMaxRelay(int maxRelay)
{
    _maxRelay = maxRelay;
}

bool Relays::isSetup()
{
    return _setup;
}

uint8_t Relays::getRelayStatusIndex(uint8_t relayPin)
{
    uint8_t index = _RELAYS_MAX_STATUS_SIZE + 1;
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        if ( _status[i].relayPin() == relayPin ) {
            index = i;
        }
    return index;
}

uint8_t Relays::getPowerStatusIndex(uint8_t powerPin)
{
    uint8_t index = _RELAYS_MAX_STATUS_SIZE + 1;
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        if ( _status[i].powerPin() == powerPin ) {
            index = i;
        }
    return index;
}

void Relays::resetStatus()
{
#ifdef Relays_save
    RTC.setDataStatus(false);
#endif Relays_save
#ifdef Relays_at24
    at24.setDataStatus(false);
#endif Relays_at24
}

#ifdef Relays_save
void Relays::saveStatus() {
    _save.value = 0;
    RTC.setDataStatus(true);
    for( uint8_t i = 0 ; i < min(_statusSize,15) ; i++ ) {
        if (_status[i].isOn()) {
            _save.value |= (1 << i) ;
        }
    }
    _save.lasttime = now();
    RTC.setData(&_save);
}
#endif

#ifdef Relays_at24
void Relays::saveStatusAt24() {
    at24.setDataStatus(true);
    for( uint8_t i = 0 ; i < min(_statusSize,AT24_STATUS_SIZE) ; i++ ) {
        _status[i].backup(&_save.relay[i]);
    }
    _save.lasttime = now();
    at24.setData(&_save);
    if (at24.getDataStatus() && _looper > 15) {
        at24.getData(&_save);
    }
}
#endif

#ifdef Relays_print
String Relays::loopStringStatus() {
    String text = "Relays";
    for( int i = 0 ; i < _statusSize ; i++ ) {
        if ( _status[i].isSetup() ) {
            text += ", R";
            text += String(i+1,HEX);
            if( _status[i].isOn() ) {
                text += "=ON";
            } else {
                text += "=OFF";
            }
#ifdef Relays_power
            if (_status[i].isPower()) {
                text += ", A=";
                text += _status[i].getPower();
                text += ", W=";
                text += (float)((_status[i].getPower() * RELAYSTATUS_POWER_VOLTS)/1000);
            }
#endif Relays_power
        }
    }
    text += "\n";
    return text;
}
#endif

