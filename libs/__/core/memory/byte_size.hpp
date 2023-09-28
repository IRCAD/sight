/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "core/config.hpp"

#include <core/base.hpp>

#include <cstdint>
#include <string>

namespace sight::core::memory
{

/**
 * @brief Conversion helper for size in bytes
 * Converts string to number of bytes and vice-versa. This class is also able to
 * manage conversions between units standards (IEC, SI)
 */
class CORE_CLASS_API byte_size
{
public:

    typedef enum
    {
        SI,
        IEC
    } standard_type;

    CORE_API static constexpr std::uint64_t bytes = 1;

    // SI units
    CORE_API static constexpr std::uint64_t KB = 1000LL;
    CORE_API static constexpr std::uint64_t MB = 1000000LL;
    CORE_API static constexpr std::uint64_t GB = 1000000000LL;
    CORE_API static constexpr std::uint64_t TB = 1000000000000LL;
    CORE_API static constexpr std::uint64_t PB = 1000000000000000LL;

    // IEC units
    CORE_API static constexpr std::uint64_t ki_b = 1LL << 10;
    CORE_API static constexpr std::uint64_t mi_b = 1LL << 20;
    CORE_API static constexpr std::uint64_t gi_b = 1LL << 30;
    CORE_API static constexpr std::uint64_t ti_b = 1LL << 40;
    CORE_API static constexpr std::uint64_t pi_b = 1LL << 50;

    using size_t = std::uint64_t;
    using unit_t = std::uint64_t;

    /**
     * @brief Default constructor
     */
    CORE_API byte_size();

    /**
     * @brief Build a byte_size object from given size and unit
     * @{
     */
    CORE_API byte_size(size_t size, unit_t unit = bytes);
    template<typename T>
    byte_size(T size, unit_t unit = bytes)
    {
        this->set_size(size_t(size), unit);
    }

    ///@}

    /**
     * @brief Build a byte_size object from given size and unit
     */
    CORE_API byte_size(double size, unit_t unit = bytes);

    /**
     * @brief Build a byte_size object from given string
     */
    CORE_API byte_size(const std::string& size);

    /**
     * @brief Convert this size to a string with specified unit
     *
     * @param unit Unit to convert this size to.
     *
     * @return std::string representing the size
     */
    [[nodiscard]] CORE_API std::string get_size_as_string(unit_t unit = bytes) const;
    /**
     * @brief Convert this size to a human readable string in the required
     * Convert this size to a human readable string in the required standard.
     * The unit will be chosen to make the numeric value fit in [0; 1024[ when
     * possible.
     *
     * @param standard Wanted set of unit
     *
     * @return std::string representing the size
     */
    [[nodiscard]] CORE_API std::string get_human_readable_size(standard_type standard = IEC) const;

    /**
     * @brief Returns size in bytes.
     */
    [[nodiscard]] size_t size() const
    {
        return m_size;
    }

    /**
     * @brief Build a byte_size object from given size and unit
     * @{
     */
    CORE_API void set_size(size_t size, unit_t unit = bytes);
    //------------------------------------------------------------------------------

    template<typename T>
    void set_size(T size, unit_t unit = bytes)
    {
        this->set_size(size_t(size), unit);
    }

    ///@}

    /**
     * @brief Build a byte_size object from given size and unit
     */
    CORE_API void set_size(double size, unit_t unit = bytes);
    /**
     * @brief Build a byte_size object from given string
     */
    CORE_API void set_size(const std::string& size);

    CORE_API byte_size& operator=(size_t size);
    CORE_API byte_size& operator=(double size);
    CORE_API byte_size& operator=(const std::string& size);
    //------------------------------------------------------------------------------

    template<typename T>
    byte_size& operator=(T size)
    {
        this->operator=(size_t(size));
        return *this;
    }

    operator size_t() const
    {
        return m_size;
    }
    operator std::string() const
    {
        return get_human_readable_size();
    }

    CORE_API static bool parse_size(const std::string& s, std::uint64_t& size);
    CORE_API static std::string unit_to_string(unit_t unit);

protected:

    size_t m_size {};
};

} //namespace sight::core::memory
