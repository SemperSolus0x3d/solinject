// SPDX-License-Identifier: LGPL-3.0-or-later

/*
 * solinject - C++ Dependency Injection header-only library
 * Copyright (C) 2022  SemperSolus0x3d
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include <iostream>
#include <fstream>
#include <filesystem>
#include <vector>
#include <thread>
#include <assert.h>
#include <solinject.hpp>
#include <solinject-macros.hpp>

#include "TestClasses.hpp"

using namespace sol::di;

const std::string TestFilesFolderName = "test-files";

void EnsureTestFilesFolderExists();
void RunTests();

int main()
{
    try
    {
        EnsureTestFilesFolderExists();
        RunTests();
        return 0;
    }
    catch (const std::exception& ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return -1;
}

void EnsureTestFilesFolderExists()
{
    std::filesystem::create_directory(TestFilesFolderName);
}

void AssertConfigurationItem(
    const ConfigurationItem& item,
    std::string expectedInterfaceKey,
    std::string expectedImplementationKey,
    ServiceLifetime expectedLifetime
)
{
    assert(item.InterfaceKey() == expectedInterfaceKey);
    assert(item.ImplementationKey() == expectedImplementationKey);
    assert(item.Lifetime() == expectedLifetime);
}

void ItParsesConfigurationCorrectly()
{
    ConfigurationParser parser;

    std::string input = "\
        TestA TestA Singleton\n\
        \n\
        # Some comment\n\
        TestB Self Transient\n\
        \n\
        # Some\n\
        # multiline\n\
        # comment\n\
        ITestD TestD Shared\n\
        \n\
        \n\
        TestC {\n\
            TestC Singleton\n\
            TestC Scoped\n\
        }\n\
    ";

    const auto configuration = parser.Parse(input);
    const auto& items = configuration.ConfigurationItems();

    assert(items.size() == 5);

    AssertConfigurationItem(items[0], "TestA", "TestA", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[1], "TestB", "TestB", ServiceLifetime::Transient);
    AssertConfigurationItem(items[2], "ITestD", "TestD", ServiceLifetime::Shared);
    AssertConfigurationItem(items[3], "TestC", "TestC", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[4], "TestC", "TestC", ServiceLifetime::Scoped);
}

void ItParsesConfigurationFromFile()
{
    const std::string filename = TestFilesFolderName + "/ItParsesConfigurationFromFile-config.txt";

    std::string input = "\
        TestA TestA Singleton\n\
        \n\
        # Some comment\n\
        TestB Self Transient\n\
        \n\
        # Some\n\
        # multiline\n\
        # comment\n\
        ITestD TestD Shared\n\
        \n\
        \n\
        TestC {\n\
            TestC Singleton\n\
            TestC Scoped\n\
        }\n\
    ";

    std::ofstream(filename, std::ios::out).close();
    std::fstream file(filename, std::ios::in | std::ios::out | std::ios::binary);
    assert(file.is_open());

    file.write(input.data(), input.size());
    file.seekg(0);

    Configuration configuration;
    file >> configuration;

    file.close();
    std::filesystem::remove(filename);

    const auto& items = configuration.ConfigurationItems();

    assert(items.size() == 5);

    AssertConfigurationItem(items[0], "TestA", "TestA", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[1], "TestB", "TestB", ServiceLifetime::Transient);
    AssertConfigurationItem(items[2], "ITestD", "TestD", ServiceLifetime::Shared);
    AssertConfigurationItem(items[3], "TestC", "TestC", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[4], "TestC", "TestC", ServiceLifetime::Scoped);
}

void ItHandlesUnicodeCharacters()
{
    ConfigurationParser parser;

    std::string input = u8"\
        # Some com🔥ment\n\
        TestA TestA Singleton\n\
        Te👍stB Self Transient\n\
        # So🎉me\n\
        # multil🎉ine\n\
        # c🎉omment\n\
        🎉ITestD🎉 ♂TestD♂ Shared\n\
        TestC {\n\
            ♂T♂e♂s♂t♂C♂ Singleton\n\
            TestC Scoped\n\
        }\n\
    ";

    const auto configuration = parser.Parse(input);
    const auto& items = configuration.ConfigurationItems();

    assert(items.size() == 5);

    AssertConfigurationItem(items[0], u8"TestA", u8"TestA", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[1], u8"Te👍stB", u8"Te👍stB", ServiceLifetime::Transient);
    AssertConfigurationItem(items[2], u8"🎉ITestD🎉", u8"♂TestD♂", ServiceLifetime::Shared);
    AssertConfigurationItem(items[3], u8"TestC", u8"♂T♂e♂s♂t♂C♂", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[4], u8"TestC", u8"TestC", ServiceLifetime::Scoped);
}

void ItHandlesQuotesAndEscapedCharacters()
{
    ConfigurationParser parser;

    std::string input = "\
        Test\\ A \"Test A\" Singleton\n\
        # Some comment with \"quotes\"\n\
        TestB \"Self\" Transient\n\
        \\\"TestC\\\" TestC Scoped\n\
    ";

    const auto configuration = parser.Parse(input);
    const auto& items = configuration.ConfigurationItems();

    assert(items.size() == 3);

    AssertConfigurationItem(items[0], "Test A", "Test A", ServiceLifetime::Singleton);
    AssertConfigurationItem(items[1], "TestB", "Self", ServiceLifetime::Transient);
    AssertConfigurationItem(items[2], "\"TestC\"", "TestC", ServiceLifetime::Scoped);
}

void RunTests()
{
    ItParsesConfigurationCorrectly();
    ItParsesConfigurationFromFile();
    ItHandlesUnicodeCharacters();
    ItHandlesQuotesAndEscapedCharacters();
}
