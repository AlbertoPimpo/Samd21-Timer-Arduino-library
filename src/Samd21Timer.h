#include <Arduino.h>
#ifndef _SAMD21_TIMER_
#define _SAMD21_TIMER_
// Used StaticAssert as a specialization of bool template instead of built-in one in order to grant 
// back compatibility with old standards
template <bool b>
struct StaticAssert {};

template <>
struct StaticAssert<true> {
    static void assert() {}
};



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
        //virtual void disableCheck() = 0; //permit to use params that are unknown at compile time
    protected:
        //virtual bool isCheckEnabled() = 0;
        //virtual bool isUnsafeModeEnabled() = 0;
}; 



//refactor change timerinfo to timer flags array
template <class TimerNumber, class TimerResolution, class TimerFlags>
class IChecker {
    public:
        virtual bool checkResolution(TimerNumber timer, double freq, TimerResolution res) = 0;
        virtual bool checkFrequency(double freq, TimerResolution res) = 0;
        virtual bool checkCompatibility(TimerFlags timerFlags[]) = 0;
};









/****************************************************************
 ****************************************************************
 ****************     SAMD21 implementation    ******************
 ****************************************************************
 ****************************************************************/


#ifdef __SAMD21G18A__ 
//refactor, change this macro with the built-in one
#define SAMD21_CLK 48000000

typedef enum{
    TIMER_0 = 0,
    TIMER_1 = 1,
    TIMER_2 = 2,
    TIMER_3 = 3,
    TIMER_4 = 4,
    TIMER_5 = 5
} TimerNumberSamd21;

typedef enum{
    RESOLUTION_8_BIT = 8,
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
    void (*timer_0_routine)();
    void (*timer_1_routine)();
    void (*timer_2_routine)();
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




//refactor check unused template class

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
        Tc* timerLookUp[timerAmount] = {(Tc*) TCC0, (Tc*) TCC1, (Tc*) TCC2, TC3, TC4, TC5};
        void enable(TimerNumberSamd21 timer, float freq, void(*callback)(),  uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5);  //Automatic selection of the timer resolution 
        void enable(TimerNumberSamd21 timer, float freq, void(*callback)(), TimerResolutionSamd21 res, uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5); //Manual selection of timer resolution
        void disable(TimerNumberSamd21 timer);
        void disableCheck(); //permit to use params that are unknown at compile time
        template <class TimerRegisters> void setTimerBit(TimerRegisters TC);     

    private:
        bool _unsafeMode = false;
        bool _disableCheck = false;
        bool isCheckEnabled();
        bool isUnsafeModeEnabled();
        TimerParamsSamd21 getTimerParams(float freq, TimerResolutionSamd21 res);
        void setGeneralClock(TimerNumberSamd21 timer, GeneralClockSamd21 gclk);
        template <class TimerRegisters> void setTimerRegisters(TimerNumberSamd21 timer, TimerRegisters TC, TimerParamsSamd21* params,  uint8_t priority, GeneralClockSamd21 gclk);
        template <class TimerRegisters> void reset(TimerRegisters TC); 
        template <class TimerRegisters> bool isSyncing(TimerRegisters TC);
        void setNVIC(TimerNumberSamd21 timer, uint8_t priority);
        void setCallback(TimerNumberSamd21 timer, void(*callback)());

};



// class CheckerSamd21 : public IChecker<TimerNumberSamd21, TimerResolutionSamd21, TimerInfoSamd21> {
//     public:
//         bool checkResolution(TimerNumberSamd21 timer, double freq, TimerResolutionSamd21 res);
//         bool checkFrequency(double freq, TimerResolutionSamd21 res);
//         bool checkCompatibility(TimerInfoSamd21* timerinfo);
// };





extern void TCC0_Handler();
extern void TCC1_Handler();
extern void TCC2_Handler();
extern void TC3_Handler();
extern void TC4_Handler();
extern void TC5_Handler();



extern Samd21TimerClass Timer;



#endif  //__SAMD21G18A__



#endif  //_SAMD21_TIMER_