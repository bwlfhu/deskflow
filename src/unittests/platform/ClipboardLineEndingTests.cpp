/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2026 Deskflow Developers
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include <QTest>
#include <cstring>

#if defined(Q_OS_WIN)
#include "platform/MSWindowsClipboardUTF16Converter.h"
#elif defined(Q_OS_MACOS)
#include "platform/OSXClipboardUTF8Converter.h"
#endif

class ClipboardLineEndingTests : public QObject
{
  Q_OBJECT

private Q_SLOTS:
  void normalizesInput_data();
  void normalizesInput();
  void repeatedRoundTrips();
};

void ClipboardLineEndingTests::normalizesInput_data()
{
  QTest::addColumn<QByteArray>("input");
  QTest::addColumn<QByteArray>("canonical");

  QTest::newRow("empty") << QByteArray() << QByteArray();
  QTest::newRow("single-line") << QByteArray("AAA") << QByteArray("AAA");
  QTest::newRow("lf") << QByteArray("AAA\nBBB\nCCC") << QByteArray("AAA\nBBB\nCCC");
  QTest::newRow("crlf") << QByteArray("AAA\r\nBBB\r\nCCC") << QByteArray("AAA\nBBB\nCCC");
  QTest::newRow("cr") << QByteArray("AAA\rBBB\rCCC") << QByteArray("AAA\nBBB\nCCC");
  QTest::newRow("mixed") << QByteArray("AAA\r\nBBB\rCCC\nDDD") << QByteArray("AAA\nBBB\nCCC\nDDD");
  QTest::newRow("intentional-blank-lines") << QByteArray("AAA\r\n\r\nBBB\n\nCCC")
                                         << QByteArray("AAA\n\nBBB\n\nCCC");
  QTest::newRow("trailing-crlf") << QByteArray("AAA\r\n") << QByteArray("AAA\n");
  QTest::newRow("trailing-cr") << QByteArray("AAA\r") << QByteArray("AAA\n");
  QTest::newRow("cr-before-crlf") << QByteArray("AAA\r\r\nBBB") << QByteArray("AAA\n\nBBB");
  QTest::newRow("unicode") << QByteArray::fromHex("e4b8ade696870d0af09f9880")
                           << QByteArray::fromHex("e4b8ade696870af09f9880");
}

void ClipboardLineEndingTests::normalizesInput()
{
  QFETCH(QByteArray, input);
  QFETCH(QByteArray, canonical);

#if defined(Q_OS_WIN)
  MSWindowsClipboardUTF16Converter converter;
  const auto handle = converter.fromIClipboard(input.toStdString());
  QVERIFY(handle != nullptr);
  const auto *native = static_cast<const wchar_t *>(GlobalLock(handle));
  QVERIFY(native != nullptr);
  const auto actual = QString::fromWCharArray(native);
  GlobalUnlock(handle);
  const auto internal = converter.toIClipboard(handle);
  GlobalFree(handle);
  auto expected = canonical;
  expected.replace("\n", "\r\n");
  QCOMPARE(actual, QString::fromUtf8(expected));
  QCOMPARE(internal, canonical.toStdString());

  // Exercise native input independently of the converter's own output.
  const auto input16 = QString::fromUtf8(input);
  const auto bytes = (input16.size() + 1) * sizeof(wchar_t);
  const auto rawHandle = GlobalAlloc(GMEM_MOVEABLE, bytes);
  QVERIFY(rawHandle != nullptr);
  auto *raw = static_cast<wchar_t *>(GlobalLock(rawHandle));
  QVERIFY(raw != nullptr);
  memcpy(raw, input16.utf16(), bytes);
  GlobalUnlock(rawHandle);
  const auto decoded = converter.toIClipboard(rawHandle);
  GlobalFree(rawHandle);
  QCOMPARE(decoded, canonical.toStdString());
#elif defined(Q_OS_MACOS)
  OSXClipboardUTF8Converter converter;
  auto expected = canonical;
  expected.replace('\n', '\r');
  QCOMPARE(converter.fromIClipboard(input.toStdString()), expected.toStdString());
  QCOMPARE(converter.toIClipboard(input.toStdString()), canonical.toStdString());
#endif
}

void ClipboardLineEndingTests::repeatedRoundTrips()
{
#if defined(Q_OS_WIN)
  MSWindowsClipboardUTF16Converter converter;
#elif defined(Q_OS_MACOS)
  OSXClipboardUTF8Converter converter;
#endif
  const std::string expected = "AAA\n\nBBB\nCCC\n";
  auto text = expected;
  for (int round = 0; round < 1000; ++round) {
    const auto native = converter.fromIClipboard(text);
#if defined(Q_OS_WIN)
    QVERIFY(native != nullptr);
#endif
    text = converter.toIClipboard(native);
#if defined(Q_OS_WIN)
    GlobalFree(native);
#endif
    QCOMPARE(text, expected);
  }
}

QTEST_APPLESS_MAIN(ClipboardLineEndingTests)
#include "ClipboardLineEndingTests.moc"
