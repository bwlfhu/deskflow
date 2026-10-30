/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2012 - 2016 Synergy App Ltd
 * SPDX-FileCopyrightText: (C) 2004 Chris Schoeneman
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "platform/OSXClipboardAnyTextConverter.h"

#include <algorithm>

//
// OSXClipboardAnyTextConverter
//

IClipboard::Format OSXClipboardAnyTextConverter::getFormat() const
{
  return IClipboard::Format::Text;
}

std::string OSXClipboardAnyTextConverter::fromIClipboard(const std::string &data) const
{
  // convert linefeeds and then convert to desired encoding
  return doFromIClipboard(convertLinefeedToMacOS(data));
}

std::string OSXClipboardAnyTextConverter::toIClipboard(const std::string &data) const
{
  // convert text then newlines
  return convertLinefeedToUnix(doToIClipboard(data));
}

static bool isLF(char ch)
{
  return (ch == '\n');
}

std::string OSXClipboardAnyTextConverter::convertLinefeedToMacOS(const std::string &src)
{
  // note -- we assume src is a valid UTF-8 string
  std::string copy = convertLinefeedToUnix(src);

  std::replace_if(copy.begin(), copy.end(), isLF, '\r');

  return copy;
}

std::string OSXClipboardAnyTextConverter::convertLinefeedToUnix(const std::string &src)
{
  std::string copy;
  copy.reserve(src.size());

  // Consume CRLF together so it cannot turn into two LF characters.
  for (std::string::size_type i = 0; i < src.size(); ++i) {
    if (src[i] == '\r') {
      if (i + 1 < src.size() && src[i + 1] == '\n') {
        ++i;
      }
      copy += '\n';
    } else {
      copy += src[i];
    }
  }

  return copy;
}
