#include "ripple.h"

#include <math.h>

void Ripple::SetCoordinates(const std::map<uint32_t, RipplePoint> &coordinates) {
  this->coordinates = coordinates;
  this->waves.clear();
  this->held.clear();
}

void Ripple::UpdatePressed(const std::vector<Pixel> &pressed, RGB color) {
  std::set<uint32_t> nowHeld;

  for (const Pixel &pixel : pressed) {
    nowHeld.insert(pixel.mask);

    // Only a fresh press starts a ripple, not holding the button down.
    if (held.count(pixel.mask))
      continue;

    auto position = coordinates.find(pixel.mask);
    if (position == coordinates.end())
      continue;

    if (waves.size() >= MAX_WAVES)
      waves.erase(waves.begin()); // drop the oldest

    waves.push_back({ position->second, get_absolute_time(), color });
  }

  held = nowHeld;
}

static inline uint8_t addChannel(uint8_t base, float add) {
  float total = base + add;
  return total > 255.0f ? 255 : static_cast<uint8_t>(total);
}

void Ripple::Apply(RGB (&frame)[100], PixelMatrix &matrix) {
  if (waves.empty())
    return;

  absolute_time_t now = get_absolute_time();

  // Forget waves that have faded out.
  for (auto it = waves.begin(); it != waves.end();) {
    float age = absolute_time_diff_us(it->start, now) / 1000.0f;
    if (age >= LIFETIME_MS)
      it = waves.erase(it);
    else
      ++it;
  }

  for (auto &column : matrix.pixels) {
    for (auto &pixel : column) {
      if (pixel.index == NO_PIXEL.index || pixel.positions.empty())
        continue;

      auto position = coordinates.find(pixel.mask);
      if (position == coordinates.end())
        continue;

      float addR = 0, addG = 0, addB = 0;

      for (const Wave &wave : waves) {
        float age = absolute_time_diff_us(wave.start, now) / 1000.0f;
        float radius = age * SPEED_PER_MS;

        float dx = position->second.x - wave.origin.x;
        float dy = position->second.y - wave.origin.y;
        float distance = sqrtf(dx * dx + dy * dy);

        // Bright where the ring is passing, dark elsewhere.
        float ring = 1.0f - fabsf(distance - radius) / RING_WIDTH;
        if (ring <= 0.0f)
          continue;

        // Smooth the ring edge, and fade the whole wave as it gets older.
        ring = ring * ring * (3.0f - 2.0f * ring);
        float strength = ring * (1.0f - age / LIFETIME_MS);

        addR += wave.color.r * strength;
        addG += wave.color.g * strength;
        addB += wave.color.b * strength;
      }

      if (addR == 0 && addG == 0 && addB == 0)
        continue;

      for (uint8_t led : pixel.positions) {
        frame[led].r = addChannel(frame[led].r, addR);
        frame[led].g = addChannel(frame[led].g, addG);
        frame[led].b = addChannel(frame[led].b, addB);
      }
    }
  }
}
