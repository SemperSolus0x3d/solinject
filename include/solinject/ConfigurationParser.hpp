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
#include <vector>
#include <string>
#include <array>
#include <stdexcept>
#include <cstring>
#include "Defines.hpp"
#include "Configuration.hpp"
#include "ConfigurationParserToken.hpp"

namespace sol::di
{
    /// @brief DI @ref Configuration parser
    class ConfigurationParser
    {
    public:
        /// @brief Parses configuration
        /// @param input the UTF8-encoded input string
        /// @return the parsed configuration
        Configuration Parse(const std::string& input)
        {
            m_Input = input;

            Configuration result;

            SkipBom();

            Token token(impl::TokenType::Key, "");

            while (GetNextToken(token))
            {
                auto configurationItems = ParseConfigurationItem(token);

                for (auto&& item : configurationItems)
                    result.AddConfigurationItem(std::move(item));
            }

            return result;
        }
    private:
        /// @copydoc impl::ConfigurationParserToken
        using Token = impl::ConfigurationParserToken;

        /// @brief Byte
        using byte = unsigned char;

        /// @brief The input string
        std::string m_Input;

        /// @brief The parser position in input
        size_t m_Pos = 0;

        /// @brief The current parsed lexem
        std::string m_Lexem;

        /// @brief The current parsed token type
        impl::TokenType m_CurrentType = impl::TokenType::Key;

        /// @brief Tells if the parser has reached the end of input
        /// @return `true` if the parser has reached the end of input,
        /// `false` otherwise
        bool IsEndOfInput() { return m_Pos >= m_Input.size(); }

        /// @brief Skips @ref bytesCount bytes of input
        /// @param bytesCount how many bytes to skip
        void Skip(size_t bytesCount = 1)
        {
            m_Pos += bytesCount;
        }

        /// @brief Skips BOM
        void SkipBom()
        {
            solinject_req_assert(m_Pos == 0);

            if (m_Input.size() <= 3)
                return;

            byte bom[] = { 0xEF, 0xBB, 0xBF };

            if (std::memcmp(m_Input.data(), bom, 3) == 0)
                Skip(3);
        }

        /// @brief Gets current codepoint size
        /// @return current codepoint size
        size_t GetCodepointSize()
        {
            byte firstByte = m_Input[m_Pos];

            if ((firstByte & 0b1000'0000u) == 0)
                return 1;
            else if ((firstByte & 0b1110'0000u) == 0b1100'0000u)
                return 2;
            else if ((firstByte & 0b1111'0000u) == 0b1110'0000u)
                return 3;
            else if ((firstByte & 0b1111'1000u) == 0b1111'0000u)
                return 4;
            else
                throw std::runtime_error("Invalid UTF8 character");
        }

        /// @brief Resets state and returns token
        /// @param[out] token token
        /// @param skipTypeGuess `true` if token type guess should be skipped
        void ResetStateAndReturnToken(Token& token, bool skipTypeGuess = false)
        {
            using namespace impl;

            if (!skipTypeGuess)
                if (m_Lexem == "Self")
                    m_CurrentType = TokenType::Self;
                else if (m_Lexem == "Singleton")
                    m_CurrentType = TokenType::Singleton;
                else if (m_Lexem == "Transient")
                    m_CurrentType = TokenType::Transient;
                else if (m_Lexem == "Shared")
                    m_CurrentType = TokenType::Shared;
                else if (m_Lexem == "Scoped")
                    m_CurrentType = TokenType::Scoped;
                else if (m_Lexem == "None")
                    m_CurrentType = TokenType::None;

            token = Token(m_CurrentType, m_Lexem);

            m_CurrentType = TokenType::Key;
            m_Lexem = "";
        }

        /// @brief Skips comment
        void SkipComment()
        {
            while (!IsEndOfInput())
            {
                size_t codepointSize = GetCodepointSize();

                if (codepointSize > 1)
                {
                    Skip(codepointSize);
                    continue;
                }
                else
                {
                    byte codepoint = m_Input[m_Pos];

                    Skip();

                    if (codepoint == '\n')
                        break;
                }
            }
        }

        /// @brief Tokenizes quoted literal
        void TokenizeQuotedLiteral()
        {
            Skip();

            while (!IsEndOfInput())
            {
                size_t codepointSize = GetCodepointSize();

                if (codepointSize > 1)
                {
                    m_Lexem.append(m_Input.data() + m_Pos, codepointSize);
                    Skip(codepointSize - 1);
                }
                else
                {
                    byte codepoint = m_Input[m_Pos];

                    Skip();

                    if (codepoint == '\"')
                        break;
                    else
                        m_Lexem += codepoint;
                }
            }

            m_CurrentType = impl::TokenType::Key;
        }

        /// @brief Tokenizes escaped character
        void TokenizeEscapedCharacter()
        {
            Skip();

            size_t codepointSize = GetCodepointSize();

            m_Lexem.append(m_Input.data() + m_Pos, codepointSize);
            Skip(codepointSize - 1);
        }

        /// @brief Returns next token
        /// @param[out] token token
        /// @return `true` if @p token has been set to a valid token
        bool GetNextToken(Token& token)
        {
            using namespace impl;

            for (; !IsEndOfInput(); m_Pos++)
            {
                size_t codepointSize = GetCodepointSize();
                if (codepointSize > 1)
                {
                    m_Lexem.append(m_Input.data() + m_Pos, codepointSize);
                    Skip(codepointSize - 1);
                }
                else
                {
                    byte codepoint = m_Input[m_Pos];

                    switch (codepoint)
                    {
                        case '\"':
                            TokenizeQuotedLiteral();
                            ResetStateAndReturnToken(token, true);
                            return true;

                        case '\\':
                            TokenizeEscapedCharacter();
                            break;

                        case ' ':
                        case '\n':
                        case '\t':
                            if (m_Lexem != "")
                            {
                                ResetStateAndReturnToken(token);
                                return true;
                            }

                            break;

                        case '#':
                            if (m_Lexem != "")
                            {
                                ResetStateAndReturnToken(token);
                                return true;
                            }

                            SkipComment();

                            break;

                        case '{':
                            if (m_Lexem != "")
                            {
                                ResetStateAndReturnToken(token);
                                return true;
                            }

                            token = Token(TokenType::OpeningCurlyBracket, "");
                            Skip();
                            return true;

                        case '}':
                            if (m_Lexem != "")
                            {
                                ResetStateAndReturnToken(token);
                                return true;
                            }

                            token = Token(TokenType::ClosingCurlyBracket, "");
                            Skip();
                            return true;

                        default:
                            m_Lexem += codepoint;
                            break;
                    }
                }
            }

            if (m_Lexem != "")
            {
                ResetStateAndReturnToken(token);
                return true;
            }

            return false;
        }

        /// @brief Parses configuration items
        /// @param[in] initialToken previous token
        /// @return parsed configuration items
        std::vector<ConfigurationItem> ParseConfigurationItem(const Token& initialToken)
        {
            using namespace impl;

            solinject_req_assert(initialToken.Type() == TokenType::Key);

            std::string interfaceKey = initialToken.Content();
            std::vector<ConfigurationItem> result;
            Token token(TokenType::Key, "");

            if (!GetNextToken(token))
                throw std::runtime_error("Unexpected end of input");

            switch (token.Type())
            {
                case TokenType::Key:
                case TokenType::Self:
                    result.push_back(
                        ParseImplementationRegistration(interfaceKey, token)
                    );
                    break;
                case TokenType::OpeningCurlyBracket:
                    result = ParseMultipleImplementationRegistrations(interfaceKey);
                    break;
            }

            return result;
        }

        /// @brief Parses implementation registration
        /// @param interfaceKey the configuration item interface key
        /// @param initialToken the previous token
        /// @return parsed configuration item
        ConfigurationItem ParseImplementationRegistration(std::string interfaceKey, const Token& initialToken)
        {
            using namespace impl;

            solinject_assert(
                initialToken.Type() == TokenType::Key ||
                initialToken.Type() == TokenType::Self
            );

            std::string implementationKey;

            switch (initialToken.Type())
            {
                case TokenType::Key:
                    implementationKey = initialToken.Content();
                    break;
                case TokenType::Self:
                    implementationKey = interfaceKey;
                    break;
                default:
                    using namespace std::string_literals;
                    throw std::runtime_error("Unexpected token: "s + initialToken.Content());
            }

            Token token(TokenType::Key, "");

            if (!GetNextToken(token))
                throw std::runtime_error("Unexpected end of input");

            ServiceLifetime lifetime = ServiceLifetime::Singleton;

            switch (token.Type())
            {
            case TokenType::Singleton:
                lifetime = ServiceLifetime::Singleton;
                break;

            case TokenType::Transient:
                lifetime = ServiceLifetime::Transient;
                break;

            case TokenType::Shared:
                lifetime = ServiceLifetime::Shared;
                break;

            case TokenType::Scoped:
                lifetime = ServiceLifetime::Scoped;
                break;

            default:
                using namespace std::string_literals;
                throw std::runtime_error("Unexpected token: "s + token.Content());
            }

            return ConfigurationItem(interfaceKey, implementationKey, lifetime);
        }

        /// @brief Parses multiple implementation registrations
        /// @param interfaceKey the configuration item interface key
        /// @return parsed configuration items
        std::vector<ConfigurationItem> ParseMultipleImplementationRegistrations(std::string interfaceKey)
        {
            using namespace impl;

            Token token(TokenType::Key, "");
            std::vector<ConfigurationItem> result;

            while(GetNextToken(token))
            {
                if (token.Type() == TokenType::ClosingCurlyBracket)
                    break;

                result.push_back(
                    ParseImplementationRegistration(interfaceKey, token)
                );
            }

            return result;
        }
    };
}
