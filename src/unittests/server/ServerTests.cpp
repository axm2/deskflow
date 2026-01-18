/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2014 - 2016 Symless Ltd.
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "ServerTests.h"

#include "server/Server.h"
#include "deskflow/IKeyState.h"

void ServerTests::SwitchToScreenInfo_alloc_screen()
{
  auto actual = new Server::SwitchToScreenInfo("test");
  QCOMPARE(actual->m_screen, "test");
  delete actual;
}

void ServerTests::KeyboardBroadcastInfo_alloc_stateAndSceens()
{
  auto info = new Server::KeyboardBroadcastInfo(Server::KeyboardBroadcastInfo::State::kOn, "test");
  QCOMPARE(info->m_state, Server::KeyboardBroadcastInfo::State::kOn);
  QCOMPARE(info->m_screens, "test");
  delete info;
}

void ServerTests::activeScreenOnly_forwardsOriginalKeystroke()
{
  // Test that KeyInfo properly stores original keystroke information
  // for use when activeScreenOnly check fails
  
  std::set<std::string> targetScreens;
  targetScreens.insert("client1");
  targetScreens.insert("client2");
  
  // Original hotkey: Control+Backspace (key=0x08, mask=0x0002)
  KeyID originalKey = 0x08;
  KeyModifierMask originalMask = 0x0002;
  
  // Remapped key: Super+Backspace (key=0x08, mask=0x0040)
  KeyID remappedKey = 0x08;
  KeyModifierMask remappedMask = 0x0040;
  
  // Create KeyInfo with activeScreenOnly enabled
  auto *keyInfo = IKeyState::KeyInfo::alloc(
      remappedKey, remappedMask, 0, 0, targetScreens, true, originalKey, originalMask
  );
  
  // Verify all fields are properly set
  QVERIFY(keyInfo != nullptr);
  QCOMPARE(keyInfo->m_key, remappedKey);
  QCOMPARE(keyInfo->m_mask, remappedMask);
  QVERIFY(keyInfo->m_activeScreenOnly == true);
  QCOMPARE(keyInfo->m_originalKey, originalKey);
  QCOMPARE(keyInfo->m_originalMask, originalMask);
  
  // Verify target screens are stored correctly
  QVERIFY(IKeyState::KeyInfo::contains(keyInfo->m_screens, "client1"));
  QVERIFY(IKeyState::KeyInfo::contains(keyInfo->m_screens, "client2"));
  QVERIFY(!IKeyState::KeyInfo::contains(keyInfo->m_screens, "server"));
  
  free(keyInfo);
  
  // Test KeyInfo without activeScreenOnly (default behavior)
  auto *keyInfo2 = IKeyState::KeyInfo::alloc(
      remappedKey, remappedMask, 0, 0, targetScreens, false, 0, 0
  );
  
  QVERIFY(keyInfo2 != nullptr);
  QVERIFY(keyInfo2->m_activeScreenOnly == false);
  QCOMPARE(keyInfo2->m_originalKey, static_cast<KeyID>(0));
  QCOMPARE(keyInfo2->m_originalMask, static_cast<KeyModifierMask>(0));
  
  free(keyInfo2);
}

QTEST_MAIN(ServerTests)
