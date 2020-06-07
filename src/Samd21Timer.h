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
        virtual void enable(TimerNumber timer, double freq, void(*callback)(),  Priority priority, GeneralClock gclk) = 0;  //Automatic selection of the timer resolution 
        virtual void enable(TimerNumber timer, double freq, void(*callback)(), TimerResolution res, uint8_t priority, GeneralClock gclk) = 0; //Manual selection of timer resolution
        virtual void disable(TimerNumber timer) = 0;
        virtual void disable_check() = 0; //permit to use params that are unknown at compile time
        virtual void unsafe_mode() = 0; //let use timer used by system library

    protected:
        bool _unsafe_mode = false;
        bool _disable_check = false;
        TimerInfo timer_info;
        Callbacks callbacks;
}; 


template <class TimerResolution, class TimerInfo>
class IChecker {
    public:
        virtual bool check_frequency(double freq, TimerResolution res) = 0;
        virtual bool check_compatibility(TimerInfo* timerinfo) = 0;
};









/****************************************************************
 ****************************************************************
 ****************     SAMD21 implementation    ******************
 ****************************************************************
 ****************************************************************/


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
    uint32_t mode_count;
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
        void enable(TimerNumberSamd21 timer, double freq, void(*callback)(), TimerResolutionSamd21 res = RESOLUTION_16_BIT, uint8_t priority = 0, GeneralClockSamd21 gclk = GCLK_5); //Manual selection of timer resolution
        void disable(TimerNumberSamd21 timer);
        void disable_check(); //permit to use params that are unknown at compile time
        void unsafe_mode(); //enable usage of timer 0 and timer 1

    private:
        TimerParamsSamd21 get_timer_params(double freq, TimerResolutionSamd21 res);
        template <class T> void set_timer(TimerNumberSamd21 timer, T timer_controller, TimerParamsSamd21* params, GeneralClockSamd21 gclk);
        void set_general_clock(TimerNumberSamd21 timer, GeneralClockSamd21 gclk);
        void set_NVIC(TimerNumberSamd21 timer, uint8_t priority);
        template <class T> bool isSyncing(T timer_controller);
        template <class T> void reset(T timer_controller);        
};



class CheckerSamd21 : public IChecker<TimerResolutionSamd21, TimerInfoSamd21> {
    public:
        bool check_frequency(double freq, TimerResolutionSamd21 res);
        bool check_compatibility(TimerInfoSamd21* timerinfo);
};


#ifdef __SAMD21G18A__ 
extern Samd21TimerClass Timer;
#endif