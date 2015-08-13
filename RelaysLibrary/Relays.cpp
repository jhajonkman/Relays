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
    _statusSize = 0;
    _taskSize = 0;
    _taskID = 0;
    _setup = true;
    if( timeStatus() == timeSet ) {
        _lastTime = now();
    }
}

void Relays::loop()
{
    unsigned long m_seconds = millis();
    if (_last_run < m_seconds) {
#ifdef Relay_debug
        Serial.println("Relays::loop next run.");
#endif
        loopTask();
        loopStatus();
#ifdef Relay_power
        loopPower();
#endif
#ifdef Relay_print
        if (_looper%15==0 && _setup) {
            Serial.print(loopStringStatus());
        }
#endif
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
            if( task == RELAYTASK_TASK_TIME  ) {
                // Check time task
                for (uint8_t j = 0 ; j < _statusSize; j++) {
                    // Check relay task with RelayStatus
                    if ( (relay & 1<<j ) != (1<<j) ) {
                        continue;
                    }
                    // Check defaultMode of the RelayStatus
                    uint8_t defaultMode = _status[j].getDefaultMode();
#ifdef Relay_time_debug
                    Serial.print("Relays::loopTask Time defaultMode=");
                    Serial.print(defaultMode,BIN);
                    Serial.print(", RELAYSTATUS_MODE_TIME=");
                    Serial.println(RELAYSTATUS_MODE_TIME,BIN);
#endif
                    if ( (defaultMode & RELAYSTATUS_MODE_TIME) == RELAYSTATUS_MODE_TIME ) {
                        defaultMode = _status[j].getDefaultMode();
                        bool check = _task[i].checkTime(now());
#ifdef RelayTask_debug
                        Serial.print("Relays::loopTask Time, relay=");
                        Serial.print(_status[j].relayPin());
                        if (!check) {
                            Serial.println(" checked=false");
                        }
#endif
                        if (check && _task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayON");
#endif
                            _status[j].relayOn(RELAYSTATUS_MODE_TIME);
                        }
                        if (check && !_task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayOFF");
#endif
                            _status[j].relayOff(RELAYSTATUS_MODE_TIME);
                        }
                    }
                }
            }
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
#ifdef Relay_debug
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
                        if (check && _task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayON");
#endif
                            _status[j].relayOn(RELAYSTATUS_MODE_TEMPERATURE);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                        if (check && !_task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayOFF");
#endif
                            _status[j].relayOff(RELAYSTATUS_MODE_TEMPERATURE);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
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
#ifdef Relay_debug
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
                        if (check && _task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayON");
#endif
                            _status[j].relayOn(RELAYSTATUS_MODE_HUMIDITY);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                        if (check && !_task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayOFF");
#endif
                            _status[j].relayOff(RELAYSTATUS_MODE_HUMIDITY);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
            if (_light != _RELAYS_LIGHT_NOSET && task == RELAYTASK_TASK_LIGHT) {
                // Check light
#ifdef RelayTask_debug || RelayStatus_debug
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
#ifdef Relay_debug
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
                        if (check && _task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayON");
#endif
                            _status[j].relayOn(RELAYSTATUS_MODE_LIGHT);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                        if (check && !_task[i].isOn()) {
#ifdef RelayTask_debug
                            Serial.println(" relayOFF");
#endif
                            _status[j].relayOff(RELAYSTATUS_MODE_LIGHT);
                            if (_task[i].isTimeoutSet()) {
                                _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                            }
                        }
                    }
                }
            }
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
#ifdef Relay_time_debug
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
#ifdef Relay_time_debug
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
#ifdef Relay_time_debug
                        text += ", deltaMinute=";
                        text += deltaMinute;
#endif
                        delay -= deltaMinute;
#ifdef Relay_time_debug
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
#ifdef Relay_time_debug
                    } else {
                        text += "\n";
#endif
                    }
                    break;
                case RELAYSTATUS_DELAYTYPE_SECONDS:
#ifdef Relay_time_debug
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
#ifdef Relay_time_debug
                        text += ", deltaSeconds=";
                        text += deltaSeconds;
#endif
                        delay = (delay <= deltaSeconds)?0:delay - deltaSeconds;
#ifdef Relay_time_debug
                        text += ", delay=";
                        text += delay;
#endif
                        if (delay <= 1) {
#ifdef Relay_time_debug
                            text += " TIMER PASSED, ";
#endif
                            if (_status[j].isOn()) {
#ifdef Relay_time_debug
                                text += " SET OFF!\n";
#endif
                                _status[j].relayOff(RELAYSTATUS_MODE_TIMER);
                            } else {
#ifdef Relay_time_debug
                                text += " SET ON!\n";
#endif
                                _status[j].relayOn(RELAYSTATUS_MODE_TIMER);
                            }
                        } else {
#ifdef Relay_time_debug
                            text += "\n";
#endif
                            _status[j].setTimer(RELAYSTATUS_DELAYTYPE_SECONDS,delay);
                        }
                    }
                    break;
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
    Serial.print(text);
    //return text;
}

#ifdef Relay_power
void Relays::loopPower()
{
#ifdef Relay_debug
    Serial.println("Relays::loopPower");
#endif
    for( int i = 0 ; i < _statusSize ; i++ ) {
        if (_looper%(_statusSize) == i) {
            _status[i].loop();
        }
    }
}
#endif

int Relays::addRelay( uint8_t relayPin )
{
    return addRelay(relayPin, 0, false, RELAYSTATUS_MODE_DEFAULT);
}

int Relays::addRelay( uint8_t relayPin, bool defaultOn)
{
    return addRelay(relayPin, 0, defaultOn,RELAYSTATUS_MODE_DEFAULT);
}

int Relays::addRelay( uint8_t relayPin, uint16_t defaultMode)
{
    return addRelay(relayPin, 0, false,defaultMode);
}

int Relays::addRelay( uint8_t relayPin, int16_t powerPin)
{
    return addRelay(relayPin, powerPin, false, RELAYSTATUS_MODE_DEFAULT);
}
int Relays::addRelay( uint8_t relayPin, int16_t powerPin, bool defaultOn)
{
    return addRelay(relayPin, powerPin, defaultOn, RELAYSTATUS_MODE_DEFAULT);
}
                     
int Relays::addRelay( uint8_t relayPin, int16_t powerPin, bool defaultOn, uint16_t defaultMode)
{
    if ( _statusSize < _RELAYS_MAX_STATUS_SIZE ) {
#ifdef Relay_debug
            Serial.print("Relays::add: relayPin=");
            Serial.print(relayPin);
            if ( powerPin != 0 ) {
                Serial.print(", powerPin=");
                Serial.print(powerPin);
            }
            Serial.println();
#endif
        RelayStatus status;
        status.setup(relayPin,powerPin,defaultMode,defaultOn);
        _status[_statusSize] = status;
    } else {
        Serial.println("Relays::add: ERROR OUT OF ARRAY!");
        return -1;
    }
    _statusSize++;
    return _statusID++;
}

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

int Relays::addTaskTemperature( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setTemperature(operatortype,value);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}

int Relays::addTaskHumidity( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setHumidity(operatortype,value);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}

int Relays::addTaskLight( uint16_t relays, bool on, uint8_t operatortype, int value)
{
    if (_taskSize < _RELAYS_MAX_TASK_SIZE) {
        RelayTask task;
        task.setup();
        task.setOn(on);
        task.setRelay(relays);
        task.setLight(operatortype,value);
        _task[_taskSize] = task;
        _taskSize++;
        return _taskID++;
    }
    return -1;
}

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
#ifdef Relay_debug
        Serial.print("Relay::power totalPower=");
        Serial.print(totalPower);
        Serial.print(", count=");
        Serial.println(count);
#endif
    return max(totalPower / count,0);
}

void Relays::setTemperature(int temperature)
{
#ifdef Relay_set_debug
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
#ifdef Relay_set_debug
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
#ifdef Relay_set_debug
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
#ifdef Relay_time_debug
                    Serial.print("Relays::trigger relayPin=");
                    Serial.print(_status[j].relayPin());
                    Serial.print(", timerType=");
                    Serial.print(_status[j].getTimerType());
                    Serial.print(", timerDelay=");
                    Serial.print(_status[j].getTimerDelay());
#endif
                    if (_task[i].isOn()) {
#ifdef Relay_time_debug
                        Serial.println(" SET ON!");
#endif
                        _status[j].relayOn(RELAYSTATUS_MODE_TRIGGER);
                        if (_task[i].isTimeoutSet()) {
                            _status[j].setTimer(_task[i].getTimerType(),_task[i].getTimerDelay());
                        }
                    } else {
#ifdef Relay_time_debug
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

uint8_t Relays::putXBeeData(ByteBuffer *buffer)
{
    if (putXBeeTime(buffer)) {
        if (putXBeeStatus(buffer)) {
            if (putXBeePower(buffer)) {
                return 1;
            }
        }
    }
    return 0;
}

uint8_t Relays::putXBeeTime(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= 5) {
        if(!buffer->put(XBEE_TIME_HEADER)) {
            return 0;
        }
        return buffer->putTime(_lastTime);
    }
    return 0;
}

uint8_t Relays::putXBeeStatus(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= (2 + _statusSize + 1)) {
        if(!buffer->put(XBEE_RELAY_HEADER)){
            return 0;
        }
        if (!buffer->put(_statusSize)) {
            return 0;
        }
        for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
            if(!_status[i].putXBeeStatus(buffer)) {
                return 0;
            }
        }
        return 1;
    }
    return 0;
}

uint8_t Relays::putXBeePower(ByteBuffer *buffer)
{
    if (buffer->getFreeSize() >= (2 + (_statusSize + 1) * 2)) {
        uint8_t count = 0;
        for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
            if(_status[i].isPower() > 0) {
                count++;
            }
        }
        if (count == 0) {
            return 1;
        }
        if(!buffer->put(XBEE_POWER_HEADER)){
            return 0;
        }
        if (!buffer->put(count)) {
            return 0;
        }
        for( uint8_t i = 0 ; i < _statusSize ; i++ ) {
            if(_status[i].isPower()) {
                if(!_status[i].putXBeePower(buffer)) {
                    return 0;
                }
            }
        }
        return 1;
    }
    return 0;
}

void Relays::relaysOn() {
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        relayOn(_status[i].relayPin() );
}

void Relays::relayOn(uint8_t relayPin) {
    uint8_t index = getRelayStatusIndex(relayPin);
    if (index <= _RELAYS_MAX_STATUS_SIZE && _status[index].isSetup()) {
        _status[index].relayOn();
    }
}

void Relays::relayOn(int relayID) {
    if (relayID <= _RELAYS_MAX_STATUS_SIZE && _status[relayID].isSetup()) {
        _status[relayID].relayOn(RELAYSTATUS_MODE_MANUAL);
    }
}

void Relays::relaysOff() {
    for( uint8_t i = 0 ; i < _statusSize ; i++ )
        relayOff(_status[i].relayPin() );
}

void Relays::relayOff(uint8_t relayPin) {
    uint8_t index = getRelayStatusIndex(relayPin);
    if (index <= _RELAYS_MAX_STATUS_SIZE && _status[index].isSetup()) {
        _status[index].relayOff();
    }
}

void Relays::relayOff(int relayID) {
    if (relayID <= _RELAYS_MAX_STATUS_SIZE && _status[relayID].isSetup()) {
        _status[relayID].relayOff(RELAYSTATUS_MODE_MANUAL);
    }
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
#ifdef Relay_debug
    Serial.print("digitalReadOutputPin pin=");
    Serial.print(pin);
    Serial.print(", bit=");
    Serial.print(bit);
    Serial.print(", port=");
    Serial.print(port);
#endif
    if (port == NOT_A_PIN) {
#ifdef Relay_debug
            Serial.println("");
#endif
        return LOW;
    }
#ifdef Relay_debug
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


#ifdef Relay_print
String Relays::loopStringStatus() {
    text = "Relays";
    for( int i = 0 ; i < _statusSize ; i++ ) {
        if ( _status[i].isSetup() ) {
            text += ", R";
            text.concat(i+1,HEX);
            if( _status[i].isOn() ) {
                text += "=ON";
            } else {
                text += "=OFF";
            }
            text += ", A=";
            text += _status[i].getPower();
            text += ", W=";
            text += (float)(_status[i].getPower() * RELAYSTATUS_POWER_VOLTS)/1000);
        }
    }
    text += "\n";
    return text
}
#endif

