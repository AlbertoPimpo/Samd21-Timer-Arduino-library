#include "Samd21Timer.h"

//Implementation of Samd21TimerClass Methods

#ifdef __SAMD21G18A__
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






void TCC0_Handler(){
    Timer.callbacks.timer_0_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer0.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TCC0);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TCC0);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TCC0);
            break;
    };
}


void TCC1_Handler(){
    Timer.callbacks.timer_1_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer1.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TCC1);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TCC1);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TCC1);
            break;
    };
}


void TCC2_Handler(){
    Timer.callbacks.timer_2_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer2.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TCC2);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TCC2);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TCC2);
            break;
    };
}


void TC3_Handler(){
    Timer.callbacks.timer_3_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer3.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TC3);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TC3);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TC3);
            break;
    };
}


void TC4_Handler(){
    Timer.callbacks.timer_4_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer4.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TC4);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TC4);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TC4);
            break;
    };
}


void TC5_Handler(){
    Timer.callbacks.timer_5_routine();
    //enable interrupt again
    switch(Timer.timerInfo.Timer5.res){
        case RESOLUTION_32_BIT:
            Timer.setTimerBit((TcCount32*) TC5);
            break;
        case RESOLUTION_16_BIT:
            Timer.setTimerBit((TcCount16*) TC5);
            break;
        case RESOLUTION_8_BIT:
        default:
            Timer.setTimerBit((TcCount8*) TC5);
            break;
    };
}


#endif