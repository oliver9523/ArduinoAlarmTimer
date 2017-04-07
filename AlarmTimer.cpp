#include "AlarmTimer.h"

AlarmTimer::AlarmTimer(void){
    TotalTime = 0;

    FirstStarted = false;
    FirstStartTime = 0;
    
    StartTime = 0;
    ElapsedTime = 0;

    AlarmTime = 0;
    AlarmSet = false;
    AlarmStarted = false;
    AlarmTriggered = false;
    AlarmRepeat = 1;

    CurrentTime = 0;
}

void AlarmTimer::Start(){
    StartTime = millis();
    if (!FirstStarted){
        FirstStartTime = StartTime;
        FirstStarted = true;
    }
}

unsigned long AlarmTimer::Stop(){
    //TODO - Make it stop everything to save resources
    CurrentTime = millis();
    ElapsedTime = (CurrentTime - StartTime);
    TotalTime = (CurrentTime - FirstStartTime);
    return ElapsedTime;
}

unsigned long AlarmTimer::Reset(){
    unsigned long duration = Stop();
    Start();
    return duration;
}

void AlarmTimer::StartAlarmTimer(){
    AlarmStartTime = millis();
    AlarmStarted = true;
}

void AlarmTimer::StopAlarmTimer(){
    AlarmSet = false;
    AlarmStarted = false;
}

void AlarmTimer::ResetAlarmTimer(){
    StartAlarmTimer();
}

void AlarmTimer::CheckAlarm(){
    if (!AlarmSet) return;
    if (!AlarmStarted) return;

    AlarmElapsed = millis() - AlarmStartTime;
    if (AlarmElapsed >= AlarmTime) {
        if (AlarmRepeat > 0) {
            AlarmRepeat--;
        }
        if (AlarmRepeat == 0) {
            AlarmSet = false;
        }
        TriggerAlarm();
        ResetAlarmTimer();
    }
}

void AlarmTimer::TriggerAlarm(){
    //TODO - Add a callback manager for multiple callbacks
    (*callbackfn)();
}

unsigned long AlarmTimer::GetStartTime(){
    return StartTime;
}

unsigned long AlarmTimer::GetElapsed(){
    return millis() - StartTime;
}

bool AlarmTimer::CheckElapsed(unsigned long elapsed){
    if (GetElapsed() < elapsed) return false;
    return true;
}

unsigned long AlarmTimer::GetTotalTime(){
    //TODO - protect against overlfow
    return TotalTime;
}

void AlarmTimer::SetAlarm(unsigned long duration, void (*callback)()){
    //TODO - Add a callback manager for multiple callbacks
    AlarmTime = duration;
    AlarmSet = true;
    AlarmRepeat = 1;
    callbackfn = callback;
}

void AlarmTimer::SetAlarm(unsigned long duration, void (*callback)(), int repeat){
    //TODO - Add a callback manager for multiple callbacks
    AlarmTime = duration;
    AlarmSet = true;
    AlarmRepeat = repeat;
    callbackfn = callback;
}

void AlarmTimer::AddTimeToAlarm(unsigned long ExtraTime, unsigned long MaxLength){
    if (!AlarmSet) return;
    if (AlarmTriggered){
        ResetAlarmTimer();
        AlarmSet = true;
        AlarmTriggered = false;
        AlarmRepeat = 1;
        AlarmTime = ExtraTime;
    }
    CurrentTime = millis();
    AlarmTime = CurrentTime + GetTimeLeftOnAlarm() + ExtraTime;
    AlarmTime = min(AlarmTime, CurrentTime+MaxLength);
}

unsigned long AlarmTimer::GetTimeLeftOnAlarm() {
    if (AlarmSet) return AlarmTime - (millis() - AlarmStartTime);
    return 0;
}

void AlarmTimer::Update(){
    CurrentTime = millis();
    if (AlarmSet && !AlarmTriggered) CheckAlarm();
}

void AlarmTimer::AddAlarm(unsigned long duration, void (*callback)(), int repeat){
    for (int i=0; i<MAX_NUM_ALARMS; i++){
        if (!Alarms[i].SET) {
            Alarms[i].Set(duration,callback,repeat);
        }
    }
}