/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_TIMELINE_RAWBUFFER_HPP__
#define __EXTDATA_TIMELINE_RAWBUFFER_HPP__

#include <boost/function.hpp>

#include "extData/timeline/Buffer.hpp"
#include "extData/config.hpp"

namespace extData
{

namespace timeline
{

/**
 * @class   Object
 * @brief   This class defines a TimeLine object buffer.
 */
class EXTDATA_CLASS_API RawBuffer : public extData::timeline::Buffer
{
public:

    /// Constructor
    EXTDATA_API RawBuffer(::fwCore::HiResClock::HiResClockType timestamp = 0,
                          BufferDataType buffer = 0, size_t size = 0, DeleterType d = 0);

    /// Destructor
    EXTDATA_API virtual ~RawBuffer();

    /// Set buffer values
    EXTDATA_API void setBufferValues(BufferDataType values);

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

} // namespace extData

#endif /* __EXTDATA_TIMELINE_RAWBUFFER_HPP__ */
