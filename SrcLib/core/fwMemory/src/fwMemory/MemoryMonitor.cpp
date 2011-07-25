/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <boost/foreach.hpp>
#include "fwMemory/MemoryMonitor.hpp"
#include "fwMemory/policy/NeverDumpPolicy.hpp"
#include "fwMemory/policy/LimitedMemoryDumpPolicy.hpp"

namespace fwMemory
{

::boost::shared_ptr<MemoryMonitor> MemoryMonitor::m_instance;

//------------------------------------------------------------------------------

const ::boost::shared_ptr< MemoryMonitor > MemoryMonitor::getDefault() throw()
{
    SLM_TRACE_FUNC();
    if( m_instance.get() == 0 )
    {
        SLM_DEBUG("MemoryMonitor::getDefault() >> New Class Instance");
        m_instance = ::boost::shared_ptr<MemoryMonitor>(new MemoryMonitor());
    }
    return m_instance;
}

//------------------------------------------------------------------------------

void MemoryMonitor::setPolicy( ::boost::shared_ptr< ::fwMemory::IDumpPolicy > _policy ) throw()
{
    SLM_TRACE_FUNC();
    getDefault()->m_policy = _policy ;
    getDefault()->m_policy->apply();
}


//------------------------------------------------------------------------------

MemoryMonitor::MemoryMonitor() : m_policy( new ::fwMemory::policy::NeverDumpPolicy )
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

MemoryMonitor::~MemoryMonitor()
{
    SLM_TRACE_FUNC();
    OSLM_DEBUG(" MemoryMonitor::~MemoryMonitor() : m_statistics.size()= " << m_statistics.size() );
    m_statistics.clear();

    // BE CARREFULL : I need to reset m_instance == 0 because getDefault() is applied after the singleton destruction
    m_instance = ::boost::shared_ptr<MemoryMonitor>();
}

//------------------------------------------------------------------------------

const MemoryMonitor::StatisticsContainer  & MemoryMonitor::getStatistics() throw()
{
    SLM_TRACE_FUNC();
    getDefault()->clean();
    return getDefault()->m_statistics;
}

//------------------------------------------------------------------------------

void MemoryMonitor::registerObject( ::fwTools::Object::wptr newObject ) throw()
{
    SLM_TRACE_FUNC();
    assert( !newObject.expired() );

    OSLM_DEBUG( "MemoryMonitor::Register object " << newObject.lock()->className() << " " << newObject.lock().get() );
    if ( findObject(newObject.lock()) !=  m_statistics.end() )
    {
        OSLM_DEBUG("MemoryMonitor::Register object " << newObject.lock()->className() << " " << newObject.lock().get() << " Already registered");
        return;
    }

    MemoryInfo memInfo; // default value ?
    m_statistics[ newObject ] = memInfo;
}

//------------------------------------------------------------------------------

void MemoryMonitor::unregisterObject( ::fwTools::Object::wptr obj ) throw()
{
    SLM_TRACE_FUNC();
    std::stringstream ss;
    if ( obj.expired() )
    {
        ss << "MemoryMonitor::Unregister an expired object";
        this->clean();
        return;
    }

    ss << "MemoryMonitor::Unregister object" << obj.lock()->className() << " " << obj.lock().get();

    StatisticsContainer::iterator iter = findObject(obj.lock());
    if ( iter != m_statistics.end() )
    {
        OSLM_DEBUG( ss.str() << " is OK" );
        m_statistics.erase(iter);
    }
    else
    {
        OSLM_DEBUG( ss.str() << " NOK NOT PREVIOUSLY REGISTRED" );
    }
}

//------------------------------------------------------------------------------

void  MemoryMonitor::clean() throw()
{
    SLM_TRACE_FUNC();
    OSLM_DEBUG(" MemoryMonitor::clean() : m_statistics.size() before " << m_statistics.size() );

    StatisticsContainer::iterator iter = m_statistics.begin();
    while ( iter !=  m_statistics.end() )
    {
        if ( iter->first.expired() )
        {
            m_statistics.erase(iter);
            iter = m_statistics.begin();
        }
        else
        {
            ++iter;
        }
    }
    //          ++iter )

    OSLM_DEBUG(" MemoryMonitor::clean() : m_statistics.size() after " << m_statistics.size() );
}

//------------------------------------------------------------------------------

void MemoryMonitor::statusChanged( ::fwTools::Object::wptr obj, MemoryInfo &memInfo ) throw()
{
    SLM_TRACE_FUNC();
    assert( !obj.expired() );
    StatisticsContainer::iterator iter = findObject(obj.lock());
    if ( iter != m_statistics.end() )
    {
        OSLM_DEBUG("MemoryMonitor::statusChanged object" << obj.lock()->className() << " " << obj.lock().get() << " is OK" );

        MemoryInfo oldMemInfo = iter->second;

        iter->second = memInfo;

        if (    oldMemInfo.virtualSizeInBytes != memInfo.virtualSizeInBytes ||
                oldMemInfo.currentSizeInBytes != memInfo.currentSizeInBytes ||
                memInfo.dumpLockCount == 0 )
        {
            m_policy->apply() ;
        }
    }
    else
    {
        OSLM_DEBUG("MemoryMonitor::statusChanged object" << obj.lock()->className() << " " << obj.lock().get() << " NOK !!! object not registred" );
    }
}

//------------------------------------------------------------------------------

const MemoryMonitor::MemoryInfo  MemoryMonitor::getStatus( ::fwTools::Object::wptr obj ) const throw()
{
    SLM_TRACE_FUNC();
    assert( !obj.expired() );

    StatisticsContainer::const_iterator iter = m_statistics.find(obj);
    if ( iter != m_statistics.end() )
    {
        //OSLM_DEBUG("MemoryMonitor::getStatus object" << obj.lock()->className() << " " << obj.lock().get() << " is OK" );
        return (*iter).second;
    }
    else
    {
        OSLM_DEBUG("MemoryMonitor::getStatus object" << obj.lock()->className() << " " << obj.lock().get() << " NOK !!! object not registered" );
        return MemoryInfo();
    }
}

//------------------------------------------------------------------------------

MemoryMonitor::StatisticsContainer::iterator
MemoryMonitor::findObject( ::fwTools::Object::sptr obj )
{
    SLM_TRACE_FUNC();
    for (   StatisticsContainer::iterator iter= MemoryMonitor::getDefault()->m_statistics.begin();
            iter !=  MemoryMonitor::getDefault()->m_statistics.end();
            ++iter )
    {
        if( ! (*iter).first.expired() )
        {
            if ( (*iter).first.lock() == obj )
            {
                return iter;
            }
        }
    }

    return  m_statistics.end();
}

//------------------------------------------------------------------------------

::boost::uint64_t MemoryMonitor::totalUsedSizeInBytes()
{
    SLM_TRACE_FUNC();
    ::boost::uint64_t result=0;

    for (   StatisticsContainer::iterator iter = m_statistics.begin();
            iter !=  m_statistics.end();
            ++iter )
    {
        if ( ! iter->first.expired() )
        {
            result += (*iter).second.currentSizeInBytes;
        }
    }

    OSLM_DEBUG( "MemoryMonitor::totalUsedSizeInBytes=" << result);
    return result;
}

//------------------------------------------------------------------------------

bool  MemoryMonitor::reserveMemory(::boost::uint64_t nbBytes)
{
    SLM_TRACE_FUNC();
    return m_policy->apply(nbBytes);
}

//------------------------------------------------------------------------------

MemoryMonitor::StatisticsContainer::iterator
MemoryMonitor::findCandidate()
{
    SLM_TRACE_FUNC();
    MemoryMonitor::StatisticsContainer::iterator result=m_statistics.end();

    for (StatisticsContainer::iterator iter= m_statistics.begin() ; iter !=  m_statistics.end(); ++iter )
    {
        if ( ! iter->first.expired() )
        {
            MemoryMonitor::StatisticsContainer::iterator candidate = iter;
            bool candidateisRestored = ( iter->second.currentSizeInBytes == iter->second.virtualSizeInBytes );
            bool candidateisLockFree = ( iter->second.dumpLockCount == 0 );
            bool candidateisWellInitialized = ( iter->second.virtualSizeInBytes != 0 );
            if ( candidateisRestored  && candidateisWellInitialized && candidateisLockFree
                             && ( result==m_statistics.end() ||  ( candidate->second.lastAccessTime < result->second.lastAccessTime ) )
                )
            {
                 result = candidate;
            }
        }
    }
    return result;
}

//------------------------------------------------------------------------------

std::string MemoryMonitor::log() throw()
{
    StatisticsContainer::iterator iter;
    std::stringstream msg ;
    for (   iter = m_statistics.begin();
            iter!= m_statistics.end();
            ++iter )
    {
        if( ! iter->first.expired() )
        {
            ::fwTools::Object::sptr obj( (*iter).first );
            msg << (*iter).first.lock()->className() << "(" << obj.get();
            msg << ") virtualMemSize=" << iter->second.virtualSizeInBytes;
            msg << " Bytes. currentMemSize=" << iter->second.currentSizeInBytes;
            msg << " Bytes. TimeStamp = " <<  iter->second.lastAccessTime ;
            msg << " Bytes. dumpLockCount = " <<  iter->second.dumpLockCount ;
            msg  <<  std::endl;
        }
    }
    return msg.str();
}

//------------------------------------------------------------------------------

}
