/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_TIMELINE_BUFFER_HPP__
#define __ARDATA_TIMELINE_BUFFER_HPP__

#include <boost/function.hpp>

#include "arData/timeline/Object.hpp"
#include "arData/config.hpp"

namespace arData
{

namespace timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class ARDATA_CLASS_API Buffer : public arData::timeline::Object
{
public:
    typedef uint8_t* BufferDataType;
    typedef ::boost::function< void (void*) > DeleterType;

    /// Constructor
    ARDATA_API Buffer(::fwCore::HiResClock::HiResClockType timestamp = 0,
                      BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

    /// Destructor
    ARDATA_API virtual ~Buffer();

    /// Makes a copy of this buffer
    ARDATA_API virtual void deepCopy(const ::arData::timeline::Object& other);

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

} // namespace arData

#endif /* __ARDATA_TIMELINE_BUFFER_HPP__ */
