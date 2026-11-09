#ifndef _ANIMATION_STATION_H_
#define _ANIMATION_STATION_H_

#include <algorithm>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include "hardware/clocks.h"

#include "NeoPico.h"
#include "animation.h"
#include "effects/chase.h"
#include "effects/customtheme.h"
#include "effects/customthemepressed.h"
#include "effects/rainbow.h"
#include "effects/ripple.h"
#include "effects/staticcolor.h"
#include "effects/statictheme.h"

#include "config.pb.h"
#include "enums.pb.h"

const int TOTAL_EFFECTS = 4; // Exclude custom theme until verified present

class AnimationStation
{
public:
    AnimationStation();
    void Animate();
    void HandleEvent(GamepadHotkey action);
    void Clear();
    void ChangeAnimation(int changeSize);
    void ApplyBrightness(uint32_t *frameValue);
    uint16_t AdjustIndex(int changeSize);
    void HandlePressed(std::vector<Pixel> pressed);
    void ClearPressed();
    void SetMode(uint8_t mode);
    void SetMatrix(PixelMatrix matrix);
    void SetRippleLayout(const std::map<uint8_t, RipplePoint> &pinPositions,
                         const std::map<uint32_t, uint8_t> &maskToPin);
    void HandlePressedPins(uint32_t pressedPins);

    // LED press styles, cycled with Select + Start + Home.
    // bit 0: ripple instead of afterglow, bit 1: LEDs off when not pressed.
    static const uint32_t PRESS_STYLE_COUNT = 4;
    static bool StyleUsesRipple();
    static bool StyleIsDark();
    void NextPressStyle();

    // The custom theme's normal colour for each button (by mask). Flashes,
    // afterglow and ripples all use these, so effects match the board.
    static std::map<uint32_t, RGB> ThemeColors();
    // Brightness the theme rests at in non-dark styles, so presses stand out.
    static constexpr float THEME_REST_LEVEL = 0.3f;
    void ConfigureBrightness(uint8_t max, uint8_t steps);
    float GetBrightnessX();
    float GetLinkageModeOfBrightnessX();
    uint8_t GetBrightness();
    void SetBrightness(uint8_t brightness);
    void DecreaseBrightness();
    void IncreaseBrightness();
    void DimBrightnessTo0();
    uint8_t GetBrightnessSteps(){ return this->brightnessSteps; };
    uint8_t GetCustomBrightnessStepsSize(){ return (brightnessMax / brightnessSteps); };
    RGB linkageFrame[100]; // copy baseAnimation frame exclude buttonAnimation frame

private:
    Animation* baseAnimation;
    Animation* buttonAnimation;
    std::vector<Pixel> lastPressed;
    absolute_time_t nextChange;
    uint8_t effectCount;
    RGB frame[100];
    bool ambientLightEffectsChangeFlag = false; 
    bool ambientLightOnOffFlag = false;
    bool ambientLightLinkageOnOffFlag = false;
    bool aleLedsBrightnessCustomXupFlag = false;
    bool aleLedsBrightnessCustomXDownFlag = false;
    bool aleLedsParameterCustomUpFlag = false;
    bool aleLedsParameterCustomDownFlag = false;
    bool alGradientChaseBreathSpeedUpFlag = false;
    bool alGradientChaseBreathSpeedDownFlag = false;
    bool alCustomLinkageModeFlag = false;
    uint8_t getBrightnessStepSize() { return (brightnessMax / brightnessSteps); }
    uint8_t getLinkageModeOfBrightnessStepSize() { return (255 / brightnessSteps); }
    float linkageModeOfBrightnessX;
    uint8_t brightnessMax;
    uint8_t brightnessSteps;
    float brightnessX;
    PixelMatrix matrix;
    Ripple ripple;
    RGB RippleColor();
};

#endif
