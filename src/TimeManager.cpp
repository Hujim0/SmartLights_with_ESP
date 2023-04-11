#include <TimeManager.h>

TimeManager *TimeManager::Instance = 0;

void TimeManager::Setup(uint16_t epoch_time_seconds, int _dayOfTheWeek)
{
    Instance = this;

    dayOfTheWeek = _dayOfTheWeek;

    MillisOffset = (unsigned long)(epoch_time_seconds * (unsigned short)1000) - millis();

    _hours = (epoch_time_seconds / (unsigned short)3600);
    _minuts = (epoch_time_seconds % (unsigned short)3600) / (unsigned short)60;
    _seconds = epoch_time_seconds % (unsigned short)60;

    isSetuped = true;
}

void TimeManager::Update()
{
    if (!isSetuped)
        return;

    epoch_time_day_seconds = (unsigned short)(((MillisOffset + millis()) / 1000UL) % 86400UL);

    UpdateSeconds();
}

void TimeManager::UpdateHours()
{
    int timeHours = (int)((epoch_time_day_seconds / (unsigned short)3600));

    if (_hours == timeHours)
        return;

    _hours = timeHours;

    // time events (hour update)
    // probably unnecessary
}

void TimeManager::UpdateMinuts()
{
    int timeMins = (int)((epoch_time_day_seconds % (unsigned short)3600) / (unsigned short)60);

    if (_minuts == timeMins)
        return;

    _minuts = timeMins;

    UpdateHours();

    // time events (minute update)
}

void TimeManager::UpdateSeconds()
{
    int timeSecs = (int)(epoch_time_day_seconds % (unsigned short)60);

    if (timeSecs == _seconds)
        return;

    _seconds = timeSecs;

    UpdateMinuts();

    // time events (second update)

    Serial.println(GetFormattedTime());
}

String TimeManager::GetFormattedTime()
{
    if (!isSetuped)
        return "unknown";

    String time = "";

    if (_hours < 10)
        time += "0";
    time += String(_hours) + ":";
    if (_minuts < 10)
        time += "0";
    time += String(_minuts) + ":";
    if (_seconds < 10)
        time += "0";
    time += String(_seconds);

    return time;
}

TimeManager::TimeManager() {}