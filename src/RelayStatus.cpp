//
//  RelayStatus 
//  Library C++ code
//  ----------------------------------
//  Developed with embedXcode
//
//  Relays
//  Created by jeroenjonkman on 13-06-15
//  Modified by jeroenjonkman on 31-07-19
// 

#include <RelayStatus.h>

void RelayStatus::setup(uint8_t relayPin)
{
    setup(relayPin,0,RELAYSTATUS_MODE_DEFAULT,false);
}

void RelayStatus::setup(uint8_t relayPin, uint16_t defaultMode)
{
    setup(relayPin,0,defaultMode,false);
}

void RelayStatus::setup(uint8_t relayPin, bool defaultOn)
{
    setup(relayPin,0,RELAYSTATUS_MODE_DEFAULT,defaultOn);
}

void RelayStatus::setup(uint8_t relayPin, int powerPin)
{
    setup(relayPin,powerPin,RELAYSTATUS_MODE_DEFAULT,false);
}

void RelayStatus::setup(uint8_t relayPin, int powerPin, bool defaultOn)
{
    setup(relayPin,powerPin,RELAYSTATUS_MODE_DEFAULT,defaultOn);
}

void RelayStatus::setup(uint8_t relayPin, int powerPin, uint16_t defaultMode)
{
    setup(relayPin,powerPin,defaultMode,false);
}

void RelayStatus::setup(uint8_t relayPin, int powerPin, uint16_t defaultMode, bool defaultOn)
{
    uint8_t powerType = ((powerPin & 0xFF00 ) >> 8);
#ifdef RelayStatus_debug_power
    Serial.print("RelayStatus::setup relayPin=");
    Serial.print(relayPin);
    Serial.print(", powerPin=");
    Serial.print(powerPin);
    Serial.print(", powerType=");
    Serial.print(powerType);
    Serial.print(", defaultMode=");
    Serial.print(defaultMode,BIN);
    Serial.print(", defaultOn=");
    Serial.println(defaultOn,BIN);
#endif
    _status         = 0;
    _relayPin       = relayPin;
    _powerPin       = powerPin & 0x00FF;
    _mode           = defaultMode & 0x00FF;
    _defaultMode    = defaultMode & 0x00FF;
#ifdef RelayStatus_power
    _power          = 0;
    _powerOffset    = 0;
#endif
    if (_relayPin > 0) {
        pinMode(_relayPin,OUTPUT);
        digitalWrite(_relayPin,_RELAYSTATUS_OFF);
    } else {
        bitWrite(_status,RELAYSTATUS_STATUS_DISABLED_BIT,true);
    }
    bitWrite(_status,RELAYSTATUS_STATUS_DEFAULT_ON_BIT,defaultOn);
    if (defaultOn) {
        relayOn(_mode);
    }
    if( _powerPin > 0) {
        bitWrite(_status,RELAYSTATUS_STATUS_POWER_BIT,true);
#ifdef RelayStatus_debug_power
        Serial.print("setup powertype=");
        Serial.println(powerType);
#endif
        switch (powerType) {
            case RELAYSTATUS_POWER_TYPE_0 >> 8:
                break;
            case RELAYSTATUS_POWER_TYPE_5 >> 8:
                bitWrite(_status,RELAYSTATUS_STATUS_A5_BIT,true);
                break;
            default:
            case RELAYSTATUS_POWER_TYPE_20 >> 8:
                bitWrite(_status,RELAYSTATUS_STATUS_A20_BIT,true);
                break;
            case RELAYSTATUS_POWER_TYPE_30 >> 8:
                bitWrite(_status,RELAYSTATUS_STATUS_A5_BIT,true);
                bitWrite(_status,RELAYSTATUS_STATUS_A20_BIT,true);
                break;
        }
    }
    bitWrite(_status,RELAYSTATUS_STATUS_SETUP_BIT,true);
#ifdef Relays_reset
    if (_status == 0 || _status >= RELAYSTATUS_STATUS_ERROR) {
        reset();
    }
#endif
}

void RelayStatus::loop()
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::loop relayPin");
    Serial.println(_relayPin);
#endif
    if (bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT)) {
#ifdef RelayStatus_debug_power
        Serial.print("RelayStatus::loop powerPin");
        Serial.print(_powerPin);
#endif
        _power = getPowerX(_RELAYSTATUS_POWER_CYCLES);
    }
#ifdef RelayStatus_debug_power
    Serial.print(",Power=");
    Serial.println(_power);
#endif
}

#ifdef Relays_at24
void RelayStatus::restore(at24Element_t *relay)
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::restore S=");
    Serial.print(relay->status,HEX);
    Serial.print(",M=");
    Serial.print(relay->mode,HEX);
    Serial.print(",D=");
    Serial.println(relay->defaultmode,HEX);
#endif
#ifdef Relays_reset
    if (relay->status == 0 || relay->status >= RELAYSTATUS_STATUS_ERROR) {
        reset();
    }
#endif
    _defaultMode = relay->defaultmode;
    if( relay->status & RELAYSTATUS_STATUS_ON) {
        relayOn(RELAYSTATUS_MODE_RESTORE);
    }
    _status = relay->status;
    _mode = relay->mode;
}

void RelayStatus::backup(at24Element_t *relay)
{
    relay->status = _status;
    relay->mode = _mode;
    relay->defaultmode = _defaultMode;
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::backup S=");
    Serial.print(relay->status,HEX);
    Serial.print(",M=");
    Serial.print(relay->mode,HEX);
    Serial.print(",D=");
    Serial.println(relay->defaultmode,HEX);
#endif
}
#endif Relays_at24


uint8_t RelayStatus::getMode() {
    return _mode;
}

uint8_t RelayStatus::getDefaultMode() {
    return _defaultMode;
}

uint8_t RelayStatus::relayPin()
{
    return _relayPin;
}

uint8_t RelayStatus::powerPin()
{
    return _powerPin;
}

void RelayStatus::setPowerPin(uint8_t powerPin)
{
    _powerPin = powerPin;
    if ( _powerPin > 0 ) {
        bitWrite(_status,RELAYSTATUS_STATUS_POWER_BIT,true);
    }
}

void RelayStatus::relayOn() {
    relayOn(RELAYSTATUS_MODE_TRIGGER);
}

void RelayStatus::relayOn(uint16_t mode)
{
    if (mode == RELAYSTATUS_MODE_ALL) {
        // Not allowed MODE_ALL
#ifdef RelayStatus_debug
        Serial.println("RelayStatus::relayOn mode=RELAYSTATUS_MODE_ALL!");
#endif
        return;
    }
    if ( isLocked() || isDisabled () ) {
        return;
    }
    if (!isSensorsOn() && (mode & RELAYSTATUS_MODE_SENSORS)) {
        return;
    }
    if( (mode & _mode) == mode || (mode & RELAYSTATUS_MODE_MANUAL)) {
#ifdef RelayStatus_debug_onoff
        Serial.print("RelayStatus::relayOn mode checked, mode=");
        Serial.print(mode,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
        Serial.print(" ");
#endif
        if (!isOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.print("SET ON ");
#endif
            digitalWrite(_relayPin,_RELAYSTATUS_ON);
            _mode = mode;
            setOn(true);
        }
        if (isOn() == isDefaultOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.print("reset to default ");
#endif
            _mode = _defaultMode;
            _timer = RELAYSTATUS_TIMER_NONE;
        }
#ifdef RelayStatus_debug
        Serial.println();
#endif
    } else {
        if( (mode & _defaultMode) == mode ) {
            // mode set, next event with a other mode but in the default mode
#ifdef RelayStatus_debug_onoff
            Serial.print("RelayStatus::relayOn defaultMode checked, mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.print(_mode,BIN);
            Serial.print(", _defaultMode=");
            Serial.print(_defaultMode,BIN);
            Serial.print(" ");
#endif
            if (isOn()) {
                // set extra mode
#ifdef RelayStatus_debug_onoff
                Serial.print("set extra mode.");
#endif
                _mode = _mode|mode;
            }
#ifdef RelayStatus_debug
            Serial.println();
        } else {
            Serial.print("RelayStatus::relayOn mode fault mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.println(_mode,BIN);
#endif
        }
    }
}

void RelayStatus::relayOff()
{
    relayOff(RELAYSTATUS_MODE_TRIGGER);
}

void RelayStatus::relayOff(uint16_t mode)
{
    if (mode == RELAYSTATUS_MODE_ALL) {
#ifdef RelayStatus_debug
        Serial.println("RelayStatus::relayOff mode=RELAYSTATUS_MODE_ALL");
#endif
        return;
    }
    if ( isLocked() || isDisabled () ) {
        return;
    }
    if (!isSensorsOn() && (mode & RELAYSTATUS_MODE_SENSORS)) {
        return;
    }
    if( (mode & _mode) == mode || (mode & RELAYSTATUS_MODE_MANUAL)) {
#ifdef RelayStatus_debug_onoff
        Serial.print("RelayStatus::relayOff mode checked, mode=");
        Serial.print(mode,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
        Serial.print(" ");
#endif
        if (isOn()) {
            digitalWrite(_relayPin,_RELAYSTATUS_OFF);
            _mode = mode;
#ifdef RelayStatus_debug_onoff
            Serial.print("SET OFF ");
#endif
            setOn(false);
        }
        if (isOn() == isDefaultOn()) {
#ifdef RelayStatus_debug_onoff
            Serial.println("reset to default");
#endif
            _mode = _defaultMode;
            _timer = RELAYSTATUS_TIMER_NONE;
        }
#ifdef RelayStatus_debug
        Serial.println();
#endif
    } else {
        if( (mode & _defaultMode) == mode ) {
            // mode set, next event with a other mode but in the default mode
#ifdef RelayStatus_debug_onoff
            Serial.print("RelayStatus::relayOff defaultMode checked, mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.print(_mode,BIN);
            Serial.print(", _defaultMode=");
            Serial.print(_defaultMode,BIN);
            Serial.print(" ");
#endif
            if (!isOn()) {
                // set extra mode
#ifdef RelayStatus_debug_onoff
                Serial.println("set extra mode");
#endif
                _mode = _mode|mode;
            }
#ifdef RelayStatus_debug
            Serial.println();
        } else {
            Serial.print("RelayStatus::relayOff mode fault mode=");
            Serial.print(mode,BIN);
            Serial.print(", _mode=");
            Serial.println(_mode,BIN);
#endif
        }
    }
}

void RelayStatus::relayLock(bool locked)
{
    bitWrite(_status,RELAYSTATUS_STATUS_LOCKED_BIT,locked);
}

bool RelayStatus::isSetup()
{
    return bitRead(_status,RELAYSTATUS_STATUS_SETUP_BIT);
}

#ifdef RelayStatus_debug

void RelayStatus::print()
{
    Serial.print("RelayStatus ");
    if (!isSetup()) {
        Serial.print("setup not runned.");
        return;
    }
    Serial.print(" relayPin=");
    Serial.print(_relayPin);
    Serial.print(", powerPin=");
    if (_powerPin>0) {
        Serial.print(_powerPin);
    } else {
        Serial.print("NONE");
    }
    Serial.print((isOn())?", NO":", OFF");
    Serial.print((isDefaultOn())?", default NO":", Default OFF");
    Serial.print(", mode=");
    Serial.print(_mode,BIN);
    Serial.print(", default mode=");
    Serial.print(_defaultMode,BIN);
    Serial.print(", status=");
    Serial.print(_status,BIN);
    
}

void RelayStatus::println()
{
    print();
    Serial.println();
}

#endif

bool RelayStatus::isOn() {
    return bitRead(_status,RELAYSTATUS_STATUS_ON_BIT);
}

bool RelayStatus::isDefaultOn() {
    return bitRead(_status,RELAYSTATUS_STATUS_DEFAULT_ON_BIT);
}

bool RelayStatus::isTimer() {
    return bitRead(_timer,RELAYSTATUS_TIMER_BIT);
}

bool RelayStatus::isLocked() {
    return bitRead(_status,RELAYSTATUS_STATUS_LOCKED_BIT);
}

bool RelayStatus::isPower() {
    return bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT);
}

bool RelayStatus::isDisabled() {
    return bitRead(_status,RELAYSTATUS_STATUS_DISABLED_BIT);
}

bool RelayStatus::isSensorsOn() {
    // Negative switch
    return !bitRead(_status,RELAYSTATUS_STATUS_NOSENSORS_BIT);
}

bool RelayStatus::isOke() {
    uint8_t status = _status & 0x00FF;
    return status > 0x01 && status < 0xFF;
}


void RelayStatus::setTimer(uint8_t delayType, uint16_t delay)
{
#ifdef RelayStatus_debug
    Serial.print("RelayStatus::setTimer delayType=");
    Serial.print(delayType);
    Serial.print(", delay=");
    Serial.print(delay);
#endif
    if (delayType != RELAYSTATUS_DELAYTYPE_NONE &&
        ( ( _defaultMode & RELAYSTATUS_MODE_TIMER ) == RELAYSTATUS_MODE_TIMER ||
          ( _defaultMode & RELAYSTATUS_MODE_TRIGGER ) == RELAYSTATUS_MODE_TRIGGER
        ) ) {
        _timer = delay & 0x0FFF;        // 12 bits
        _timer = _timer << 3;
        _timer += delayType & 0x07;     // 3 bits
        _timer = _timer << 1;
        _timer += 1;                    // 1 bit
        // Add RELAYSTATUS_MODE_TIMER to mode;
        _mode = _mode | RELAYSTATUS_MODE_TIMER;
#ifdef RelayStatus_debug
        Serial.print(", _timer=");
        Serial.print(_timer,BIN);
        Serial.print(", _mode=");
        Serial.print(_mode,BIN);
#endif
    } else {
        _timer = RELAYSTATUS_TIMER_NONE;
#ifdef RelayStatus_debug
        Serial.print(", _timer=");
        Serial.print(_timer,BIN);
#endif
    }
#ifdef RelayStatus_debug
    Serial.println();
#endif
}

#ifdef RelayStatus_power
uint16_t RelayStatus::getPower()
{
    if (bitRead(_status,RELAYSTATUS_STATUS_POWER_BIT)) {
        return _power;
    }
    return 0;
}

void RelayStatus::setPowerOffset(int8_t offset)
{
    _powerOffset = offset;
}
#endif

void RelayStatus::setSensorsOn(bool on)
{
    // Negative switch
    bitWrite(_status,RELAYSTATUS_STATUS_NOSENSORS_BIT,!on);
}

uint8_t RelayStatus::getTimerType()
{
    if (isTimer()) {
        return (_timer & RELAYSTATUS_TIMER_DELAYTYPE) >> 1;
    }
    return 0;
}

uint16_t RelayStatus::getTimerDelay()
{
    if (isTimer()) {
        return (_timer & RELAYSTATUS_TIMER_DELAY) >> 4;
    }
    return 0;
}

uint8_t RelayStatus::putXBeeStatus(ByteBuffer *buffer)
{
    uint8_t value = _status & 0x00FF;
    
    if( buffer->getFreeSize() > 0 ) {
        buffer->put(value);
        return 1;
    }
    return 0;
}

uint8_t RelayStatus::putXBeePower(ByteBuffer *buffer)
{
    uint16_t value = _power & 0xFFFF;
    if( buffer->getSize() > 0 ) {
        buffer->putU16(value);
        return 1;
    }
    return 0;
}

void RelayStatus::setOn(bool on)
{
    bitWrite(_status,RELAYSTATUS_STATUS_ON_BIT,on);
}

#ifdef RelayStatus_power

uint16_t RelayStatus::getPowerX(uint8_t count)
{
    double power = 0;
    count = (count > 0 && count <= 32)?count:1;
    if ((_status & RELAYSTATUS_STATUS_POWER_TYPE) == (RELAYSTATUS_POWER_TYPE_0 >> 4) ) {
#ifdef RelayStatus_debug_power
        Serial.print(",getPowerX TYPE 0");
#endif
        return 0;
    }
    for( uint8_t i=0;i<count;i++)
        power+=_getRawPower();
#ifdef RelayStatus_debug
    Serial.print("getPowerX X=" );
    Serial.print(count);
    Serial.print(", pin = ");
    Serial.println(_powerPin);
    Serial.print(", power = ");
    Serial.println(power / count);
#endif
    switch (_status & RELAYSTATUS_STATUS_POWER_TYPE ) {
        case (RELAYSTATUS_POWER_TYPE_5 >> 4):
#ifdef RelayStatus_debug_power
            Serial.print(",getPowerX TYPE 5");
#endif
            return (power / count) * RELAYSTATUS_POWER_AMPS_5;
            break;
        default:
        case (RELAYSTATUS_POWER_TYPE_20 >> 4):
#ifdef RelayStatus_debug_power
            Serial.print(",getPowerX TYPE 20");
#endif
            return (power / count) * RELAYSTATUS_POWER_AMPS_20;
            break;
        case (RELAYSTATUS_POWER_TYPE_30 >> 4):
#ifdef RelayStatus_debug_power
            Serial.print(",getPowerX TYPE 30");
#endif
            return (power / count) * RELAYSTATUS_POWER_AMPS_30;
            break;
    }
}

uint16_t RelayStatus::_getRawPower()
{
    uint16_t  _default  = _DEFAULT_POWER_VALUE + _powerOffset;
    uint16_t  _max      = _default;
    uint16_t  _min      = _default;
    //int       _sample   = _default;
    uint16_t  _count    = 0;
    uint16_t  _power    = _default;
    long      _start    = millis();
    long      _stop     = _start + _POWER_SAMPLE_SIZE;
    // Overrun protection
    _stop = (_stop<_start)?_start:_stop;
    while (millis() < _stop ) {
        uint16_t _sample = analogRead(_powerPin);
        _min = min(_min,_sample);
        _max = max(_max,_sample);
        _count++;
    }
    //_power = max(DEFAULT_VALUE-_min,_max-_default);
    _power = ((_default-_min) + (_max-_default))/2;
#ifdef RelayStatus_power_turning
    int maxdiff = _max-_default;
    int mindiff = _default-_min;
    int diff = maxdiff - mindiff;
    Serial.print("_powerPin=" );
    Serial.print((_powerPin-18)+0xA0,HEX);
    Serial.print(",Count=" );
    Serial.print(_count);
    Serial.print(",default=");
    Serial.print(_default);
    Serial.print(",offset=");
    Serial.print(_powerOffset);
    Serial.print(",raw_power=");
    Serial.print(_power);
    Serial.print(",min=");
    Serial.print(_min);
    Serial.print(",max=");
    Serial.print(_max);
    Serial.print(",mindiff=");
    Serial.print(mindiff);
    Serial.print(",maxdiff=");
    Serial.print(maxdiff);
    Serial.print(",diff=");
    Serial.print(diff);
    Serial.print(",newoffset=");
    Serial.print(_powerOffset + (diff/2));
#endif
    _power = max(0,_power - _POWER_TRIGGER_VALUE);
#ifdef RelayStatus_power_turning
    Serial.print(",power=");
    Serial.print(_power);
    Serial.println();
#endif
    return _power & 0xFFFF;
}
#endif



