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

namespace sol::di
{
    /// @brief Service lifetime
    enum class ServiceLifetime
    {
        Singleton = 0, //< Singleton service lifetime
        Transient, //< Transient service lifetime
        Shared, //< Shared service lifetime
        Scoped, //< Scoped service lifetime
        None //< No lifetime. Only valid for interface-to-interface registration.
    };

    /// @brief DI configuration item
    class ConfigurationItem
    {
    public:
        /// @brief Key, used to map classes,
        /// registered in a @ref ContainerBuilder
        /// to the ones specified in a config file
        using Key = std::string;

        /// @brief Parameterless constructor
        ConfigurationItem() : ConfigurationItem("", "", ServiceLifetime::Singleton)
        {
        }

        /// @brief Constructor
        /// @param interfaceKey the interface key
        /// @param implementationKey the implementation key
        /// @param lifetime the service lifetime
        ConfigurationItem(
            std::string interfaceKey,
            std::string implementationKey,
            ServiceLifetime lifetime
        ) :
            m_InterfaceKey(interfaceKey),
            m_ImplementationKey(implementationKey),
            m_Lifetime(lifetime)
        {
        }

        /// @brief Constructor. Used when the interface key
        /// and the implementation key are identical
        /// @param key the service key
        /// @param lifetime the service lifetime
        ConfigurationItem(std::string key, ServiceLifetime lifetime) :
            ConfigurationItem(key, key, lifetime)
        {
        }

        /// @brief Copy constructor
        ConfigurationItem(const ConfigurationItem& other) :
            m_InterfaceKey(other.m_InterfaceKey),
            m_ImplementationKey(other.m_ImplementationKey),
            m_Lifetime(other.m_Lifetime)
        {
        }

        /// @brief Move constructor
        ConfigurationItem(ConfigurationItem&& other) noexcept :
            ConfigurationItem()
        {
            swap(*this, other);
        }

        /// @brief Copy-assignment operator
        ConfigurationItem& operator=(ConfigurationItem other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        /// @brief Swaps to @ref ConfigurationItem instances
        friend void swap(ConfigurationItem& a, ConfigurationItem& b)
        {
            using std::swap;

            swap(a.m_InterfaceKey, b.m_InterfaceKey);
            swap(a.m_ImplementationKey, b.m_ImplementationKey);
            swap(a.m_Lifetime, b.m_Lifetime);
        }

        /// @brief Interface key property
        /// @return the interface key
        Key InterfaceKey() const { return m_InterfaceKey; }

        /// @brief Implementation key property
        /// @return the implementation key
        Key ImplementationKey() const { return m_ImplementationKey; }

        /// @brief Lifetime property
        /// @return the service lifetime
        ServiceLifetime Lifetime() const { return m_Lifetime; }

    private:
        /// @brief The interface key
        Key m_InterfaceKey;

        /// @brief The implementation key
        Key m_ImplementationKey;

        /// @brief The service lifetime
        sol::di::ServiceLifetime m_Lifetime;
    };
}
