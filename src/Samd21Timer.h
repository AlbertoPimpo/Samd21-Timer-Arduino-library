#include <Arduino.h>

typedef enum{
    RESOLUTION_8_BIT = 8,
    RESOLUTION_16_BIT = 16,
    RESOLUTION_32_BIT = 32
} TimerResolution;

typedef enum{
    TIMER_0 = 0,
    TIMER_1 = 1,
    TIMER_2 = 2,
    TIMER_3 = 3,
    TIMER_4 = 4,
    TIMER_5 = 5
} TimerNumber;

typedef enum{
    GCLK_0 = 0,
    GCLK_1 = 1,
    GCLK_2 = 2,
    GCLK_3 = 3,
    GCLK_4 = 4,
    GCLK_5 = 5
} GeneralClock;

typedef struct{
    uint32_t mode_count;
    uint32_t prescaler;
    uint32_t compare;
} TimerParams;


//flags to know the state of the timer, used for compile-time check
typedef struct{
    bool isActive = false;
    TimerResolution res; 
} TimerFlag;


typedef struct{
    TimerFlag Timer0;
    TimerFlag Timer1;
    TimerFlag Timer2;
    TimerFlag Timer3;
    TimerFlag Timer4;
    TimerFlag Timer5;
} TimerInfo;

typedef struct{
    void (*timer_0_routine)();
    void (*timer_1_routine)();
    void (*timer_2_routine)();
    void (*timer_3_routine)();
    void (*timer_4_routine)();
    void (*timer_5_routine)();
} Callbacks;


class Samd21TimerClass {
    public:
        void enable(TimerNumber timer, double freq, void(*callback)(),  uint8_t priority = 0, GeneralClock gclk = GCLK_5);  //Automatic selection of the timer resolution 
        void enable(TimerNumber timer, double freq, void(*callback)(), TimerResolution res = RESOLUTION_16_BIT, uint8_t priority = 0, GeneralClock gclk = GCLK_5); //Manual selection of timer resolution
        void disable(TimerNumber timer);
        void disable_check(); //permit to use params that are unknown at compile time
        void unsafe_mode(); //enable usage of timer 0 and timer 1

    private:
        bool _unsafe_mode = false;
        bool _disable_check = false;
        TimerInfo timer_info;
        Callbacks callbacks;

        TimerParams get_timer_params(double freq, TimerResolution res);
        template <class T> void set_timer(TimerNumber timer, T timer_controller, TimerParams* params, GeneralClock gclk);
        void set_general_clock(TimerNumber timer, GeneralClock gclk);
        void set_NVIC(TimerNumber timer, uint8_t priority);
        template <class T> bool isSyncing(T timer_controller);
        template <class T> void reset(T timer_controller);        
};


// Used StaticAssert as a specialization of bool template instead of built-in one in order to grant 
// back compatibility with old standards
template <bool b>
struct StaticAssert {};

template <>
struct StaticAssert<true> {
    static void assert() {}
};


class InputChecker {
    public:
        bool check_frequency(double freq, TimerResolution res);
        bool check_compatibility(TimerInfo* timerinfo);
};

extern Samd21TimerClass Timer;