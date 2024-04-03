/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <sight/core/config.hpp>

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
class SIGHT_CORE_CLASS_API byte_size
{
public:

    enum standard_type
    {
        si,
        iec
    };

    SIGHT_CORE_API static constexpr std::uint64_t BYTES = 1;

    // SI units
    SIGHT_CORE_API static constexpr std::uint64_t KB = 1000LL;
    SIGHT_CORE_API static constexpr std::uint64_t MB = 1000000LL;
    SIGHT_CORE_API static constexpr std::uint64_t GB = 1000000000LL;
    SIGHT_CORE_API static constexpr std::uint64_t TB = 1000000000000LL;
    SIGHT_CORE_API static constexpr std::uint64_t PB = 1000000000000000LL;

    // IEC units
    SIGHT_CORE_API static constexpr std::uint64_t KI_B = 1LL << 10;
    SIGHT_CORE_API static constexpr std::uint64_t MI_B = 1LL << 20;
    SIGHT_CORE_API static constexpr std::uint64_t GI_B = 1LL << 30;
    SIGHT_CORE_API static constexpr std::uint64_t TI_B = 1LL << 40;
    SIGHT_CORE_API static constexpr std::uint64_t PI_B = 1LL << 50;

    using size_t = std::uint64_t;
    using unit_t = std::uint64_t;

    /**
     * @brief Default constructor
     */
    SIGHT_CORE_API byte_size();

    /**
     * @brief Build a byte_size object from given size and unit
     * @{
     */
    SIGHT_CORE_API byte_size(size_t _size, unit_t _unit = BYTES);
    template<typename T>
    byte_size(T _size, unit_t _unit = BYTES)
    {
        this->set_size(size_t(_size), _unit);
    }

    ///@}

    /**
     * @brief Build a byte_size object from given size and unit
     */
    SIGHT_CORE_API byte_size(double _size, unit_t _unit = BYTES);

    /**
     * @brief Build a byte_size object from given string
     */
    SIGHT_CORE_API byte_size(const std::string& _size);

    /**
     * @brief Convert this size to a string with specified unit
     *
     * @param _unit Unit to convert this size to.
     *
     * @return std::string representing the size
     */
    [[nodiscard]] SIGHT_CORE_API std::string get_size_as_string(unit_t _unit = BYTES) const;
    /**
     * @brief Convert this size to a human readable string in the required
     * Convert this size to a human readable string in the required standard.
     * The unit will be chosen to make the numeric value fit in [0; 1024[ when
     * possible.
     *
     * @param _standard Wanted set of unit
     *
     * @return std::string representing the size
     */
    [[nodiscard]] SIGHT_CORE_API std::string get_human_readable_size(standard_type _standard = iec) const;

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
    SIGHT_CORE_API void set_size(size_t _size, unit_t _unit = BYTES);
    //------------------------------------------------------------------------------

    template<typename T>
    void set_size(T _size, unit_t _unit = BYTES)
    {
        this->set_size(size_t(_size), _unit);
    }

    ///@}

    /**
     * @brief Build a byte_size object from given size and unit
     */
    SIGHT_CORE_API void set_size(double _size, unit_t _unit = BYTES);
    /**
     * @brief Build a byte_size object from given string
     */
    SIGHT_CORE_API void set_size(const std::string& _size);

    SIGHT_CORE_API byte_size& operator=(size_t _size);
    SIGHT_CORE_API byte_size& operator=(double _size);
    SIGHT_CORE_API byte_size& operator=(const std::string& _size);
    //------------------------------------------------------------------------------

    template<typename T>
    byte_size& operator=(T _size)
    {
        this->operator=(size_t(_size));
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

    SIGHT_CORE_API static bool parse_size(const std::string& _s, std::uint64_t& _size);
    SIGHT_CORE_API static std::string unit_to_string(unit_t _unit);

protected:

    size_t m_size {};
};

} //namespace sight::core::memory
