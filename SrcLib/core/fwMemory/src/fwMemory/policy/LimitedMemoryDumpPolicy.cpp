/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <set>
#include <boost/lexical_cast.hpp>

#include <fwCore/base.hpp>
#include <fwTools/ClassRegistrar.hpp>
#include <fwServices/Base.hpp>

#include "fwMemory/IDumpService.hpp"
#include "fwMemory/policy/LimitedMemoryDumpPolicy.hpp"
#include "fwMemory/MemoryMonitor.hpp"
#include "fwMemory/tools/MemoryMonitorTools.hpp"


REGISTER_BINDING( ::fwMemory::IDumpPolicy, ::fwMemory::policy::LimitedMemoryDumpPolicy , std::string , "LimitedMemoryDumpPolicy"  );

namespace fwMemory
{
namespace policy
{

//-----------------------------------------------------------------------------

LimitedMemoryDumpPolicy::LimitedMemoryDumpPolicy() :
            m_threshold( 100 * 1024 * 1024 ) // 100 mo
{
    OSLM_TRACE("LimitedMemoryDumpPolicy constructor threshold setted to (Mo)=" << m_threshold/1024/1024) ;
}
//-----------------------------------------------------------------------------

LimitedMemoryDumpPolicy::~LimitedMemoryDumpPolicy()
{
    SLM_TRACE_FUNC() ;
}

//-----------------------------------------------------------------------------

void LimitedMemoryDumpPolicy::setThreshold(::boost::uint64_t threshold)
{
    m_threshold = threshold;
    OSLM_INFO(" LimitedMemoryDumpPolicy::setThreshold(Bytes) =" <<  m_threshold);
}

//-----------------------------------------------------------------------------

void LimitedMemoryDumpPolicy::configure( ::fwRuntime::ConfigurationElement::sptr  cfgElt)
{
    SLM_TRACE_FUNC();
    if( cfgElt->hasAttribute("threshold") )
    {
        std::string threshold = cfgElt->getExistingAttributeValue("threshold") ;
        this->setThreshold(::boost::lexical_cast< ::boost::uint64_t >(threshold)*1024*1024 ); // unit in xml are Mo
        // unit LimitedMemoryDumpPolicy::m_threshold in Bytes
    }
}

//-----------------------------------------------------------------------------

bool LimitedMemoryDumpPolicy::apply(::boost::uint64_t reservedMemory)
{
    static bool lock=false;
    bool isReservationOk = false ;

    if (lock)
    {
        SLM_INFO(" LimitedMemoryDumpPolicy::apply() locked");
        return isReservationOk;
    }
    lock=true;
    ::boost::uint64_t currentFreeMem = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem() ;
    OSLM_DEBUG("LimitedMemoryDumpPolicy::apply => Current Free Mem = " << currentFreeMem );

    OSLM_DEBUG("LimitedMemoryDumpPolicy::apply => LOG " << MemoryMonitor::getDefault()->log() );

    OSLM_INFO(  "LimitedMemoryDumpPolicy::apply() : m_threshold=" << m_threshold
            <<" extra memory required " << reservedMemory << " current RAM(MemMonitor) used="
            << MemoryMonitor::getDefault()->totalUsedSizeInBytes()
            << " estimated free Memory " << currentFreeMem
            );

    std::set< ::fwTools::Object::sptr > dumpedObject;

    while ( ( reservedMemory > currentFreeMem ) || ( currentFreeMem - reservedMemory < m_threshold ) )
    {
        OSLM_DEBUG( "LimitedMemoryDumpPolicy::apply => totalUsedSizeInBytes == " << MemoryMonitor::getDefault()->totalUsedSizeInBytes() );
        MemoryMonitor::StatisticsContainer::iterator toDump = MemoryMonitor::getDefault()->findCandidate();
#ifndef __MACOSX__
        SLM_ASSERT("StatisticsContainer is empty", MemoryMonitor::getDefault()->getStatistics().size() );
#endif
        if ( toDump == MemoryMonitor::getDefault()->getStatistics().end() )
        {
            SLM_WARN("LimitedMemoryDumpPolicy unable to find a candidate to dump: POLICY ABORDED");
            lock=false;
            return isReservationOk;
        }

        ::fwTools::Object::sptr objToDump = (*toDump).first.lock();

        // detection of dump loop
        if ( !dumpedObject.insert(objToDump).second )
        {
            SLM_WARN("LimitedMemoryDumpPolicy dump loop detected : POLICY ABORDED");
            lock=false;
            return isReservationOk;
        }

        OSLM_INFO("LimitedMemoryDumpPolicy request dump of : " << objToDump->className() << "(" << objToDump << ")") ;
        OSLM_ASSERT(objToDump->className() << " has no IDumpService", ::fwServices::OSR::has(objToDump, "::fwMemory::IDumpService") ) ;
        ::fwServices::get< IDumpService >( objToDump )->dump() ;

        // Updating the free memory variable
        currentFreeMem = ::fwMemory::tools::MemoryMonitorTools::estimateFreeMem() ;
    }
    lock = false;
    isReservationOk = true ;
    return isReservationOk ;
}

//-----------------------------------------------------------------------------


} //namespace policy
} //namespace fwMemory
