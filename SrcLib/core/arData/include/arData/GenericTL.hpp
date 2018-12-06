/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
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

#ifndef __ARDATA_GENERICTL_HPP__
#define __ARDATA_GENERICTL_HPP__

#include "arData/BufferTL.hpp"
#include "arData/config.hpp"
#include "arData/timeline/GenericObject.hpp"

namespace arData
{
/**
 * @brief   This class defines a timeline of template objects. All objects have the same maximum number of elements,
 *          nevertheless it is not mandatory to provide all elements.
 */
template < class BUFFER_TYPE >
class GenericTL : public BufferTL
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (GenericTL<BUFFER_TYPE>)(::fwData::Object) )

    typedef ::arData::timeline::GenericObject< BUFFER_TYPE > BufferType;
    /**
     * @brief Constructor
     * @param key Private construction key
     */
    GenericTL( ::fwData::Object::Key key );

    /// Destructor
    virtual ~GenericTL();

    /// Defines deep copy
    virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /**
     * @brief Returns the closest buffer to the given timestamp
     * @param timestamp timestamp used to find the closest buffer
     * @param direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    CSPTR(BufferType) getClosestBuffer( ::fwCore::HiResClock::HiResClockType timestamp,
                                        DirectionType direction = BOTH) const;

    /// Returns the buffer matching the specified timestamp, returns NULL if object is not found
    CSPTR(BufferType) getBuffer(::fwCore::HiResClock::HiResClockType timestamp) const;

    /// Initializes the size of the pool buffer.
    virtual void initPoolSize(unsigned int maxElementNum);

    /**
     * @brief Returns a new ::arData::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SPTR(::arData::timeline::Object) createObject(::fwCore::HiResClock::HiResClockType timestamp) override;

    /**
     * @brief Returns a new BufferType with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    SPTR(BufferType) createBuffer(::fwCore::HiResClock::HiResClockType timestamp);

    /// Check if the type of an object is compatible with this timeline
    virtual bool isObjectValid(const CSPTR(::arData::timeline::Object)& obj) const override;

    /// Get/set the maximum number of objects inside a single buffer
    unsigned int getMaxElementNum() const;

protected:
    /// maximum number of elements inside a single buffer
    unsigned int m_maxElementNum;

}; // class GenericTL

} // namespace arData

#endif // __ARDATA_GENERICTL_HPP__
