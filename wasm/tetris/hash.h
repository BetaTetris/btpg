#pragma once

#include <cstdint>

constexpr uint64_t Hash(uint64_t a, uint64_t b) {
  constexpr uint64_t kTable[3] = {0x9e3779b185ebca87, 0xc2b2ae3d27d4eb4f, 0x165667b19e3779f9};
  auto Mix = [&](uint64_t a, uint64_t b) {
    a += b * kTable[1];
    a = (a << 31) | (a >> 33);
    return a * kTable[0];
  };
  uint64_t v1 = Mix(-kTable[0], a);
  uint64_t v2 = Mix(kTable[1], b);
  uint64_t ret = ((v1 << 18) | (v1 >> 46)) + ((v2 << 7) | (v2 >> 57));
  ret ^= ret >> 33;
  ret *= kTable[1];
  ret ^= ret >> 29;
  // ret *= kTable[2];
  // ret ^= ret >> 32;
  return ret;
}
