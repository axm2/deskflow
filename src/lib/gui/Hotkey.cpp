/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2012 - 2016 Symless Ltd.
 * SPDX-FileCopyrightText: (C) 2008 Volker Lanz <vl@fidra.de>
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "Hotkey.h"

#include <QSettings>

QString Hotkey::text() const
{
  QString hotkeyText = m_keySequence.isMouseButton() ? kMousebutton.arg(m_keySequence.toString())
                                                     : kKeystroke.arg(m_keySequence.toString());

  // Add disableGlobalHotkeyRegister option if set (only for keystroke, not mousebutton)
  if (!m_keySequence.isMouseButton() && m_disableGlobalHotkeyRegister) {
    // Insert the option before the closing parenthesis
    hotkeyText.insert(hotkeyText.length() - 1, QStringLiteral(",disableGlobalHotkeyRegister"));
  }

  return hotkeyText;
}

void Hotkey::loadSettings(QSettings &settings)
{
  m_keySequence.loadSettings(settings);

  m_actions.clear();
  int num = settings.beginReadArray(kSectionActions);
  for (int i = 0; i < num; i++) {
    settings.setArrayIndex(i);
    Action a;
    a.loadSettings(settings);
    m_actions.append(a);
  }

  settings.endArray();

  m_disableGlobalHotkeyRegister = settings.value(kDisableGlobalHotkeyRegister, false).toBool();
}

void Hotkey::saveSettings(QSettings &settings) const
{
  m_keySequence.saveSettings(settings);

  settings.beginWriteArray(kSectionActions);
  for (int i = 0; i < m_actions.size(); i++) {
    settings.setArrayIndex(i);
    m_actions.at(i).saveSettings(settings);
  }
  settings.endArray();

  settings.setValue(kDisableGlobalHotkeyRegister, m_disableGlobalHotkeyRegister);
}

bool Hotkey::operator==(const Hotkey &hk) const
{
  return m_keySequence == hk.keySequence() && m_actions == hk.actions() &&
         m_disableGlobalHotkeyRegister == hk.disableGlobalHotkeyRegister();
}

QTextStream &operator<<(QTextStream &outStream, const Hotkey &hotkey)
{
  // Don't write a config if there is no actions
  if (hotkey.actions().size() == 0)
    return outStream;

  QString outText = QStringLiteral("\t%1 = ").arg(hotkey.text());
  for (int i = 0; i < hotkey.actions().size(); i++) {
    outText.append(hotkey.actions().at(i).text());
    if (i != hotkey.actions().size() - 1) {
      outText.append(QStringLiteral(", "));
    }
  }
  outText.append(QStringLiteral("\n"));

  outStream << outText;
  return outStream;
}
