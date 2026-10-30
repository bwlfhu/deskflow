/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "platform/MSWindowsMouseMotion.h"

#include <iostream>
#include <limits>

using deskflow::platform::isBogusMouseDelta;
using deskflow::platform::MouseBounds;

int main()
{
  int checks = 0;
  int failures = 0;
  const auto check = [&](bool result, const char *name) {
    ++checks;
    if (!result) {
      ++failures;
      std::cerr << "FAIL: " << name << '\n';
    }
  };
  const MouseBounds primary{0, 0, 1920, 1080};
  check(isBogusMouseDelta(1900, 0, 960, 540, primary), "reject stale move towards the second monitor");
  check(isBogusMouseDelta(-1900, 0, 960, 540, primary), "reject mirrored stale move");
  check(isBogusMouseDelta(0, 1000, 960, 540, primary), "reject vertical stale move");
  check(!isBogusMouseDelta(500, 100, 960, 540, primary), "preserve fast hardware motion");
  check(!isBogusMouseDelta(-500, -100, 960, 540, primary), "preserve fast mirrored hardware motion");
  check(!isBogusMouseDelta(950, 530, 960, 540, primary), "inclusive original ten-pixel boundary");
  check(!isBogusMouseDelta(-950, -530, 960, 540, primary), "inclusive mirrored boundary");
  check(isBogusMouseDelta(951, 0, 960, 540, primary), "reject beyond right boundary");
  check(isBogusMouseDelta(-951, 0, 960, 540, primary), "reject beyond left boundary");
  check(isBogusMouseDelta(0, 531, 960, 540, primary), "reject beyond bottom boundary");
  check(isBogusMouseDelta(0, -531, 960, 540, primary), "reject beyond top boundary");
  check(isBogusMouseDelta(0, 0, 1920, 540, primary), "invalid parking point is rejected");
  check(
      isBogusMouseDelta(std::numeric_limits<int32_t>::min(), 0, 960, 540, primary), "minimum delta does not overflow"
  );
  check(
      isBogusMouseDelta(std::numeric_limits<int32_t>::max(), 0, 960, 540, primary), "maximum delta does not overflow"
  );
  check(!isBogusMouseDelta(900, 0, 1920, 1080, {0, 0, 3840, 2160}), "large primary monitor has its own limits");

  // Translating a monitor, including above/left of the origin, must not
  // change which motion is accepted. Extra monitors never enter this policy.
  for (const auto origin :
       {MouseBounds{-1920, 0, 0, 1080}, MouseBounds{0, -1080, 1920, 0}, MouseBounds{1920, 1080, 3840, 2160}}) {
    for (int dx = -2000; dx <= 2000; dx += 17) {
      for (int dy = -1100; dy <= 1100; dy += 29) {
        check(
            isBogusMouseDelta(dx, dy, 960, 540, primary) ==
                isBogusMouseDelta(dx, dy, origin.left + 960, origin.top + 540, origin),
            "monitor translation invariance"
        );
      }
    }
  }
  check(primary.contains(0, 0), "capture rectangle includes its top-left pixel");
  check(!primary.contains(1920, 0), "capture rectangle excludes its right edge");
  check(!MouseBounds{1920, 0, 3840, 1080}.contains(960, 540), "clipped second monitor excludes the parking point");
  std::cout << checks << " checks, " << failures << " failures\n";
  return failures == 0 ? 0 : 1;
}
