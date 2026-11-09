#ifndef _RIPPLE_H_
#define _RIPPLE_H_

#include <map>
#include <vector>
#include "pico/stdlib.h"
#include "animation.h"

/*
 * Ripple overlay.
 *
 * Not a base animation: it is drawn on top of whatever base + pressed
 * animation is active, so the normal press colour and fade ("afterglow")
 * keep working underneath it.
 *
 * Works on physical buttons (GPIO pins), not on mapped functions, so a
 * ripple always starts under the finger regardless of the active profile.
 */

struct RipplePoint {
  float x;
  float y;
};

class Ripple {
public:
  // pinPositions: physical position of each button, by GPIO pin.
  // maskToPin:    which pin each LED's button sits on, by the LED's button mask.
  void SetLayout(const std::map<uint8_t, RipplePoint> &pinPositions,
                 const std::map<uint32_t, uint8_t> &maskToPin);

  // Called every frame with the currently held pins (bit n = GPIO n).
  // Spawns a ripple for each pin that was not held on the previous frame.
  void UpdatePressed(uint32_t pressedPins, RGB color);

  // Adds the active ripples onto the frame.
  void Apply(RGB (&frame)[100], PixelMatrix &matrix);

private:
  struct Wave {
    RipplePoint origin;
    absolute_time_t start;
    RGB color;
  };

  static const size_t MAX_WAVES = 8;

  // Distances are in "button widths": neighbouring buttons are about 1.0 apart.
  static constexpr float SPEED_PER_MS = 0.016f;  // ring grows ~1 button every 60ms
  static constexpr float RING_WIDTH = 1.1f;      // how thick the ring is
  static constexpr float LIFETIME_MS = 650.0f;   // ring fades out completely by then

  std::map<uint8_t, RipplePoint> pinPositions;
  std::map<uint32_t, uint8_t> maskToPin;
  uint32_t heldPins = 0;
  std::vector<Wave> waves;
};

#endif
