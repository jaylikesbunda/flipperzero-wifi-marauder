#pragma once
#include "lf-rfid-scene-generic.h"
#include "../helpers/key-info.h"

class LfrfidSceneEmulateHID : public LfrfidScene {
public:
    void on_enter(LfrfidApp* app) final;
    bool on_event(LfrfidApp* app, LfrfidEvent* event) final;
    void on_exit(LfrfidApp* app) final;

private:
    const uint8_t data[3] = {0xED, 0x87, 0x70};
};