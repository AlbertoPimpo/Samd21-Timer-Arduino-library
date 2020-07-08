#include "Samd21Timer.h"
//Implementation of Samd21TimerClass Methods

#ifdef __SAMD21G18A__


void Samd21TimerClass::enable(TimerNumberSamd21 timer, float freq, void(*callback)(),  uint8_t priority, GeneralClockSamd21 gclk){
    if(freq >= 0.75)
        this->enable(timer, freq, callback, RESOLUTION_16_BIT, priority, gclk);
    else 
        this->enable(timer, freq, callback, RESOLUTION_32_BIT, priority, gclk);
}


void Samd21TimerClass::enable(TimerNumberSamd21 timer, float freq, void(*callback)(), TimerResolutionSamd21 res, uint8_t priority, GeneralClockSamd21 gclk){
    this->timerFlags[timer].res = res; 
    this->setCallback(timer, callback);
    setGeneralClock(timer, gclk);
    TimerParamsSamd21 params = this->getTimerParams(freq, res);

    //refactor to selection in array
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
        
        this->setTimerRegisters(timer, TC, &params, priority, gclk);

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

        this->setTimerRegisters(timer, TC, &params, priority, gclk);
    
    }

    else if (res == RESOLUTION_8_BIT){
        //TO DO 

    }

    this->timerFlags[timer].isActive = true;
}


TimerParamsSamd21 Samd21TimerClass::getTimerParams(float freq, TimerResolutionSamd21 res){
    TimerParamsSamd21 params;
    switch (res){
        case RESOLUTION_16_BIT:
            params.modeCount = TC_CTRLA_MODE_COUNT16;

            if ((freq < 24000000) && (freq > 800)) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV1;
                params.compare = SAMD21_CLK/freq;
            } else if (freq > 400) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV2;
                params.compare = (SAMD21_CLK/2)/freq;
            } else if (freq > 200) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV4;
                params.compare = (SAMD21_CLK/4)/freq;
            } else if (freq > 100) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV8;
                params.compare = (SAMD21_CLK/8)/freq;
            } else if (freq > 50) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV16;
                params.compare = (SAMD21_CLK/16)/freq;
            } else if (freq > 12) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV64;
                params.compare = (SAMD21_CLK/64)/freq;
            } else if (freq > 3) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV256;
                params.compare = (SAMD21_CLK/256)/freq;
            } else if (freq > 0.75){
                params.prescaler = TC_CTRLA_PRESCALER_DIV1024;
                params.compare = (SAMD21_CLK/1024)/freq;
            } 

            break;
        
        case RESOLUTION_32_BIT:
            params.modeCount = TC_CTRLA_MODE_COUNT16;

            if (freq > 0.02) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV1;
                params.compare = SAMD21_CLK/freq;
            } else if (freq > 0.006) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV2;
                params.compare = (SAMD21_CLK/2)/freq;
            } else if (freq > 0.003) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV4;
                params.compare = (SAMD21_CLK/4)/freq;
            } else if (freq > 0.0015) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV8;
                params.compare = (SAMD21_CLK/8)/freq;
            } else if (freq > 0.0008) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV16;
                params.compare = (SAMD21_CLK/16)/freq;
            } else if (freq > 0.0002) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV64;
                params.compare = (SAMD21_CLK/64)/freq;
            } else if (freq > 0.00005) {
                params.prescaler = TC_CTRLA_PRESCALER_DIV256;
                params.compare = (SAMD21_CLK/256)/freq;
            } else if (freq >= 0.000011){
                params.prescaler = TC_CTRLA_PRESCALER_DIV1024;
                params.compare = (SAMD21_CLK/1024)/freq;
            } 

            break;

        case RESOLUTION_8_BIT:
        default:
            //TO DO
            break;
        
    };

    return params;
}


void Samd21TimerClass::setGeneralClock(TimerNumberSamd21 timer, GeneralClockSamd21 gclk){  
    unsigned long clockGenCtrlId;
    unsigned long clockGenCtrlGen;
    
    switch (timer){
        case TIMER_0:
        case TIMER_1:
            clockGenCtrlId = GCLK_CLKCTRL_ID_TCC0_TCC1;
            break;
        case TIMER_2:
        case TIMER_3:
            clockGenCtrlId = GCLK_CLKCTRL_ID_TCC2_TC3;
            break;
        case TIMER_4:
        case TIMER_5:
        default:
            clockGenCtrlId = GCLK_CLKCTRL_ID_TC4_TC5;
            break;
    };

    switch(gclk){
        case GCLK_0:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK0;
            break;
        case GCLK_1:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK1;
            break;
        case GCLK_2:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK2;
            break;
        case GCLK_3:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK3;
            break;
        case GCLK_4:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK4;
            break;
        case GCLK_5:
        default:
            clockGenCtrlGen = GCLK_CLKCTRL_GEN_GCLK5;
            break;
    };


    GCLK->GENDIV.reg = GCLK_GENDIV_DIV(1) |
                        GCLK_GENDIV_ID(gclk);
    while (GCLK->STATUS.bit.SYNCBUSY);

    GCLK->GENCTRL.reg = GCLK_GENCTRL_IDC |
                        GCLK_GENCTRL_GENEN |
                        GCLK_GENCTRL_SRC_DFLL48M |
                        GCLK_GENCTRL_ID(gclk);
    while (GCLK->STATUS.bit.SYNCBUSY);

    GCLK->CLKCTRL.reg = GCLK_CLKCTRL_CLKEN |
                        clockGenCtrlGen |
                        clockGenCtrlId;
    while (GCLK->STATUS.bit.SYNCBUSY);

}


template <class TimerRegisters> void Samd21TimerClass::setTimerRegisters(TimerNumberSamd21 timer, TimerRegisters TC, TimerParamsSamd21* params, uint8_t priority, GeneralClockSamd21 gclk){
    this->reset(TC);

    TC->CTRLA.reg |= params->modeCount;
    TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;
    TC->CTRLA.reg |= params->prescaler;
    //TC->CTRLA.reg |= TC_CTRLA_ENABLE;



    TC->CC[0].reg = params->compare;
    
    while (this->isSyncing(TC));

    this->setNVIC(timer, priority);

    TC->INTENSET.bit.MC0 = 1;
    while (this->isSyncing(TC));

    TC->CTRLA.reg |= TC_CTRLA_ENABLE;
    while (this->isSyncing(TC));

}


template <class TimerRegisters> void Samd21TimerClass::reset(TimerRegisters TC){
    TC->CTRLA.reg = TC_CTRLA_SWRST;
    while (this->isSyncing(TC));
    while (TC->CTRLA.bit.SWRST);
}


template <class TimerRegisters> bool Samd21TimerClass::isSyncing(TimerRegisters TC){
    return TC->STATUS.bit.SYNCBUSY == 1;
}


void Samd21TimerClass::setNVIC(TimerNumberSamd21 timer, uint8_t priority){
    //refactor as array
    IRQn_Type irqn;
    switch(timer){
        case TIMER_0:
            irqn = TCC0_IRQn;
            break;
        case TIMER_1:
            irqn = TCC1_IRQn;
            break;
        case TIMER_2:
            irqn = TCC2_IRQn;
            break;
        case TIMER_3:
            irqn = TC3_IRQn;
            break;
        case TIMER_4:
            irqn = TC4_IRQn;
            break;
        case TIMER_5:
        default:
            irqn = TC5_IRQn;
            break;
    };

    NVIC_DisableIRQ(irqn);
    NVIC_ClearPendingIRQ(irqn);
    NVIC_SetPriority(irqn, priority);
    NVIC_EnableIRQ(irqn);
}


template <class TimerRegisters> void Samd21TimerClass::setTimerBit(TimerRegisters TC){
    TC->INTFLAG.bit.MC0 = 1;
}



void Samd21TimerClass::setCallback(TimerNumberSamd21 timer, void(*callback)()){

    //refactor as array of callbacks
    switch (timer) {
        case TIMER_0:
            this->callbacks.timer_0_routine = callback;
            break;
        case TIMER_1:
            this->callbacks.timer_1_routine = callback;
            break;
        case TIMER_2:
            this->callbacks.timer_2_routine = callback;
            break;
        case TIMER_3:
            this->callbacks.timer_3_routine = callback;
            break;
        case TIMER_4:
            this->callbacks.timer_4_routine = callback;
            break;
        case TIMER_5:
        default:
            this->callbacks.timer_5_routine = callback; 
            break;
        
    }
}

void Samd21TimerClass::disable(TimerNumberSamd21 timer){
    TcCount16* TC = (TcCount16*) this->timerLookUp[timer]; //casted to 32 bit because it's the same for each resolution
    TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
    while (this->isSyncing(TC));
}



//refactor, define if timer exist
//refactor, define a parametric handler what will be called by each handler

void TCC0_Handler(){
    Timer.callbacks.timer_0_routine();
    //enable interrupt again
    switch(Timer.timerFlags[TIMER_0].res){
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
    switch(Timer.timerFlags[TIMER_1].res){
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
    switch(Timer.timerFlags[TIMER_2].res){
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
    switch(Timer.timerFlags[TIMER_3].res){
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
    switch(Timer.timerFlags[TIMER_4].res){
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
    switch(Timer.timerFlags[TIMER_5].res){
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

Samd21TimerClass Timer;


#endif  /* __SAMD21G18A__ */