#ifndef _RIPPLE_H_
#define _RIPPLE_H_

#include <map>
#include <set>
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
 * Every new button press spawns a ring that expands outward from that
 * button's physical position and lights each button it passes over.
 */

struct RipplePoint {
  float x;
  float y;
};

class Ripple {
public:
  // Physical position of each button, keyed by its gamepad mask.
  // Buttons without a position never start a ripple and are never lit by one.
  void SetCoordinates(const std::map<uint32_t, RipplePoint> &coordinates);

  // Called every frame with the buttons currently held. Spawns a ripple for
  // each button that was not held on the previous frame.
  void UpdatePressed(const std::vector<Pixel> &pressed, RGB color);

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

  std::map<uint32_t, RipplePoint> coordinates;
  std::set<uint32_t> held;
  std::vector<Wave> waves;
};

#endif
