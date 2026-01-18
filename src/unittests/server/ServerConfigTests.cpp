/*
 * Deskflow -- mouse and keyboard sharing utility
 * SPDX-FileCopyrightText: (C) 2025 Chris Rizzitello <sithlord48@gmail.com>
 * SPDX-FileCopyrightText: (C) 2014 - 2016 Symless Ltd.
 * SPDX-License-Identifier: GPL-2.0-only WITH LicenseRef-OpenSSL-Exception
 */

#include "ServerConfigTests.h"

#include "server/Config.h"

class OnlySystemFilter : public InputFilter::Condition
{
public:
  Condition *clone() const override
  {
    return new OnlySystemFilter();
  }
  std::string format() const override
  {
    return "";
  }

  InputFilter::FilterStatus match(const Event &ev) override
  {
    return ev.getType() == EventTypes::System ? InputFilter::FilterStatus::Activate
                                              : InputFilter::FilterStatus::NoMatch;
  }
};

using namespace deskflow::server;

void ServerConfigTests::equalityCheck()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(a != b);

  QVERIFY(b.addScreen("screenB"));
  QVERIFY(a != b);

  QVERIFY(a.addScreen("screenB"));
  QVERIFY(a.addScreen("screenC"));
  QVERIFY(a.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(a.connect("screenB", Direction::Left, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(b.addScreen("screenC"));
  QVERIFY(b.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(b.connect("screenB", Direction::Left, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(a.addOption("screenA", kOptionClipboardSharing, 1));
  QVERIFY(b.addOption("screenA", kOptionClipboardSharing, 1));
  QVERIFY(a.addOption(std::string(), kOptionClipboardSharing, 1));
  QVERIFY(b.addOption(std::string(), kOptionClipboardSharing, 1));

  a.getInputFilter()->addFilterRule(InputFilter::Rule{new OnlySystemFilter()});
  b.getInputFilter()->addFilterRule(InputFilter::Rule{new OnlySystemFilter()});
  QVERIFY(a.addAlias("screenA", "aliasA"));
  QVERIFY(b.addAlias("screenA", "aliasA"));
  /* TODO Fix linking to the proper libs
  NetworkAddress addr1("localhost", 8080);
  addr1.resolve();
  NetworkAddress addr2("localhost", 8080);
  addr2.resolve();
  a.setDeskflowAddress(addr1);
  b.setDeskflowAddress(addr2);
  */
  QVERIFY(a == b);
}

void ServerConfigTests::equalityCheck_diff_options()
{
  Config a(nullptr);
  Config b(nullptr);

  QVERIFY(a.addScreen("screenA"));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(a.addOption("screenA", kOptionClipboardSharing, 0));
  QVERIFY(b.addOption("screenA", kOptionClipboardSharing, 1));
  QVERIFY(a != b);
}

void ServerConfigTests::equalityCheck_diff_alias()
{
  Config a(nullptr);
  Config b(nullptr);

  QVERIFY(a.addScreen("screenA"));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(b.addAlias("screenA", "aliasA"));
  QVERIFY(a != b);

  QVERIFY(a.addAlias("screenA", "aliasA"));
  QVERIFY(b.addAlias("screenA", "aliasB"));
  QVERIFY(a != b);
}

void ServerConfigTests::equalityCheck_diff_filters()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(b.addScreen("screenA"));

  a.getInputFilter()->addFilterRule(InputFilter::Rule{new OnlySystemFilter()});
  QVERIFY(a != b);
}

// TODO FIX
/*
void ServerConfigTests::equalityCheck_diff_address()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(b.addScreen("screenA"));
  a.setDeskflowAddress(NetworkAddress(8000));
  b.setDeskflowAddress(NetworkAddress(9000));
  QVERIFY(a != b);
}
*/

void ServerConfigTests::equalityCheck_diff_neighbours1()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(a.addScreen("screenB"));
  QVERIFY(a.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(b.addScreen("screenB"));
  QVERIFY(a != b);
  QVERIFY(b != a);
}

void ServerConfigTests::equalityCheck_diff_neighbours2()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(a.addScreen("screenB"));
  QVERIFY(a.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(b.addScreen("screenB"));
  QVERIFY(b.connect("screenA", Direction::Bottom, 0.0f, 0.25f, "screenB", 0.25f, 1.0f));
  QVERIFY(a != b);
}

void ServerConfigTests::equalityCheck_diff_neighbours3()
{
  Config a(nullptr);
  Config b(nullptr);
  QVERIFY(a.addScreen("screenA"));
  QVERIFY(a.addScreen("screenB"));
  QVERIFY(a.addScreen("screenC"));
  QVERIFY(a.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenB", 0.5f, 1.0f));
  QVERIFY(b.addScreen("screenA"));
  QVERIFY(b.addScreen("screenB"));
  QVERIFY(b.addScreen("screenC"));
  QVERIFY(b.connect("screenA", Direction::Bottom, 0.0f, 0.5f, "screenC", 0.5f, 1.0f));
  QVERIFY(a != b);
}

void ServerConfigTests::activeScreenOnly_multipleScreens()
{
  // Test that activeScreenOnly works with multiple screens using colon-separated list
  const char *configStr = R"(
section: screens
	Server:
	Mac-Mini:
	Macbook:
end

section: links
	Server:
		right = Mac-Mini
	Mac-Mini:
		left = Server
		right = Macbook
	Macbook:
		left = Mac-Mini
end

section: options
	# Test colon-separated screen list
	keystroke(Control+c,disableGlobalHotkeyRegister) = keystroke(Super+c,Mac-Mini:Macbook,activeScreenOnly)
	
	# Test chained actions
	keystroke(Control+v,disableGlobalHotkeyRegister) = keystroke(Super+v,Mac-Mini,activeScreenOnly),keystroke(Super+v,Macbook,activeScreenOnly)
end
)";

  std::istringstream configStream(configStr);
  Config config(nullptr);

  // Parse the config - this should succeed without throwing
  try {
    configStream >> config;
    // Verify screens were added
    QVERIFY(config.isScreen("Server"));
    QVERIFY(config.isScreen("Mac-Mini"));
    QVERIFY(config.isScreen("Macbook"));
  } catch (const std::exception &e) {
    QFAIL(e.what());
  }
}

QTEST_MAIN(ServerConfigTests)
