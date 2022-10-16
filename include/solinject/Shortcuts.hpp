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

/** @file
 * @brief This file provides handy shortcuts for reading
 * configuration from string or file
*/

#pragma once
#include <iostream>
#include <array>
#include "Configuration.hpp"
#include "ConfigurationParser.hpp"

namespace sol::di
{
    static std::string operator>>(std::string input, Configuration& config)
    {
        config = ConfigurationParser().Parse(input);
        return input;
    }

    static std::istream& operator>>(std::istream& stream, Configuration& config)
    {
        std::array<char, 4096> buffer;
        std::string input;

        while(!stream.eof())
        {
            stream.read(buffer.data(), buffer.size());
            input.append(buffer.data(), stream.gcount());
        }

        input >> config;
        return stream;
    }
}
