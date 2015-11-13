/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATATOOLS_THREAD_REGIONTHREADER_HPP__
#define __FWDATATOOLS_THREAD_REGIONTHREADER_HPP__

#include <algorithm>
#include <cstddef>
#include <limits>
#include <thread>

namespace fwDataTools
{

namespace thread
{


class RegionThreader
{

public:

    RegionThreader()
        : m_nbThread( (std::thread::hardware_concurrency() > 1) ? std::thread::hardware_concurrency() : 1 )
    {
    }

    RegionThreader(size_t nbThread, bool capped = true)
        : m_nbThread( std::min( capped ? std::thread::hardware_concurrency() : std::numeric_limits<size_t>::max(),
                                (nbThread > 1) ? nbThread : 1) )
    {
    }

    template<typename T> void operator()(T func, const size_t dataSize)
    {
        std::vector< std::thread* > threads;

        const size_t step  = (dataSize / m_nbThread) + 1;
        size_t regionBegin = 0;
        size_t threadId    = 0;

        if (m_nbThread > 1)
        {
            for (; regionBegin < dataSize; regionBegin += step, ++threadId)
            {
                threads.push_back(new std::thread(func, regionBegin, std::min( dataSize,  regionBegin + step),
                                                  threadId ));
            }

            for( std::thread *thread: threads)
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

    size_t numberOfThread()
    {
        return m_nbThread;
    }

protected:

    const size_t m_nbThread;
};

}   // namespace thread

}   // namespace fwDataTools


#endif //__FWDATATOOLS_THREAD_REGIONTHREADER_HPP__

