#include <Arduino.h>


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
    class TimerInfo,
    class Callbacks,
    class GeneralClock,
    class Priority
>
class ITimer {
    public:
        Callbacks callbacks;
        TimerInfo timerInfo;
        virtual void enable(TimerNumber timer, double freq, void(*callback)(),  Priority priority, GeneralClock gclk) = 0;  //Automatic selection of the timer resolution 
        virtual void enable(TimerNumber timer, double freq, void(*callback)(), TimerResolution res, Priority priority, GeneralClock gclk) = 0; //Manual selection of timer resolution
        virtual void disable(TimerNumber timer) = 0;
        virtual void disableCheck() = 0; //permit to use params that are unknown at compile time
        virtual void unsafeMode() = 0; //let use timer used by system library

    protected:
        bool _unsafeMode = false;
        bool _disableCheck = false;
        virtual bool isCheckEnabled() = 0;
        virtual bool isUnsafeModeEnabled() = 0;
}; 


template <class TimerNumber, class TimerResolution, class TimerInfo>
class IChecker {
    public:
        virtual bool checkResolution(TimerNumber timer, double freq, TimerResolution res) = 0;
        virtual bool checkFrequency(double freq, TimerResolution res) = 0;
        virtual bool checkCompatibility(TimerInfo* timerinfo) = 0;
};









/****************************************************************
 ****************************************************************
 ****************     SAMD21 implementation    ******************
 ****************************************************************
 ****************************************************************/


#ifdef __SAMD21G18A__ 


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
} TimerFlagSamd21;


typedef struct{
    TimerFlagSamd21 Timer0;
    TimerFlagSamd21 Timer1;
    TimerFlagSamd21 Timer2;
    TimerFlagSamd21 Timer3;
    TimerFlagSamd21 Timer4;
    TimerFlagSamd21 Timer5;
} TimerInfoSamd21;

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



class Samd21TimerClass : public ITimer<
    TimerNumberSamd21, 
    TimerResolutionSamd21, 
    TimerParamsSamd21, 
    TimerInfoSamd21, 
    CallbacksSamd21, 
    GeneralClockSamd21, 
    uint8_t> {
    public:
        void enable(TimerNumberSamd21 timer, double freq, void(*callback)(),  uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5);  //Automatic selection of the timer resolution 
        void enable(TimerNumberSamd21 timer, double freq, void(*callback)(), TimerResolutionSamd21 res, uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5); //Manual selection of timer resolution
        void disable(TimerNumberSamd21 timer);
        void disableCheck(); //permit to use params that are unknown at compile time
        void unsafeMode(); //enable usage of timer 0 and timer 1
        template <class TimerRegisters> void setTimerBit(TimerRegisters TC);       

    private:
        bool isCheckEnabled();
        bool isUnsafeModeEnabled();
        TimerParamsSamd21 getTimerParams(double freq, TimerResolutionSamd21 res);
        template <class TimerRegisters> void setTimer(TimerNumberSamd21 timer, TimerRegisters TC, TimerParamsSamd21* params,  uint8_t priority, GeneralClockSamd21 gclk);
        void setGeneralClock(TimerNumberSamd21 timer, GeneralClockSamd21 gclk);
        void setNVIC(TimerNumberSamd21 timer, uint8_t priority);
        template <class TimerRegisters> bool isSyncing(TimerRegisters TC);
        template <class TimerRegisters> void reset(TimerRegisters TC); 
};



class CheckerSamd21 : public IChecker<TimerNumberSamd21, TimerResolutionSamd21, TimerInfoSamd21> {
    public:
        bool checkResolution(TimerNumberSamd21 timer, double freq, TimerResolutionSamd21 res);
        bool checkFrequency(double freq, TimerResolutionSamd21 res);
        bool checkCompatibility(TimerInfoSamd21* timerinfo);
};


extern Samd21TimerClass Timer;


void TCC0_Handler();
void TCC1_Handler();
void TCC2_Handler();
void TC3_Handler();
void TC4_Handler();
void TC5_Handler();


#endif