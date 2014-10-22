/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __EXTDATA_BUFFERTL_HPP__
#define __EXTDATA_BUFFERTL_HPP__

#include <boost/array.hpp>
#include <boost/pool/poolfwd.hpp>

#include "extData/config.hpp"
#include "extData/timeline/Buffer.hpp"
#include "extData/TimeLine.hpp"

fwCampAutoDeclareDataMacro((extData)(BufferTL), EXTDATA_API);


namespace extData
{
/**
 * @class   BufferTL
 * @brief   This class defines a timeline of buffers. It implements basic features of the Timeline interface such as
 *          pushing or retrieving objects. Allocation must be done by inherited classes.
 */
class EXTDATA_CLASS_API BufferTL : public TimeLine
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (BufferTL) (::fwData::Object) );

    typedef ::fwCore::HiResClock::HiResClockType TimestampType;
    typedef std::map< TimestampType, SPTR(::extData::timeline::Buffer) > TimelineType;
    typedef std::pair< TimestampType, SPTR(::extData::timeline::Buffer) > BufferPairType;
    typedef ::boost::pool<> PoolType;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    EXTDATA_API BufferTL( ::fwData::Object::Key key );

    /// Destructor
    EXTDATA_API virtual ~BufferTL();

    /// Check if the type of an object is compatible with this timeline
    EXTDATA_API virtual bool isObjectValid(const CSPTR(::extData::timeline::Object) &obj) const = 0;

    /**
     * @brief Return the closest object to the given timestamp
     * @param timestamp timestamp used to find the closest object
     * @param direction direction to find the closest object (PAST, FUTURE, BOTH)
     */
    EXTDATA_API virtual CSPTR(::extData::timeline::Object) getClosestObject(
        ::fwCore::HiResClock::HiResClockType timestamp, DirectionType direction = BOTH) const;

    /// Return the object matching the specified timestamp, returns NULL if object is not found
    EXTDATA_API virtual CSPTR(::extData::timeline::Object) getObject(::fwCore::HiResClock::HiResClockType timestamp)
    const;

    /// Clear the timeline
    EXTDATA_API virtual void clearTimeline();

    /// Push a buffer to the timeline
    EXTDATA_API virtual void pushObject(const SPTR(::extData::timeline::Object) &obj);

    /// Return the last object in the timeline
    EXTDATA_API CSPTR(::extData::timeline::Object) getNewerObject() const;

    /// Return the last timestamp in the timeline
    EXTDATA_API ::fwCore::HiResClock::HiResClockType getNewerTimestamp() const;

    /// Change the maximum size of the timeline
    void setMaximumSize(size_t maximumSize) { m_maximumSize = maximumSize; }

    /// Default Timeline Size
    EXTDATA_API static const size_t s_DEFAULT_TIMELINE_MAX_SIZE;

protected:

    /// Allocate the pool buffer.
    EXTDATA_API void allocPoolSize(std::size_t size);

    /// Mutex to protect m_timeline and m_pool access
    mutable ::fwCore::mt::ReadWriteMutex m_tlMutex;

    ///Timeline
    TimelineType m_timeline;

    /// Pool of buffer
    SPTR(PoolType) m_pool;

    /// maximum size
    size_t m_maximumSize;

}; // class BufferTL

} // namespace extData


#endif // __EXTDATA_BUFFERTL_HPP__
