/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCORE_HIRESTIMER_HPP_
#define _FWCORE_HIRESTIMER_HPP_

#ifdef WIN32
#include <windows.h>
#else
#include <sys/time.h>
#endif

#include <fwCore/BaseObject.hpp>
#include "fwCore/HiResClock.hpp"
#include <fwCore/config.hpp>

namespace fwCore
{

/** 
 * @brief This class provide a timer (stopwatch).
 * HiResTimer is able to measure the elapsed time with a few micro-seconds.
 * accuracy, on Linux, Mac OS and Windows
 */
class FWCORE_CLASS_API HiResTimer : public BaseObject
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (HiResTimer)(BaseObject), (()), new HiResTimer) ;

    /** 
     * @name Constructor/Destructor
     * @{ */

    FWCORE_API HiResTimer();
    FWCORE_API virtual ~HiResTimer();

    /**  @} */

    /** 
     * @brief Start the timer.
     */
    FWCORE_API void   start();

    /** 
     * @brief Stop the timer.
     * stop() will not reset the timer.
     */
    FWCORE_API void   stop();

    /** 
     * @brief Reset the timer and leave it in the same state it was (started or stopped).
     * 
     * @param initial_value Initial value in microseconds from which the timer
     * will start to count.
     */
    FWCORE_API void   reset(::fwCore::HiResClock::HiResClockType initial_value=0.);


    /** 
     * @name Elapsed time
     * @brief These methods return the cumulated elapsed running time of the timer
     * the timer is running when start() has been invoked and stop running on 
     * stop() or reset() invokation.
     * @{ */

    /** 
     * @return Elapsed time in seconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInSec();

    /** 
     * @return Elapsed time in miliseconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInMilliSec();

    /** 
     * @return Elapsed time in microseconds
     */
    FWCORE_API ::fwCore::HiResClock::HiResClockType getElapsedTimeInMicroSec();

    /**  @} */
protected:


private:
    /** 
     * @brief Last start time.
     */
    ::fwCore::HiResClock::HiResClockType m_startTimeInMicroSec;
    /** 
     * @brief Last "break" time.
     */
    ::fwCore::HiResClock::HiResClockType m_endTimeInMicroSec;
    /** 
     * @brief cummulated running time.
     */
    ::fwCore::HiResClock::HiResClockType m_cumulTimeInMicroSec;
    /** 
     * @brief timer status.
     */
    bool   m_stopped;
};

} //namespace fwCore

#endif // _FWCORE_HIRESTIMER_HPP_
