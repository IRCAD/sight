/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwTest/helper/Thread.hpp"

#include <chrono>
#include <functional>

namespace fwTest
{
namespace helper
{

//-----------------------------------------------------------------------------

Thread::Thread(FunctionType f) : m_hasFailed(false)
{
    m_thread = std::thread(std::bind(&Thread::run, this, f));
}

//-----------------------------------------------------------------------------

Thread::~Thread()
{
}

//-----------------------------------------------------------------------------

void Thread::join(bool raise)
{
    m_thread.join();
    if (raise)
    {
        this->throwException();
    }
}

//-----------------------------------------------------------------------------

bool Thread::timedJoin(int time, bool raise)
{
    bool joined = false;

    std::this_thread::sleep_for(std::chrono::milliseconds(time));
    if (m_thread.joinable())
    {
        m_thread.join();
        joined = true;
    }

    if (raise)
    {
        this->throwException();
    }
    return joined;
}

//-----------------------------------------------------------------------------

void Thread::throwException()
{
    if (this->hasFailed())
    {
        throw this->getException();
    }
}

//-----------------------------------------------------------------------------

void Thread::run(FunctionType f)
{
    try
    {
        f();
    }
    catch (CPPUNIT_NS::Exception &e)
    {
        m_exception = e;
        m_hasFailed = true;
    }
}

} // namespace helper
} // namespace fwTest


