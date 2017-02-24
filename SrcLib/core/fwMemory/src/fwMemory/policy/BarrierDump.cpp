/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMemory/exception/BadCast.hpp"
#include "fwMemory/ByteSize.hpp"
#include "fwMemory/policy/registry/macros.hpp"
#include "fwMemory/policy/BarrierDump.hpp"

#include <boost/assign.hpp>

namespace fwMemory
{

namespace policy
{

fwMemoryPolicyRegisterMacro(::fwMemory::policy::BarrierDump);

//------------------------------------------------------------------------------

BarrierDump::BarrierDump() :
    m_totalAllocated(0),
    m_totalDumped(0),
    m_barrier(1024*1024*500)
{

}

//------------------------------------------------------------------------------

void BarrierDump::allocationRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                     BufferInfo::SizeType size )
{
    SLM_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(!info.loaded)
    {
        SLM_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::setRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                              BufferInfo::SizeType size )
{
    SLM_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += size;
    if(!info.loaded)
    {
        SLM_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::reallocateRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer,
                                     BufferInfo::SizeType newSize )
{
    SLM_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
    m_totalAllocated += newSize;
    if(!info.loaded)
    {
        SLM_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }
    this->apply();
}

//------------------------------------------------------------------------------


void BarrierDump::destroyRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    if(!info.loaded)
    {
        SLM_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
        m_totalDumped -= info.size;
    }
    SLM_ASSERT("Memory allocation inconsistency", m_totalAllocated >= info.size);
    m_totalAllocated -= info.size;
}

//------------------------------------------------------------------------------


void BarrierDump::lockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
}

//------------------------------------------------------------------------------


void BarrierDump::unlockRequest( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------

void BarrierDump::dumpSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    m_totalDumped += info.size;
}

//------------------------------------------------------------------------------

void BarrierDump::restoreSuccess( BufferInfo &info, ::fwMemory::BufferManager::ConstBufferPtrType buffer )
{
    SLM_ASSERT("Memory dump inconsistency", m_totalDumped >= info.size);
    m_totalDumped -= info.size;
}

//------------------------------------------------------------------------------

size_t BarrierDump::getTotalAlive() const
{
    SLM_ASSERT("More dumped data than allocated data.", m_totalAllocated >= m_totalDumped);
    size_t totalAlive = m_totalAllocated - m_totalDumped;
    return totalAlive;
}

//------------------------------------------------------------------------------

bool BarrierDump::isBarrierCrossed() const
{
    return m_barrier < getTotalAlive();
}

//------------------------------------------------------------------------------

size_t BarrierDump::dump(size_t nbOfBytes)
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = ::fwMemory::BufferManager::getDefault();
    if(manager)
    {
        const ::fwMemory::BufferManager::BufferInfoMapType bufferInfos = manager->getBufferInfos().get();

        typedef std::pair<
                ::fwMemory::BufferManager::BufferInfoMapType::key_type,
                ::fwMemory::BufferManager::BufferInfoMapType::mapped_type
                > BufferInfosPairType;
        typedef std::vector< BufferInfosPairType > BufferVectorType;

        BufferVectorType buffers;

        for(const ::fwMemory::BufferManager::BufferInfoMapType::value_type &elt :  bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( !( info.size == 0 || info.lockCount() > 0 || !info.loaded )  )
            {
                buffers.push_back(elt);
            }
        }


        for(const BufferVectorType::value_type &pair :  bufferInfos)
        {
            if(dumped < nbOfBytes)
            {
                if( manager->dumpBuffer(pair.first).get() )
                {
                    dumped += pair.second.size;
                }
            }
            else
            {
                break;
            }
        }
    }

    return dumped;
}

//------------------------------------------------------------------------------

void BarrierDump::apply()
{
    if(this->isBarrierCrossed())
    {
        this->dump(this->getTotalAlive() - m_barrier);
    }
}

//------------------------------------------------------------------------------

void BarrierDump::refresh()
{
    ::fwMemory::BufferManager::sptr manager      = ::fwMemory::BufferManager::getDefault();
    ::fwMemory::BufferManager::BufferStats stats = manager->getBufferStats().get();
    m_totalAllocated                             = stats.totalManaged;
    m_totalDumped                                = stats.totalDumped;
    this->apply();
}

//------------------------------------------------------------------------------

bool BarrierDump::setParam(const std::string &name, const std::string &value)
{
    try
    {
        if(name == "barrier")
        {
            m_barrier = ::fwMemory::ByteSize(value).getSize();
            return true;
        }
    }
    catch( ::fwMemory::exception::BadCast const& )
    {
        OSLM_ERROR("Bad value for " << name << " : " << value);
        return false;
    }

    OSLM_ERROR("Bad parameter name " << name );
    return false;
}

//------------------------------------------------------------------------------

const ::fwMemory::IPolicy::ParamNamesType &BarrierDump::getParamNames() const
{
    static const ::fwMemory::IPolicy::ParamNamesType params = ::boost::assign::list_of("barrier");
    return params;
}

//------------------------------------------------------------------------------

std::string BarrierDump::getParam(const std::string &name, bool *ok ) const
{
    bool isOk = false;
    std::string value;
    if(name == "barrier")
    {
        value = std::string(::fwMemory::ByteSize( ::fwMemory::ByteSize::SizeType(m_barrier) ));
        isOk  = true;
    }
    if (ok)
    {
        *ok = isOk;
    }
    return value;
}

//------------------------------------------------------------------------------



} // namespace policy

} //namespace fwMemory
