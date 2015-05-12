/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTEST_HELPER_THREAD_HPP__
#define __FWTEST_HELPER_THREAD_HPP__

#include <exception>

#include <boost/thread.hpp>
#include <boost/bind.hpp>
#include <boost/thread/thread_time.hpp>

#include <cppunit/Exception.h>

#include "fwTest/config.hpp"

namespace fwTest
{
namespace helper
{

/**
 * @brief   Helper to manage a thread. The thread launches a function and catches its exception.
 * @class   Thread
 *
 * @date    2012.
 *
 * @note    It is used for unit test.
 */
class FWTEST_CLASS_API Thread
{
public:

    typedef ::boost::function<void ()> FunctionType;

    /**
     * @brief Constructor : launch a thread which call run(f)
     * @param f function to be called in thread
     */
    FWTEST_API Thread(FunctionType f);

    /// Destructor
    FWTEST_API virtual ~Thread();

    /// Waits the thread execution to complete
    FWTEST_API void join(bool raise = true);

    /**
     * @brief Waits the thread of execution to complete
     * @param time time to waits for the thread stop.
     * @param raise if true raises an exception when it failed.
     * @return true if the thread of execution has completed before the call times out
     */
    FWTEST_API bool timedJoin(int time, bool raise = true);

    /// Return the exception raised in function launched by thread
    CPPUNIT_NS::Exception getException()
    {
        return m_exception;
    }

    /// Return true if function launched by thread raised exception
    bool hasFailed()
    {
        return m_hasFailed;
    }

    /**
     * @brief If any, throws the exception raised by the thread function.
     */
    void throwException();

private:

    /// Method launched by thread : call run(f) surrounded by try/catch
    void run(FunctionType f);

    /// Thread created in constructor
    ::boost::thread m_thread;

    /// Exception raised by function launched by thread
    CPPUNIT_NS::Exception m_exception;

    /// true if function launched by thread raised exception
    bool m_hasFailed;
};

} // namespace helper
} // namespace fwTest

#endif // __FWTEST_HELPER_THREAD_HPP__
