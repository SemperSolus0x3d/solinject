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
#include <vector>
#include "ConfigurationItem.hpp"

namespace sol::di
{
    /// @brief DI configuration
    class Configuration
    {
    public:
        /// @brief Parameterless constructor
        Configuration() {}

        /// @brief Constructor
        /// @param items the configuration items
        Configuration(std::vector<ConfigurationItem> items) :
            m_ConfigurationItems(std::move(items))
        {
        }

        /// @brief Copy constructor
        Configuration(const Configuration& other) :
            m_ConfigurationItems(other.m_ConfigurationItems)
        {
        }

        /// @brief Move constructor
        Configuration(Configuration&& other) noexcept : Configuration()
        {
            swap(*this, other);
        }

        /// @brief Copy-assignment operator
        Configuration& operator=(Configuration other) noexcept
        {
            swap(*this, other);
            return *this;
        }

        /// @brief Swaps two @ref Configuration instances
        friend void swap(Configuration& a, Configuration& b)
        {
            using std::swap;

            swap(a.m_ConfigurationItems, b.m_ConfigurationItems);
        }

        /// @brief Configuration items property
        /// @return the configuration items
        const std::vector<ConfigurationItem>& ConfigurationItems() const
        {
            return m_ConfigurationItems;
        }

        /// @brief Adds a configuration item
        /// @param item the configuration item
        void AddConfigurationItem(ConfigurationItem item)
        {
            m_ConfigurationItems.push_back(std::move(item));
        }

        /// @brief Adds configuration items
        /// @param items the configuration items
        void AddConfigurationItems(std::vector<ConfigurationItem> items)
        {
            impl::ConcatenateVectors(m_ConfigurationItems, std::move(items));
        }
    private:
        /// @brief The configuration items
        std::vector<ConfigurationItem> m_ConfigurationItems;
    };
}
