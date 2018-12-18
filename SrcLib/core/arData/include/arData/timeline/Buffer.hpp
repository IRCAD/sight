/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
