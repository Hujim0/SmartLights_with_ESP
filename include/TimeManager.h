#ifndef TIMEMANAGER_H
#define TIMEMANAGER_H

#include <Arduino.h>

typedef std::function<void(uint16_t epoch_time_day_secons, int day_of_the_week)> TimeEventHandler;

class TimeManager
{
private:
    uint16_t epoch_time_day_seconds = 0ULL;
    unsigned long MillisOffset = 0UL;
    int dayOfTheWeek = 0;
    TimeEventHandler timeEvents[0];
    int timeEventsCounter = 0;
    bool isSetuped = false;

    int _seconds;
    void UpdateSeconds();
    int _minuts;
    void UpdateMinuts();
    int _hours;
    void UpdateHours();

public:
    void Setup(uint16_t epoch_time_day_seconds, int dayOfTheWeek);
    void Update();
    String GetFormattedTime();

    void AddTimeEvent(TimeEventHandler handler);
    void RemoveLastTimeEvent();
    void RemoveTimeEvent(int index);

    TimeManager();

    static TimeManager *Instance;
};

#endif