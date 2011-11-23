/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATATOOLS_THREAD_REGIONTHREADER_HPP_
#define _FWDATATOOLS_THREAD_REGIONTHREADER_HPP_

#include <cstddef>

#include <boost/bind.hpp>
#include <boost/foreach.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/thread.hpp>

namespace fwDataTools
{

namespace thread
{



class RegionThreader
{

public:

    RegionThreader()
        : m_nbThread( (::boost::thread::hardware_concurrency() > 1) ? ::boost::thread::hardware_concurrency() : 1 )
    {}

    RegionThreader(size_t nbThread)
        : m_nbThread((nbThread > 1) ? nbThread : 1)
    {}

    template<typename T> void operator()(T func, const size_t dataSize)
    {
        std::vector< ::boost::thread* > threads;

        const size_t step = (dataSize / m_nbThread) + 1;
        size_t regionBegin = 0;
        size_t threadId = 0;

        if (m_nbThread > 1)
        {
            for (; regionBegin < dataSize; regionBegin += step, ++threadId)
            {
                threads.push_back(new ::boost::thread(func, regionBegin, std::min( dataSize,  regionBegin + step), threadId ));
            }

            BOOST_FOREACH( ::boost::thread *thread, threads)
            {
                thread->join();
                delete thread;
            }
            threads.clear();
        }
        else
        {
            func(0, dataSize, 0);
        }
    }

    size_t numberOfThread() { return m_nbThread; };

protected:

    const size_t m_nbThread;
};

}   // namespace thread

}   // namespace fwDataTools


#endif //_FWDATATOOLS_THREAD_REGIONTHREADER_HPP_

