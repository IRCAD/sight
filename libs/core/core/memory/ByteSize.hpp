/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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
class CORE_CLASS_API ByteSize
{
public:

    typedef enum
    {
        SI, IEC
    } StandardType;

    CORE_API static const std::uint64_t Bytes;

    // SI units
    CORE_API static const std::uint64_t KB;
    CORE_API static const std::uint64_t MB;
    CORE_API static const std::uint64_t GB;
    CORE_API static const std::uint64_t TB;
    CORE_API static const std::uint64_t PB;

    // IEC units
    CORE_API static const std::uint64_t KiB;
    CORE_API static const std::uint64_t MiB;
    CORE_API static const std::uint64_t GiB;
    CORE_API static const std::uint64_t TiB;
    CORE_API static const std::uint64_t PiB;

    typedef std::uint64_t SizeType;
    typedef std::uint64_t UnitType;

    /**
     * @brief Default constructor
     */
    CORE_API ByteSize ();

    /**
     * @brief Build a ByteSize object from given size and unit
     * @{
     */
    CORE_API ByteSize ( SizeType size, UnitType unit = Bytes );
    template <typename T>
    ByteSize ( T size, UnitType unit = Bytes ) :
        m_size(0)
    {
        this->setSize( SizeType(size), unit);
    }
    ///@}

    /**
     * @brief Build a ByteSize object from given size and unit
     */
    CORE_API ByteSize ( double size, UnitType unit = Bytes );

    /**
     * @brief Build a ByteSize object from given string
     */
    CORE_API ByteSize ( const std::string& size );

    /**
     * @brief Convert this size to a string with specified unit
     *
     * @param unit Unit to convert this size to.
     *
     * @return std::string representing the size
     */
    CORE_API std::string getSizeAsString( UnitType unit = Bytes );
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
    CORE_API std::string getHumanReadableSize( StandardType standard = IEC );

    /**
     * @brief Returns size in bytes.
     */
    SizeType getSize()
    {
        return m_size;
    }

    /**
     * @brief Build a ByteSize object from given size and unit
     * @{
     */
    CORE_API void setSize( SizeType size, UnitType unit = Bytes );
    //------------------------------------------------------------------------------

    template <typename T>
    void setSize( T size, UnitType unit = Bytes )
    {
        this->setSize(SizeType(size), unit);
    }
    ///@}

    /**
     * @brief Build a ByteSize object from given size and unit
     */
    CORE_API void setSize( double size, UnitType unit = Bytes );
    /**
     * @brief Build a ByteSize object from given string
     */
    CORE_API void setSize( const std::string& size );

    CORE_API ByteSize& operator= ( SizeType size );
    CORE_API ByteSize& operator= ( double size );
    CORE_API ByteSize& operator= ( const std::string& size );
    //------------------------------------------------------------------------------

    template <typename T>
    ByteSize& operator= ( T size )
    {
        return this->operator=( SizeType(size) );
    }

    operator SizeType() {
        return m_size;
    }
    operator std::string() {
        return getHumanReadableSize();
    }

    CORE_API static bool parseSize(const std::string& s, std::uint64_t& size);
    CORE_API static std::string unitToString( UnitType unit );

protected:
    SizeType m_size;

};

} //namespace sight::core::memory
