#include "rfid-timer-emulator.h"

extern TIM_HandleTypeDef htim1;

RfidTimerEmulator::RfidTimerEmulator() {
}

RfidTimerEmulator::~RfidTimerEmulator() {
    std::map<LfrfidKeyType, EncoderGeneric*>::iterator it;

    for(it = encoders.begin(); it != encoders.end(); ++it) {
        delete it->second;
        encoders.erase(it);
    }
}

void RfidTimerEmulator::start(LfrfidKeyType type, const uint8_t* data, uint8_t data_size) {
    if(encoders.count(type)) {
        current_encoder = encoders.find(type)->second;

        if(lfrfid_key_get_type_data_count(type) == data_size) {
            current_encoder->init(data, data_size);

            api_hal_rfid_tim_emulate(125000);
            api_hal_rfid_pins_emulate();

            api_interrupt_add(timer_update_callback, InterruptTypeTimerUpdate, this);

            // TODO make api for interrupts priority
            for(size_t i = WWDG_IRQn; i <= DMAMUX1_OVR_IRQn; i++) {
                HAL_NVIC_SetPriority(static_cast<IRQn_Type>(i), 15, 0);
            }

            HAL_NVIC_SetPriority(TIM1_UP_TIM16_IRQn, 5, 0);
            HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

            api_hal_rfid_tim_emulate_start();
        }
    } else {
        // not found
    }
}

void RfidTimerEmulator::stop() {
    api_hal_rfid_tim_emulate_stop();
    api_interrupt_remove(timer_update_callback, InterruptTypeTimerUpdate);

    api_hal_rfid_tim_reset();
    api_hal_rfid_pins_reset();
}

void RfidTimerEmulator::timer_update_callback(void* _hw, void* ctx) {
    RfidTimerEmulator* _this = static_cast<RfidTimerEmulator*>(ctx);
    TIM_HandleTypeDef* hw = static_cast<TIM_HandleTypeDef*>(_hw);

    if(api_hal_rfid_is_tim_emulate(hw)) {
        bool result;
        bool polarity;
        uint16_t period;
        uint16_t pulse;

        do {
            _this->current_encoder->get_next(&polarity, &period, &pulse);
            result = _this->pulse_joiner.push_pulse(polarity, period, pulse);
        } while(result == false);

        _this->pulse_joiner.pop_pulse(&period, &pulse);

        api_hal_rfid_set_emulate_period(period - 1);
        api_hal_rfid_set_emulate_pulse(pulse);
    }
}
