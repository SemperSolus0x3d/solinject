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

/// @file

#pragma once
#include <string>

namespace sol::di::impl
{
    /// @brief @ref sol::di::impl::ConfigurationParserToken type
    enum class TokenType
    {
        Key, ///< Key
        OpeningCurlyBracket, ///< the '{' token
        ClosingCurlyBracket, ///< the '}' token
        Self, ///< the 'Self' keyword
        Singleton, ///< the 'Singleton' keyword
        Transient, ///< the 'Transient' keyword
        Shared, ///< the 'Shared' keyword
        Scoped, ///< the 'Scoped' keyword
        None ///< the 'None' keyword
    };

    /// @brief @ref ConfigurationParser token
    class ConfigurationParserToken
    {
    public:
        /// @brief Constructor
        /// @param type the token type
        /// @param content the token content
        ConfigurationParserToken(TokenType type, std::string content) :
            m_Type(type), m_Content(content)
        {
        }

        /// @brief Copy constructor
        ConfigurationParserToken(const ConfigurationParserToken& other) :
            m_Type(other.m_Type),
            m_Content(other.m_Content)
        {
        }

        /// @brief Move constructor
        ConfigurationParserToken(ConfigurationParserToken&& other) noexcept
        {
            swap(*this, other);
        }

        /// @brief Copy-assignment operator
        ConfigurationParserToken& operator=(ConfigurationParserToken other)
        {
            swap(*this, other);
            return *this;
        }

        /// @brief Swaps two @ref ConfigurationParserToken instances
        friend void swap(ConfigurationParserToken& a, ConfigurationParserToken& b)
        {
            using std::swap;

            swap(a.m_Type, b.m_Type);
            swap(a.m_Content, b.m_Content);
        }

        /// @brief Token type property
        /// @return the token type
        TokenType Type() const { return m_Type; }

        /// @brief Token content property
        /// @return the token content or an empty string
        /// if the token type is not `TokenType::Key`
        const std::string& Content() const { return m_Content; }
    private:
        /// @brief The token type
        TokenType m_Type;

        /// @brief The token content
        std::string m_Content;
    }; // class ConfigurationParserToken
} // sol::di::impl
