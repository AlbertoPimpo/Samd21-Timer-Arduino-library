#include <Arduino.h>
#ifndef _SAMD21_TIMER_
#define _SAMD21_TIMER_





template <
    class TimerNumber,
    class TimerResolution,
    class TimerParams,
    class TimerFlags,
    class Callbacks,
    class GeneralClock,
    class Priority
>
class ITimer {
    public:
        virtual void enable(TimerNumber timer, float freq, void(*callback)(),  Priority priority, GeneralClock gclk) = 0;  //Automatic selection of the timer resolution 
        virtual void enable(TimerNumber timer, float freq, void(*callback)(), TimerResolution res, Priority priority, GeneralClock gclk) = 0; //Manual selection of timer resolution
        virtual void disable(TimerNumber timer) = 0;
}; 








/****************************************************************
 ****************************************************************
 ****************     SAMD21 implementation    ******************
 ****************************************************************
 ****************************************************************/


#ifdef __SAMD21G18A__ 
#define SAMD21_CLK 48000000

typedef enum{
    TIMER_3 = 3,
    TIMER_4 = 4,
    TIMER_5 = 5
} TimerNumberSamd21;

typedef enum{
    RESOLUTION_16_BIT = 16,
    RESOLUTION_32_BIT = 32
} TimerResolutionSamd21;

typedef struct{
    uint32_t modeCount;
    uint32_t prescaler;
    uint32_t compare;
} TimerParamsSamd21;


typedef struct{
    bool isActive = false;
    TimerResolutionSamd21 res; 
} TimerFlagsSamd21;


typedef struct{
    void (*timer_3_routine)();
    void (*timer_4_routine)();
    void (*timer_5_routine)();
} CallbacksSamd21;

typedef enum{
    GCLK_0 = 0,
    GCLK_1 = 1,
    GCLK_2 = 2,
    GCLK_3 = 3,
    GCLK_4 = 4,
    GCLK_5 = 5
} GeneralClockSamd21;






class Samd21TimerClass : public ITimer<
    TimerNumberSamd21, 
    TimerResolutionSamd21, 
    TimerParamsSamd21, 
    TimerFlagsSamd21, 
    CallbacksSamd21, 
    GeneralClockSamd21, 
    uint8_t> {
    public:
        static const int timerAmount = 6;
        TimerFlagsSamd21 timerFlags[timerAmount];
        CallbacksSamd21 callbacks;
        Tc* timerLookUp[timerAmount] = {NULL, NULL, NULL, TC3, TC4, TC5};
        void enable(TimerNumberSamd21 timer, float freq, void(*callback)(),  uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5);  //Automatic selection of the timer resolution 
        void enable(TimerNumberSamd21 timer, float freq, void(*callback)(), TimerResolutionSamd21 res, uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5); //Manual selection of timer resolution
        void disable(TimerNumberSamd21 timer);
        template <class TimerRegisters> void setTimerBit(TimerRegisters TC);     

    private:
        TimerParamsSamd21 getTimerParams(float freq, TimerResolutionSamd21 res);
        void setGeneralClock(TimerNumberSamd21 timer, GeneralClockSamd21 gclk);
        template <class TimerRegisters> void setTimerRegisters(TimerNumberSamd21 timer, TimerRegisters TC, TimerParamsSamd21* params,  uint8_t priority, GeneralClockSamd21 gclk);
        template <class TimerRegisters> void reset(TimerRegisters TC); 
        template <class TimerRegisters> bool isSyncing(TimerRegisters TC);
        void setNVIC(TimerNumberSamd21 timer, uint8_t priority);
        void setCallback(TimerNumberSamd21 timer, void(*callback)());

};





extern void TC3_Handler();
extern void TC4_Handler();
extern void TC5_Handler();



extern Samd21TimerClass Timer;



#endif  //__SAMD21G18A__



#endif  //_SAMD21_TIMER_