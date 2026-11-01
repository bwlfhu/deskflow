/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#pragma once

#include "platform/OSXClipboard.h"
#include "platform/OSXClipboardBMPConverter.h"

class OSXClipboardPNGConverter : public IOSXClipboardConverter
{
public:
  OSXClipboardPNGConverter() = default;
  ~OSXClipboardPNGConverter() override = default;

  IClipboard::Format getFormat() const override;
  CFStringRef getOSXFormat() const override;
  std::string fromIClipboard(const std::string &) const override;
  std::string toIClipboard(const std::string &) const override;

private:
  static std::string convertImage(const std::string &image, CFStringRef toType);
  OSXClipboardBMPConverter m_bmpConverter;
};
