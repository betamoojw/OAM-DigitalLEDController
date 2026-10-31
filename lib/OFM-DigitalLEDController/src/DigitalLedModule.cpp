#include "DigitalLedModule.h"
#undef memcpy_P
#include <ArduinoJson.h>
#include <charconv>
#include <cstring>

DigitalLedModule digitalLedModule;

namespace
{
    template <typename Document>
    void sendJson(Document& document)
    {
        serializeJson(document, DIGITAL_LED_SERIAL);
        DIGITAL_LED_SERIAL.print("\r\n");
    }

    bool validInteger(JsonVariantConst value, int minimum, int maximum)
    {
        return value.is<int>() && value.as<int>() >= minimum && value.as<int>() <= maximum;
    }

    bool validOptionalInteger(JsonObjectConst object, const char* key, int minimum, int maximum)
    {
        return !object.containsKey(key) || validInteger(object[key], minimum, maximum);
    }

    bool validOptionalBool(JsonObjectConst object, const char* key)
    {
        return !object.containsKey(key) || object[key].is<bool>();
    }
}

const std::string DigitalLedModule::name()
{
    return "OFM Digital LED Controller";
}

const std::string DigitalLedModule::version()
{
    return "0.1.0";
}

void DigitalLedModule::setup(bool configured)
{
    (void)configured;
    firstRequest = true;
    DIGITAL_LED_SERIAL.setTX(DIGITAL_LED_UART_TX_PIN);
    DIGITAL_LED_SERIAL.setRX(DIGITAL_LED_UART_RX_PIN);
    DIGITAL_LED_SERIAL.setFIFOSize(512);
    DIGITAL_LED_SERIAL.begin(DIGITAL_LED_UART_BAUD_RATE);
    state.brightness = ParamAPP_DefaultBrightness;
    state.palette = ParamAPP_DefaultPalette;
    state.effect = ParamAPP_DefaultEffect;
    state.effectSpeed = ParamAPP_DefaultEffectSpeed;
    state.effectIntensity = ParamAPP_DefaultEffectIntensity;
    applyConfiguredDefaults();
    lastRequest = millis();
}

void DigitalLedModule::loop()
{
    receiveSerial();
    if (depth != 0)
        return;
    if (firstRequest)
    {
        requestState();
        firstRequest = false;
        lastRequest = millis();
        return;
    }
    const uint32_t intervalSeconds = ParamAPP_RequestXInterval;
    if (intervalSeconds == 0)
        return;
    if (millis() - lastRequest >= intervalSeconds * 1000UL)
    {
        requestState();
        lastRequest = millis();
    }
}

void DigitalLedModule::processInputKo(GroupObject& ko)
{
    switch (ko.asap())
    {
    case APP_KoChDimSw:
        sendSwitch(ko.value(DPT_Switch));
        break;
    case APP_KoChDimAbs:
        sendBrightness(ko.value(DPT_Percent_U8));
        break;
    case APP_KoChDimRel:
    {
        const uint8_t command = ko.value(Dpt(3, 7, 0));
        const uint8_t step = ko.value(Dpt(3, 7, 1));
        if (step == 0 || step > 7)
            break;
        const uint8_t delta = static_cast<uint8_t>((255U + (1U << (step - 1)) / 2U) / (1U << (step - 1)));
        const uint16_t next = (command != 0) ? state.brightness + delta : (state.brightness > delta ? state.brightness - delta : 0);
        sendBrightness(static_cast<uint8_t>(next > 255 ? 255 : next));
        break;
    }
    case APP_KoChRed:
        sendColor(ko.value(DPT_Percent_U8), state.green, state.blue);
        break;
    case APP_KoChGreen:
        sendColor(state.red, ko.value(DPT_Percent_U8), state.blue);
        break;
    case APP_KoChBlue:
        sendColor(state.red, state.green, ko.value(DPT_Percent_U8));
        break;
    case APP_KoChRGB:
    {
        const uint32_t color = ko.value(DPT_Colour_RGB);
        sendColor(static_cast<uint8_t>(color >> 16), static_cast<uint8_t>(color >> 8), static_cast<uint8_t>(color));
        break;
    }
    case APP_KoChNightlightSw:
        sendNightlight(ko.value(DPT_Switch));
        break;
    case APP_KoChScene:
    {
        const uint8_t scene = ko.value(DPT_SceneNumber);
        if (scene <= 63)
            sendPreset(static_cast<uint8_t>(scene + 1));
        break;
    }
    case APP_KoChPreset:
    {
        const uint8_t preset = ko.value(DPT_Percent_U8);
        if (preset >= 1 && preset <= 250)
            sendPreset(preset);
        break;
    }
    case APP_KoChPalette:
        sendSegmentValue("pal", ko.value(DPT_Percent_U8));
        break;
    case APP_KoChEffectId:
        sendSegmentValue("fx", ko.value(DPT_Percent_U8));
        break;
    case APP_KoChEffectSpeed:
        sendSegmentValue("sx", ko.value(DPT_Percent_U8));
        break;
    case APP_KoChEffectIntensity:
        sendSegmentValue("ix", ko.value(DPT_Percent_U8));
        break;
    default:
        break;
    }
}

bool DigitalLedModule::processCommand(const std::string command, bool diagnoseKo)
{
    (void)diagnoseKo;
    if (command.rfind("wled ", 0) != 0)
        return false;

    const std::string argument = command.substr(5);
    if (argument == "on" || argument == "off")
    {
        sendSwitch(argument == "on");
        return true;
    }
    if (argument == "nl on" || argument == "nl off")
    {
        sendNightlight(argument == "nl on");
        return true;
    }
    if (argument.rfind("bri ", 0) == 0 || argument.rfind("ps ", 0) == 0)
    {
        const bool preset = argument[0] == 'p';
        const char* first = argument.data() + 4;
        const char* last = argument.data() + argument.size();
        int value = 0;
        const auto result = std::from_chars(first, last, value);
        const int minimum = preset ? 1 : 0;
        const int maximum = preset ? 250 : 255;
        if (result.ec != std::errc{} || result.ptr != last || value < minimum || value > maximum)
            return true;
        if (preset)
            sendPreset(static_cast<uint8_t>(value));
        else
            sendBrightness(static_cast<uint8_t>(value));
        return true;
    }
    return false;
}

void DigitalLedModule::showHelp()
{
    openknx.console.printHelpLine("wled on|off", "Switch the LED controller");
    openknx.console.printHelpLine("wled bri <0..255>", "Set brightness");
    openknx.console.printHelpLine("wled ps <1..250>", "Select WLED preset");
    openknx.console.printHelpLine("wled nl on|off", "Set the three-minute nightlight");
}

void DigitalLedModule::showInformations()
{
    logInfoP("=== OFM Digital LED Controller ===");
    logInfoP("UART: %i baud, poll interval: %i", DIGITAL_LED_UART_BAUD_RATE, ParamAPP_RequestXInterval);
}

void DigitalLedModule::requestState()
{
    StaticJsonDocument<JSON_OBJECT_SIZE(1)> document;
    document["v"] = true;
    sendJson(document);
}

void DigitalLedModule::applyConfiguredDefaults()
{
    StaticJsonDocument<JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1)> document;
    document["on"] = state.brightness != 0;
    document["bri"] = state.brightness;
    document["seg"][0]["pal"] = state.palette;
    document["seg"][0]["fx"] = state.effect;
    document["seg"][0]["sx"] = state.effectSpeed;
    document["seg"][0]["ix"] = state.effectIntensity;
    sendJson(document);
    state.on = state.brightness != 0;
    KoAPP_ChDimState.valueCompare(state.on, DPT_Switch);
    KoAPP_ChDimValue.valueCompare(state.brightness, DPT_Percent_U8);
    publishEffects();
}

void DigitalLedModule::sendSwitch(bool on)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(1)> document;
    document["on"] = on;
    sendJson(document);
    state.on = on;
    KoAPP_ChDimState.valueCompare(on, DPT_Switch);
}

void DigitalLedModule::sendBrightness(uint8_t brightness)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(1)> document;
    document["bri"] = brightness;
    sendJson(document);
    state.brightness = brightness;
    state.on = brightness != 0;
    KoAPP_ChDimValue.valueCompare(brightness, DPT_Percent_U8);
    KoAPP_ChDimState.valueCompare(state.on, DPT_Switch);
}

void DigitalLedModule::sendColor(uint8_t red, uint8_t green, uint8_t blue)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(3)> document;
    document["on"] = red != 0 || green != 0 || blue != 0;
    JsonArray color = document["seg"][0]["col"].createNestedArray();
    color.add(red);
    color.add(green);
    color.add(blue);
    sendJson(document);
    state.red = red;
    state.green = green;
    state.blue = blue;
    state.on = document["on"];
    publishColor();
    KoAPP_ChDimState.valueCompare(state.on, DPT_Switch);
}

void DigitalLedModule::sendNightlight(bool enabled)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(1) + JSON_OBJECT_SIZE(4)> document;
    JsonObject nightlight = document["nl"].to<JsonObject>();
    nightlight["on"] = enabled;
    nightlight["dur"] = ParamAPP_NightlightDuration;
    nightlight["mode"] = ParamAPP_NightlightMode;
    nightlight["tbri"] = ParamAPP_NightlightTargetBrightness;
    sendJson(document);
    state.nightlight = enabled;
    KoAPP_ChNightlightState.valueCompare(enabled, DPT_Switch);
}

void DigitalLedModule::sendPreset(uint8_t preset)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(1)> document;
    document["ps"] = preset;
    sendJson(document);
    state.preset = preset;
    publishPreset();
    if (preset <= 64)
        KoAPP_ChSceneState.valueCompare(static_cast<uint8_t>(preset - 1), DPT_SceneNumber);
}

void DigitalLedModule::sendSegmentValue(const char* key, uint8_t value)
{
    StaticJsonDocument<JSON_OBJECT_SIZE(2) + JSON_ARRAY_SIZE(1)> document;
    document["on"] = true;
    document["seg"][0][key] = value;
    sendJson(document);
    if (std::strcmp(key, "pal") == 0)
        state.palette = value;
    else if (std::strcmp(key, "fx") == 0)
        state.effect = value;
    else if (std::strcmp(key, "sx") == 0)
        state.effectSpeed = value;
    else if (std::strcmp(key, "ix") == 0)
        state.effectIntensity = value;
    state.on = true;
    KoAPP_ChDimState.valueCompare(state.on, DPT_Switch);
    publishEffects();
}

void DigitalLedModule::receiveSerial()
{
    if (DIGITAL_LED_SERIAL.overflow())
    {
        resetFrame();
        resyncAfterOverrun = true;
    }
    const uint32_t started = micros();
    for (size_t count = 0; count < 256 && micros() - started < 2000 && DIGITAL_LED_SERIAL.available() > 0; ++count)
    {
        const int input = DIGITAL_LED_SERIAL.read();
        if (input < 0)
            break;
        const char character = static_cast<char>(input);
        if (character == '\r' || character == '\n')
        {
            if (depth != 0 || discardingFrame || resyncAfterOverrun)
                resetFrame();
            continue;
        }
        if (discardingFrame || resyncAfterOverrun)
            continue;
        if (depth == 0)
        {
            if (character != '{')
                continue;
            depth = 0;
            frameLength = 0;
            frameStarted = millis();
            inString = false;
            escaped = false;
        }
        if (frameLength + 1 >= sizeof(frame))
        {
            discardingFrame = true;
            continue;
        }
        frame[frameLength++] = character;
        if (inString)
        {
            if (escaped)
                escaped = false;
            else if (character == '\\')
                escaped = true;
            else if (character == '"')
                inString = false;
        }
        else if (character == '"')
            inString = true;
        else if (character == '{')
            ++depth;
        else if (character == '}' && --depth == 0)
        {
            processFrame();
            resetFrame();
            return;
        }
    }
    if (depth != 0 && millis() - frameStarted >= 1000)
        resetFrame();
}

void DigitalLedModule::processFrame()
{
    StaticJsonDocument<1024> document;
    if (deserializeJson(document, frame, frameLength))
        return;
    JsonObjectConst stateObject = document["state"];
    if (stateObject.isNull())
        return;
    if (!validOptionalBool(stateObject, "on") || !validOptionalInteger(stateObject, "bri", 0, 255) ||
        !validOptionalInteger(stateObject, "ps", -1, 250))
        return;

    JsonObjectConst nightlight = stateObject["nl"];
    if (stateObject.containsKey("nl") && nightlight.isNull())
        return;
    if (!validOptionalBool(nightlight, "on") || !validOptionalInteger(nightlight, "dur", 1, 255) ||
        !validOptionalInteger(nightlight, "mode", 0, 3) || !validOptionalInteger(nightlight, "tbri", 0, 255))
        return;

    JsonArrayConst segments = stateObject["seg"];
    if (stateObject.containsKey("seg") && segments.isNull())
        return;
    JsonObjectConst segment;
    if (!segments.isNull() && segments.size() > 0)
        segment = segments[0];
    if (!segment.isNull() && (!validOptionalInteger(segment, "fx", 0, 255) || !validOptionalInteger(segment, "sx", 0, 255) ||
                              !validOptionalInteger(segment, "ix", 0, 255) || !validOptionalInteger(segment, "pal", 0, 255)))
        return;

    JsonArrayConst color;
    if (!segment.isNull() && segment.containsKey("col"))
    {
        color = segment["col"][0];
        if (color.isNull() || (color.size() != 3 && color.size() != 4))
            return;
        for (JsonVariantConst component : color)
            if (!validInteger(component, 0, 255))
                return;
    }

    if (stateObject["on"].is<bool>())
    {
        state.on = stateObject["on"];
        KoAPP_ChDimState.valueCompare(state.on, DPT_Switch);
    }
    if (stateObject["bri"].is<uint8_t>())
    {
        state.brightness = stateObject["bri"];
        KoAPP_ChDimValue.valueCompare(state.brightness, DPT_Percent_U8);
    }
    if (stateObject["ps"].is<int>())
    {
        state.preset = stateObject["ps"];
        publishPreset();
    }
    if (!nightlight.isNull() && nightlight["on"].is<bool>())
    {
        state.nightlight = nightlight["on"];
        KoAPP_ChNightlightState.valueCompare(state.nightlight, DPT_Switch);
    }
    if (!segment.isNull())
    {
        if (segment["fx"].is<int>())
            state.effect = segment["fx"];
        if (segment["sx"].is<int>())
            state.effectSpeed = segment["sx"];
        if (segment["ix"].is<int>())
            state.effectIntensity = segment["ix"];
        if (segment["pal"].is<int>())
            state.palette = segment["pal"];
        publishEffects();
        if (!color.isNull())
        {
            state.red = color[0];
            state.green = color[1];
            state.blue = color[2];
            publishColor();
        }
    }
}

void DigitalLedModule::resetFrame()
{
    frameLength = 0;
    depth = 0;
    inString = false;
    escaped = false;
    discardingFrame = false;
    resyncAfterOverrun = false;
    frameStarted = 0;
}

void DigitalLedModule::publishColor()
{
    KoAPP_ChRedValue.valueCompare(state.red, DPT_Percent_U8);
    KoAPP_ChGreenValue.valueCompare(state.green, DPT_Percent_U8);
    KoAPP_ChBlueValue.valueCompare(state.blue, DPT_Percent_U8);
    KoAPP_ChRGBValue.valueCompare((static_cast<uint32_t>(state.red) << 16) | (static_cast<uint32_t>(state.green) << 8) | state.blue, DPT_Colour_RGB);
}

void DigitalLedModule::publishPreset()
{
    KoAPP_ChPresetValue.valueCompare(static_cast<uint8_t>(state.preset > 0 ? state.preset : 0), DPT_Percent_U8);
    if (state.preset >= 1 && state.preset <= 64)
        KoAPP_ChSceneState.valueCompare(static_cast<uint8_t>(state.preset - 1), DPT_SceneNumber);
}

void DigitalLedModule::publishEffects()
{
    KoAPP_ChPaletteValue.valueCompare(state.palette, DPT_Percent_U8);
    KoAPP_ChEffectIdValue.valueCompare(state.effect, DPT_Percent_U8);
    KoAPP_ChEffectSpeedValue.valueCompare(state.effectSpeed, DPT_Percent_U8);
    KoAPP_ChEffectIntensityValue.valueCompare(state.effectIntensity, DPT_Percent_U8);
}

