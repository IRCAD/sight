/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#include <boost/assign.hpp>
#include <boost/foreach.hpp>


#ifdef _WIN32
#define MEMORYTOOLIMPL Win32MemoryMonitorTools
#include "fwMemory/tools/Win32MemoryMonitorTools.hpp"
#elif defined(__MACOSX__)
#define MEMORYTOOLIMPL DarwinMemoryMonitorTools
#include "fwMemory/tools/DarwinMemoryMonitorTools.hpp"
#else
#define MEMORYTOOLIMPL PosixMemoryMonitorTools
#include "fwMemory/tools/PosixMemoryMonitorTools.hpp"
#endif

#include <fwTools/ByteSize.hpp>


#include "fwMemory/policy/ValveDump.hpp"


namespace fwMemory
{

namespace policy
{

static IPolicy::Register<ValveDump> registerFactory(ValveDump::leafClassname());

//------------------------------------------------------------------------------

ValveDump::ValveDump() :
    m_minFreeMem(1024*1024*500LL),
    m_hysteresisOffset(0)
{

}

//------------------------------------------------------------------------------

void ValveDump::allocationRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply((size > info.size) ? size - info.size : 0);
}

//------------------------------------------------------------------------------


void ValveDump::setRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType size )
{
    this->apply();
}

//------------------------------------------------------------------------------


void ValveDump::reallocateRequest( BufferInfo &info, void **buffer, BufferInfo::SizeType newSize )
{
    this->apply((newSize > info.size) ? newSize - info.size : 0);
}

//------------------------------------------------------------------------------


void ValveDump::destroyRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void ValveDump::lockRequest( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void ValveDump::unlockRequest( BufferInfo &info, void **buffer )
{
    this->apply();
}

//------------------------------------------------------------------------------


void ValveDump::dumpSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------


void ValveDump::restoreSuccess( BufferInfo &info, void **buffer )
{
}

//------------------------------------------------------------------------------



void ValveDump::setManager(::fwTools::IBufferManager::sptr manager)
{
    m_manager = ::fwMemory::BufferManager::dynamicCast(manager);
}

//------------------------------------------------------------------------------

bool ValveDump::needDump(size_t supplement)
{
    return ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() <= (m_minFreeMem + supplement);
}

//------------------------------------------------------------------------------

size_t ValveDump::dump(size_t nbOfBytes)
{
    size_t dumped = 0;

    ::fwMemory::BufferManager::sptr manager = m_manager.lock();
    if(manager)
    {

        const fwMemory::BufferInfo::MapType &bufferInfos = manager->getBufferInfos();

        typedef std::pair<
            fwMemory::BufferInfo::MapType::key_type,
            fwMemory::BufferInfo::MapType::mapped_type
                > BufferInfosPairType;
        typedef std::vector< BufferInfosPairType > BufferVectorType;

        BufferVectorType buffers;

        BOOST_FOREACH(const ::fwMemory::BufferInfo::MapType::value_type &elt, bufferInfos)
        {
            const ::fwMemory::BufferInfo &info = elt.second;
            if( ! ( info.size == 0 || info.lockCount() > 0 || info.isDumped )  )
            {
                buffers.push_back(elt);
            }
        }


        BOOST_FOREACH(const BufferVectorType::value_type &pair, bufferInfos)
        {
            if(dumped < nbOfBytes)
            {
                if( manager->dumpBuffer(pair.first) )
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

void ValveDump::apply(size_t supplement)
{
    if(this->needDump(supplement))
    {
        this->dump( (m_minFreeMem + m_hysteresisOffset + supplement) - ::fwMemory::tools::MEMORYTOOLIMPL::getFreeSystemMemory() );
    }
}

//------------------------------------------------------------------------------

void ValveDump::refresh()
{
    this->apply();
}

//------------------------------------------------------------------------------

bool ValveDump::setParam(const std::string &name, const std::string &value)
{
    OSLM_INFO("Set " << name << " to " << value);
    try
    {
        if(name == "min_free_mem")
        {
            m_minFreeMem = ::fwTools::ByteSize(value).getSize();
            return true;

        }
        else if(name == "hysteresis_offet")
        {
            m_hysteresisOffset = ::fwTools::ByteSize(value).getSize();
            return true;
        }
    }
    catch( ::fwTools::ByteSize::Exception const& )
    {
        OSLM_ERROR("Bad value for " << name << " : " << value);
        return false;
    }
    OSLM_ERROR("Bad parameter name " << name );
    return false;
}

//------------------------------------------------------------------------------

const fwMemory::IPolicy::ParamNamesType &ValveDump::getParamNames() const
{
    static const fwMemory::IPolicy::ParamNamesType params
        = ::boost::assign::list_of ("min_free_mem")
                                   ("hysteresis_offet");
    return params;
}

//------------------------------------------------------------------------------

std::string ValveDump::getParam(const std::string &name, bool *ok )
{
    bool isOk;
    std::string value;
    if (NULL == ok)
    {
        ok = &isOk;
    }
    *ok = false;
    if(name == "min_free_mem")
    {
        value = std::string(::fwTools::ByteSize( ::fwTools::ByteSize::SizeType(m_minFreeMem) ));
        *ok = true;

    }
    else if(name == "hysteresis_offet")
    {
        value = std::string(::fwTools::ByteSize( ::fwTools::ByteSize::SizeType(m_hysteresisOffset) ));
        *ok = true;
    }
    return value;
}

//------------------------------------------------------------------------------


} // namespace policy

} //namespace fwMemory
