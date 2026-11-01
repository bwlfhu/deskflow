/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2012 - 2016 Synergy App Ltd
 * SPDX-FileCopyrightText: (C) 2011 Nick Bolton
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "OSXClipboardTests.h"

#include "platform/OSXClipboard.h"
#include "platform/OSXClipboardBMPConverter.h"
#include "platform/OSXClipboardPNGConverter.h"
#include "platform/OSXClipboardUTF8Converter.h"

#include <QImage>
#include <QtEndian>

void OSXClipboardTests::open()
{
  OSXClipboard clipboard;
  QVERIFY(clipboard.open(0));
  QVERIFY(clipboard.empty());
  clipboard.close();
}

void OSXClipboardTests::singleFormat()
{
  using enum IClipboard::Format;

  OSXClipboard clipboard;
  QVERIFY(clipboard.empty());
  clipboard.add(Text, m_testString);
  QVERIFY(clipboard.has(Text));
  QCOMPARE(clipboard.get(Text), m_testString);
}

void OSXClipboardTests::formatConvert_UTF8()
{
  OSXClipboardUTF8Converter converter;
  QCOMPARE(IClipboard::Format::Text, converter.getFormat());
  QCOMPARE(converter.getOSXFormat(), CFSTR("public.utf8-plain-text"));
  QCOMPARE(converter.fromIClipboard("test data\n"), "test data\r");
  QCOMPARE(converter.toIClipboard("test data\r"), "test data\n");
}

void OSXClipboardTests::pngBitmapConverterRoundTrip()
{
  OSXClipboardPNGConverter converter;
  QCOMPARE(converter.getFormat(), IClipboard::Format::Bitmap);
  QCOMPARE(converter.getOSXFormat(), CFSTR("public.png"));

  // A 2x1 bottom-up 32-bit BI_RGB DIB with two known RGB pixels.
  std::string dib(40 + 8, '\0');
  auto *header = reinterpret_cast<quint8 *>(dib.data());
  qToLittleEndian<quint32>(40, header);
  qToLittleEndian<qint32>(2, header + 4);
  qToLittleEndian<qint32>(1, header + 8);
  qToLittleEndian<quint16>(1, header + 12);
  qToLittleEndian<quint16>(32, header + 14);
  qToLittleEndian<quint32>(0, header + 16);
  auto *pixels = reinterpret_cast<quint8 *>(dib.data() + 40);
  // BGRA: red, then green.
  pixels[0] = 0;
  pixels[1] = 0;
  pixels[2] = 255;
  pixels[3] = 255;
  pixels[4] = 0;
  pixels[5] = 255;
  pixels[6] = 0;
  pixels[7] = 255;

  const auto png = converter.fromIClipboard(dib);
  QVERIFY(png.size() > 8);
  QCOMPARE(QByteArray(png.data(), 8), QByteArray::fromHex("89504e470d0a1a0a"));
  const auto image = QImage::fromData(QByteArray::fromRawData(png.data(), static_cast<int>(png.size())), "PNG");
  QCOMPARE(image.size(), QSize(2, 1));
  QCOMPARE(image.pixelColor(0, 0), QColor(255, 0, 0, 255));
  QCOMPARE(image.pixelColor(1, 0), QColor(0, 255, 0, 255));

  const auto roundTrip = converter.toIClipboard(png);
  QVERIFY(roundTrip.size() >= 40);
  OSXClipboardBMPConverter bmpConverter;
  const auto bmp = bmpConverter.fromIClipboard(roundTrip);
  const auto roundTripImage = QImage::fromData(
      QByteArray::fromRawData(bmp.data(), static_cast<int>(bmp.size())), "BMP"
  );
  QCOMPARE(roundTripImage.size(), QSize(2, 1));
  QCOMPARE(roundTripImage.pixelColor(0, 0), QColor(255, 0, 0, 255));
  QCOMPARE(roundTripImage.pixelColor(1, 0), QColor(0, 255, 0, 255));
}

QTEST_MAIN(OSXClipboardTests)
