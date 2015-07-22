/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_TIMELINE_BUFFER_HPP__
#define __EXTDATA_TIMELINE_BUFFER_HPP__

#include <boost/function.hpp>

#include "extData/timeline/Object.hpp"
#include "extData/config.hpp"

namespace extData
{

namespace timeline
{

/**
 * @class   Object
 * @brief   This class defines a TimeLine object buffer.
 */
class EXTDATA_CLASS_API Buffer : public extData::timeline::Object
{
public:
    typedef ::boost::uint8_t* BufferDataType;
    typedef ::boost::function< void (void *) > DeleterType;

    /// Constructor
    EXTDATA_API Buffer(::fwCore::HiResClock::HiResClockType timestamp = 0,
                       BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

    /// Destructor
    EXTDATA_API virtual ~Buffer();

    /// Makes a copy of this buffer
    EXTDATA_API virtual void deepCopy(const ::extData::timeline::Object& other);

    /// Returns size
    size_t getSize() const
    {
        return m_size;
    }

protected:

    ///Buffer size
    size_t m_size;

    ///Buffer
    BufferDataType m_buffer;

    ///Deleter
    DeleterType m_deleter;

};

} // namespace timeline

} // namespace extData

#endif /* __EXTDATA_TIMELINE_BUFFER_HPP__ */
