/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWCORE_TIMESTAMP_HPP
#define __FWCORE_TIMESTAMP_HPP

#include "fwCore/base.hpp"
#include "fwCore/HiResClock.hpp"

namespace fwCore
{

/**
 * @brief Provides a timestamp and a expiracy system.
 *
 */
class FWCORE_CLASS_API TimeStamp : public BaseObject
{

public:
        fwCoreClassDefinitionsWithFactoryMacro( (TimeStamp)(BaseObject::Baseclass), (()), new TimeStamp) ;

    /**
     * @brief Type used in logical typestamp.
     */
    typedef ::fwCore::HiResClock::HiResClockType TimeStampType;



    /**
     * @name Constructor/Destructor
     * @{ */

    FWCORE_API TimeStamp()
    {
        this->m_modifiedTime = 0;
        this->m_lifePeriod   = 0;
    };

    /**  @} */


    /**
     * @brief Update the timestamp to the current EPOCH time
     */
    FWCORE_API void modified();


    /**
     * @return TimeStamp's current value
     */
    FWCORE_API TimeStampType getTimeStamp() const
    {
        return this->m_modifiedTime;
    };



    /**
     * @brief Greater than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    FWCORE_API bool operator>(TimeStamp& ts)
    {
        return ( this->m_modifiedTime > ts.m_modifiedTime );
    };

    /**
     * @brief Lesser than operator for TimeStamp.
     *
     * @param ts TimeStamp to compare to
     */
    FWCORE_API bool operator<(TimeStamp& ts)
    {
        return ( this->m_modifiedTime < ts.m_modifiedTime );
    };

    /**
     * @brief Cast operator for TimeStamp.
     */
    FWCORE_API operator TimeStampType() const
    {
        return this->m_modifiedTime;
    };


    /**
     * @brief Setter for the life period
     *
     * @param period Period in miliseconds
     */
    FWCORE_API void setLifePeriod(TimeStampType period)
    {
        m_lifePeriod = period;
    };

    /**
     * @brief Getter for the life period
     *
     * @return TimeStamp life period
     */
    FWCORE_API TimeStampType getLifePeriod()
    {
        return m_lifePeriod;
    };


    /** 
     * @brief Check TimeStamp expiracy status
     * 
     * @return True if more than m_lifePeriod miliseconds are elapsed from
     * TimeStamp value
     */
    FWCORE_API bool periodExpired()
    {
        return (::fwCore::HiResClock::getTimeInMilliSec() - this->m_modifiedTime) > m_lifePeriod;
    }


private:
    /** 
     * @brief TimeStamp value (miliseconds)
     */
    TimeStampType m_modifiedTime;

    /** 
     * @brief life period (miliseconds)
     */
    TimeStampType m_lifePeriod;

};


} //namespace fwCore

#endif // __FWCORE_TIMESTAMP_HPP
