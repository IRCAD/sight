/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_RAWBUFFERTL_HPP__
#define __EXTDATA_RAWBUFFERTL_HPP__

#include "extData/config.hpp"
#include "extData/BufferTL.hpp"
#include "extData/timeline/RawBuffer.hpp"

#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((extData)(RawBufferTL), EXTDATA_API);

namespace extData
{
/**
 * @class   BufferTL
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class EXTDATA_CLASS_API RawBufferTL : public BufferTL
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (RawBufferTL)(::extData::TimeLine),(()),
                                            ::fwData::factory::New< RawBufferTL >);
    fwCampMakeFriendDataMacro((extData)(RawBufferTL));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    EXTDATA_API RawBufferTL( ::fwData::Object::Key key );

    /// Destructor
    EXTDATA_API virtual ~RawBufferTL();

    /// Defines deep copy
    EXTDATA_API virtual void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType &cache);

    /**
     * @brief Return the closest buffer to the given timestamp
     * @param timestamp timestamp used to find the closest buffer
     * @param direction direction to find the closest buffer (PAST, FUTURE, BOTH)
     */
    EXTDATA_API virtual CSPTR(::extData::timeline::RawBuffer) getClosestBuffer(
        ::fwCore::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH) const;

    /// Return the buffer matching the specified timestamp, returns NULL if object is not found
    EXTDATA_API virtual CSPTR(::extData::timeline::RawBuffer) getBuffer(::fwCore::HiResClock::HiResClockType timestamp)
    const;

    /// Initialize the size of the pool buffer.
    EXTDATA_API void initPoolSize(std::size_t size);

    /**
     * @brief Return a new ::extData::timeline::Object with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    EXTDATA_API SPTR(::extData::timeline::Object) createObject(::fwCore::HiResClock::HiResClockType timestamp);

    /**
     * @brief Return a new BufferType with the given timestamp.
     * @note This buffer memory is managed by the pool.
     * @warning This buffer is not registered in the timeline. You must call pushObject() to register it.
     */
    EXTDATA_API SPTR(::extData::timeline::RawBuffer) createBuffer(::fwCore::HiResClock::HiResClockType timestamp);

    /// Check if the type of an object is compatible with this timeline
    EXTDATA_API virtual bool isObjectValid(const CSPTR(::extData::timeline::Object) &obj) const;

}; // class RawBufferTL

} // namespace extData


#endif // __EXTDATA_RAWBUFFERTL_HPP__
