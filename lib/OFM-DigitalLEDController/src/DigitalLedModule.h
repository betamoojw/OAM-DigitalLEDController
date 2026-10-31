#pragma once

#include "OpenKNX.h"
#include "hardware.h"
#include "knxprod.h"
#include <Arduino.h>
#include <stdint.h>

class DigitalLedModule : public OpenKNX::Module
{
  public:
    void setup(bool configured) override;
    void loop() override;
    void processInputKo(GroupObject& ko) override;
    const std::string name() override;
    const std::string version() override;
    bool processCommand(const std::string command, bool diagnoseKo) override;
    void showHelp() override;
    void showInformations() override;

  private:
    struct State
    {
        bool on = false;
        uint8_t brightness = 0;
        uint8_t red = 0;
        uint8_t green = 0;
        uint8_t blue = 0;
        bool nightlight = false;
        uint8_t palette = 0;
        uint8_t effect = 0;
        uint8_t effectSpeed = 0;
        uint8_t effectIntensity = 0;
        int preset = -1;
    } state;

    char frame[1024]{};
    size_t frameLength = 0;
    uint8_t depth = 0;
    bool inString = false;
    bool escaped = false;
    bool discardingFrame = false;
    bool resyncAfterOverrun = false;
    bool firstRequest = true;
    uint32_t frameStarted = 0;
    uint32_t lastRequest = 0;

    void requestState();
    void applyConfiguredDefaults();
    void sendSwitch(bool on);
    void sendBrightness(uint8_t brightness);
    void sendColor(uint8_t red, uint8_t green, uint8_t blue);
    void sendNightlight(bool enabled);
    void sendPreset(uint8_t preset);
    void sendSegmentValue(const char* key, uint8_t value);
    void receiveSerial();
    void processFrame();
    void resetFrame();
    void publishColor();
    void publishPreset();
    void publishEffects();
};

extern DigitalLedModule digitalLedModule;
