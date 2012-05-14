/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWTOOLS_BYTESIZE_HPP_
#define _FWTOOLS_BYTESIZE_HPP_

#include <string>

#include <boost/cstdint.hpp>

#include <fwCore/base.hpp>

#include "fwTools/config.hpp"

namespace fwTools
{

/**
 * @brief Convertion helper for size in bytes
 * Converts string to number of bytes and vice-versa. This class is also able to
 * manage convertions between units standards (IEC, SI)
 */
class FWTOOLS_CLASS_API ByteSize
{
public:

    typedef enum {
        SI, IEC
    } StandardType ;

    static const boost::uint64_t Bytes;

    // SI units
    static const boost::uint64_t KB;
    static const boost::uint64_t MB;
    static const boost::uint64_t GB;
    static const boost::uint64_t TB;
    static const boost::uint64_t PB;

    // IEC units
    static const boost::uint64_t KiB;
    static const boost::uint64_t MiB;
    static const boost::uint64_t GiB;
    static const boost::uint64_t TiB;
    static const boost::uint64_t PiB;

    typedef ::boost::uint64_t SizeType;
    typedef ::boost::uint64_t UnitType;

    /**
     * @brief Default constructor
     */
    FWTOOLS_API ByteSize ();

    /**
     * @brief Build a ByteSize object from given size and unit
     */
    FWTOOLS_API ByteSize ( SizeType size, UnitType unit = Bytes );


    /**
     * @brief Build a ByteSize object from given size and unit
     */
    FWTOOLS_API ByteSize ( double size, UnitType unit = Bytes );

    /**
     * @brief Build a ByteSize object from given string
     */
    FWTOOLS_API ByteSize ( const std::string &size );

    /**
     * @brief Convert this size to a string with specified unit
     *
     * @param unit Unit to convert this size to.
     *
     * @return std::string representing the size
     */
    FWTOOLS_API std::string getSizeAsString( UnitType unit = Bytes );
    /**
     * @brief Convert this size to a human readable string in the required
     * Convert this size to a human readable string in the required standard.
     * The unit will be choosen to make the numeric value fit in [0; 1024[ when
     * possible.
     *
     * @param standard Wanted set of unit
     *
     * @return std::string representing the size
     */
    FWTOOLS_API std::string getHumanReadableSize( StandardType standard = IEC );

    /**
     * @brief Returns size in bytes.
     */
    FWTOOLS_API SizeType getSize(){ return m_size; }

    /**
     * @brief Build a ByteSize object from given size and unit
     */
    FWTOOLS_API void setSize( SizeType size, UnitType unit = Bytes );
    /**
     * @brief Build a ByteSize object from given size and unit
     */
    FWTOOLS_API void setSize( double size, UnitType unit = Bytes );
    /**
     * @brief Build a ByteSize object from given string
     */
    FWTOOLS_API void setSize( const std::string& size );

    FWTOOLS_API ByteSize& operator= ( SizeType size );
    FWTOOLS_API ByteSize& operator= ( double size );
    FWTOOLS_API ByteSize& operator= ( const std::string &size );

    FWTOOLS_API operator SizeType () { return m_size; }
    FWTOOLS_API operator std::string () { return getHumanReadableSize(); }

    FWTOOLS_API static bool parseSize(const std::string &s, boost::uint64_t& size);
    FWTOOLS_API static std::string unitToString( UnitType unit );

protected:
    SizeType m_size;

};


} //namespace fwTools

#endif /* _FWTOOLS_BYTESIZE_HPP_ */
