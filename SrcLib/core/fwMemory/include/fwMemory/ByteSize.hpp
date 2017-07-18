/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEMORY_BYTESIZE_HPP__
#define __FWMEMORY_BYTESIZE_HPP__

#include "fwMemory/config.hpp"

#include <fwCore/base.hpp>

#include <cstdint>
#include <string>

namespace fwMemory
{

/**
 * @brief Conversion helper for size in bytes
 * Converts string to number of bytes and vice-versa. This class is also able to
 * manage conversions between units standards (IEC, SI)
 */
class FWMEMORY_CLASS_API ByteSize
{
public:

    typedef enum
    {
        SI, IEC
    } StandardType;

    FWMEMORY_API static const std::uint64_t Bytes;

    // SI units
    FWMEMORY_API static const std::uint64_t KB;
    FWMEMORY_API static const std::uint64_t MB;
    FWMEMORY_API static const std::uint64_t GB;
    FWMEMORY_API static const std::uint64_t TB;
    FWMEMORY_API static const std::uint64_t PB;

    // IEC units
    FWMEMORY_API static const std::uint64_t KiB;
    FWMEMORY_API static const std::uint64_t MiB;
    FWMEMORY_API static const std::uint64_t GiB;
    FWMEMORY_API static const std::uint64_t TiB;
    FWMEMORY_API static const std::uint64_t PiB;

    typedef ::std::uint64_t SizeType;
    typedef ::std::uint64_t UnitType;

    /**
     * @brief Default constructor
     */
    FWMEMORY_API ByteSize ();

    /**
     * @brief Build a ByteSize object from given size and unit
     * @{
     */
    FWMEMORY_API ByteSize ( SizeType size, UnitType unit = Bytes );
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
    FWMEMORY_API ByteSize ( double size, UnitType unit = Bytes );

    /**
     * @brief Build a ByteSize object from given string
     */
    FWMEMORY_API ByteSize ( const std::string& size );

    /**
     * @brief Convert this size to a string with specified unit
     *
     * @param unit Unit to convert this size to.
     *
     * @return std::string representing the size
     */
    FWMEMORY_API std::string getSizeAsString( UnitType unit = Bytes );
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
    FWMEMORY_API std::string getHumanReadableSize( StandardType standard = IEC );

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
    FWMEMORY_API void setSize( SizeType size, UnitType unit = Bytes );
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
    FWMEMORY_API void setSize( double size, UnitType unit = Bytes );
    /**
     * @brief Build a ByteSize object from given string
     */
    FWMEMORY_API void setSize( const std::string& size );

    FWMEMORY_API ByteSize& operator= ( SizeType size );
    FWMEMORY_API ByteSize& operator= ( double size );
    FWMEMORY_API ByteSize& operator= ( const std::string& size );
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

    FWMEMORY_API static bool parseSize(const std::string& s, std::uint64_t& size);
    FWMEMORY_API static std::string unitToString( UnitType unit );

protected:
    SizeType m_size;

};

} //namespace fwMemory

#endif /* __FWMEMORY_BYTESIZE_HPP__ */
