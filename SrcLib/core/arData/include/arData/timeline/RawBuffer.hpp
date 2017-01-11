/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ARDATA_TIMELINE_RAWBUFFER_HPP__
#define __ARDATA_TIMELINE_RAWBUFFER_HPP__

#include <boost/function.hpp>

#include "arData/timeline/Buffer.hpp"
#include "arData/config.hpp"

namespace arData
{

namespace timeline
{

/**
 * @brief   This class defines a TimeLine object buffer.
 */
class ARDATA_CLASS_API RawBuffer : public arData::timeline::Buffer
{
public:

    /// Constructor
    ARDATA_API RawBuffer(::fwCore::HiResClock::HiResClockType timestamp = 0,
                         BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

    /// Destructor
    ARDATA_API virtual ~RawBuffer();

    /// Set buffer values
    ARDATA_API void setBufferValues(BufferDataType values);

    /// Return buffer
    BufferDataType getBuffer() const
    {
        return m_buffer;
    }

    template<typename TYPE>
    TYPE* getBuffer() const
    {
        return reinterpret_cast<TYPE*>(m_buffer);
    }
};

} // namespace timeline

} // namespace arData

#endif /* __ARDATA_TIMELINE_RAWBUFFER_HPP__ */
