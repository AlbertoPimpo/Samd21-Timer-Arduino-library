#include "Samd21Timer.h"

//Implementation of Samd21TimerClass Methods


void Samd21TimerClass::enable(TimerNumberSamd21 timer, double freq, void(*callback)(),  uint8_t priority, GeneralClockSamd21 gclk){
    if(freq >= 0.75)
        this->enable(timer, freq, callback, RESOLUTION_16_BIT, priority, gclk);
    else 
        this->enable(timer, freq, callback, RESOLUTION_32_BIT, priority, gclk);
}


void Samd21TimerClass::enable(TimerNumberSamd21 timer, double freq, void(*callback)(), TimerResolutionSamd21 res, uint8_t priority, GeneralClockSamd21 gclk){
    
    TimerParamsSamd21 params = this->getTimerParams(freq, res);
    setGeneralClock(timer, gclk);

    if (res == RESOLUTION_32_BIT){
        TcCount32* TC;

        switch (timer){
            case TIMER_0:
                TC = (TcCount32*) TCC0;
                break;
            case TIMER_1:
                TC = (TcCount32*) TCC1;
                break;
            case TIMER_2:
                TC = (TcCount32*) TCC2;
                break;
            case TIMER_3:
                TC = (TcCount32*) TC3;
                break;
            case TIMER_4:
                TC = (TcCount32*) TC4;
                break;
            default:
                TC = (TcCount32*) TC5;
                break;
        };
        
        this->setTimer(timer, TC, &params, priority, gclk);

    }

    else if (res == RESOLUTION_16_BIT) {
        TcCount16* TC;

        switch (timer){
            case TIMER_0:
                TC = (TcCount16*) TCC0;
                break;
            case TIMER_1:
                TC = (TcCount16*) TCC1;
                break;
            case TIMER_2:
                TC = (TcCount16*) TCC2;
                break;
            case TIMER_3:
                TC = (TcCount16*) TC3;
                break;
            case TIMER_4:
                TC = (TcCount16*) TC4;
                break;
            default:
                TC = (TcCount16*) TC5;
                break;
        };

        this->setTimer(timer, TC, &params, priority, gclk);
    
    }

    else if (res == RESOLUTION_8_BIT){
        //TO DO 

    }

}


template <class TimerRegisters> void Samd21TimerClass::setTimer(TimerNumberSamd21 timer, TimerRegisters TC, TimerParamsSamd21* params, uint8_t priority, GeneralClockSamd21 gclk){
    this->reset(TC);

    TC->CTRLA.reg |= params->modeCount;
    TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
    TC->CTRLA.reg |= params->prescaler;

    TC->CC[0].reg = params->compare;
    
    while (this->isSyncing(TC));

    this->setNVIC(timer, priority);

    TC->INTENSET.bit.MC0 = 1;
    while (this->isSyncing(TC));

    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
    while (this->isSyncing(TC));

}


