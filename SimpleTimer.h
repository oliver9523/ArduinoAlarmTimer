//TODO - protect against overflow after ~50 days when millis()
//resets to 0

#ifndef SIMPLETIMER_H
#define SIMPLETIMER_H

#include <Arduino.h>

#define MAX_NUM_ALARMS 16

enum {RUNNING, ENDED, NOT_SET, SET};

class Alarm {
  public:
    int STATE;
    bool SET;
    int AlarmRepeat;
    unsigned long CurrentTime;
    unsigned long AlarmTime;
    unsigned long AlarmStartTime;
    unsigned long AlarmElapsed;
    bool Finished;
    void (*callbackfn)(void);

    Alarm (){
        STATE = NOT_SET;
        SET = false;
    }

    void Start(){
        if (SET) {
            AlarmStartTime = millis();
            STATE = RUNNING;
            Finished = false;
        }
        STATE = NOT_SET;
    }

    void Stop(){
        STATE = ENDED;
    }

    void Reset(){
        Start();
    }

    void Set(long duration, void (*callback)(void), int repeat){
        STATE = RUNNING;
        SET = true;
        AlarmTime = duration;
        callbackfn = callback;
    }

    void Update(long ct){
        if (SET && STATE == RUNNING){
            CurrentTime = ct;
        }
    }

    void Trigger(){
        (*callbackfn)();
    }

    void AddTimeToAlarm(long ExtraTime, long MaxLength){
        if (SET && STATE == RUNNING){
            CurrentTime = millis();
            AlarmTime = CurrentTime + GetTimeLeftOnAlarm() + ExtraTime;
            AlarmTime = min(AlarmTime, CurrentTime+MaxLength);
        }
    }

    unsigned long GetTimeLeftOnAlarm() {
        if (SET) return AlarmTime - (millis() - AlarmStartTime);
        return 0UL;
    }

    void CheckAlarm(){
        if (!SET) return;
        if (STATE != RUNNING) return;

        AlarmElapsed = millis() - AlarmStartTime;
        if (AlarmElapsed >= AlarmTime) {
            if (AlarmRepeat > 0) {
                AlarmRepeat--;
            }
            if (AlarmRepeat == 0) {
                SET = false;
                STATE = ENDED;
            }
            Trigger();
            Reset();
        }
    }

};


class SimpleTimer {
private:

    unsigned long TotalTime;

    long FirstStartTime;        //time the object is created
    bool FirstStarted;          //flag that first time is set

    unsigned long StartTime;             //Start of the timer
    unsigned long ElapsedTime;           //Elapsed time since StartTime

    unsigned long AlarmStartTime;        //Alarm clock
    unsigned long AlarmTime;
    unsigned long AlarmElapsed;
    bool AlarmSet;              //alarm set flag
    bool AlarmStarted;
    bool AlarmTriggered;
    int AlarmRepeat;            //-1 repeat forever

    unsigned long CurrentTime;

    Alarm Alarms[MAX_NUM_ALARMS];

    void (*callbackfn)(void);

    void TriggerAlarm();
public:
    SimpleTimer(void);
    
    void Start();
    unsigned long Stop();
    unsigned long Reset();

    void StartAlarmTimer();
    void StopAlarmTimer();
    void ResetAlarmTimer();
    void CheckAlarm();

    unsigned long GetStartTime();
    unsigned long GetElapsed();
    unsigned long GetTotalTime();
    bool CheckElapsed(unsigned long elapsed);

    void SetAlarm(unsigned long duration, void (*callback)(void));
    void SetAlarm(unsigned long duration, void (*callback)(void), int repeat);
    void AddTimeToAlarm(unsigned long ExtraTime, unsigned long MaxLength);
    unsigned long GetTimeLeftOnAlarm();

    void Update();

    void AddAlarm(unsigned long duration, void (*callback)(void), int repeat);
    
};

#endif