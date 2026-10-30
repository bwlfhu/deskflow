/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include <cstdint>

namespace deskflow::platform {

struct MouseBounds
{
  int32_t left;
  int32_t top;
  int32_t right;
  int32_t bottom;

  bool contains(int32_t x, int32_t y) const
  {
    return x >= left && x < right && y >= top && y < bottom;
  }
};

inline bool isBogusMouseDelta(int32_t dx, int32_t dy, int32_t centerX, int32_t centerY, const MouseBounds &monitor)
{
  // A delayed pre-warp event can span almost a whole half-monitor. The
  // thresholds must describe the monitor containing the parking point, not
  // the virtual desktop, whose extra monitors would hide that jump.
  constexpr int64_t margin = 10;
  return !monitor.contains(centerX, centerY) || -int64_t{dx} + margin > int64_t{centerX} - monitor.left ||
         int64_t{dx} + margin > int64_t{monitor.right} - centerX ||
         -int64_t{dy} + margin > int64_t{centerY} - monitor.top ||
         int64_t{dy} + margin > int64_t{monitor.bottom} - centerY;
}

} // namespace deskflow::platform
