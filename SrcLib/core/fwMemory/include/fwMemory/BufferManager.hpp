/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_BUFFERMANAGER_HPP_
#define _FWMEMORY_BUFFERMANAGER_HPP_

#include <boost/filesystem/path.hpp>



// See http://www.boost.org/doc/libs/1_47_0/doc/html/signals/s04.html
#ifndef SIGNALSLIB_HPP_INCLUDED
#define SIGNALSLIB_HPP_INCLUDED

#if defined(signals) && defined(QOBJECTDEFS_H) && \
  !defined(QT_MOC_CPP)
#  undef signals
#  define signals signals
#endif

#include <boost/signal.hpp>
namespace boost
{
  namespace signalslib = signals;
}

#if defined(signals) && defined(QOBJECTDEFS_H) && \
  !defined(QT_MOC_CPP)
#  undef signals
// Restore the macro definition of "signals", as it was
// defined by Qt's <qobjectdefs.h>.
#  define signals protected
#endif



#endif

#include <fwCore/base.hpp>

#include <fwTools/IBufferManager.hpp>

#include "fwMemory/BufferInfo.hpp"
#include "fwMemory/IPolicy.hpp"
#include "fwMemory/config.hpp"

namespace fwMemory
{

class FWMEMORY_CLASS_API BufferManager : public ::fwTools::IBufferManager
{

public:

    typedef ::fwTools::IBufferManager::SizeType SizeType;

    typedef ::boost::signal<void ()> UpdatedSignalType;
    typedef ::fwMemory::BufferInfo BufferInfo;
    typedef BufferInfo::MapType BufferInfoMapType;

    fwCoreClassDefinitionsWithFactoryMacro((BufferManager), (()), new BufferManager );
    fwCoreAllowSharedFromThis();


    FWMEMORY_API virtual bool registerBuffer(void ** buffer, long * lockCount);
    FWMEMORY_API virtual bool unregisterBuffer(void ** buffer);

    FWMEMORY_API virtual bool allocateBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool setBuffer(void ** buffer, SizeType size, ::fwTools::BufferAllocationPolicy::sptr policy);
    FWMEMORY_API virtual bool reallocateBuffer(void ** buffer, SizeType newSize);
    FWMEMORY_API virtual bool destroyBuffer(void ** buffer);

    FWMEMORY_API virtual bool lockBuffer(const void * const * buffer);
    FWMEMORY_API virtual bool unlockBuffer(const void * const * buffer);

    FWMEMORY_API virtual std::string toString() const;


    FWMEMORY_API bool dumpBuffer(const void * const *  buffer);
    FWMEMORY_API bool restoreBuffer(const void * const *  buffer);


    FWMEMORY_API bool writeBuffer(const void * buffer, SizeType size, ::boost::filesystem::path &path);
    FWMEMORY_API bool readBuffer(void * buffer, SizeType size, ::boost::filesystem::path &path);


    FWMEMORY_API UpdatedSignalType &getUpdatedSignal(){return m_updated;};


    const BufferInfoMapType & getBufferInfos() const
    {
        return m_bufferInfos;
    }

    FWMEMORY_API void setDumpPolicy( ::fwMemory::IPolicy::sptr policy );

protected:

    FWMEMORY_API BufferManager();
    FWMEMORY_API virtual ~BufferManager();

    FWMEMORY_API bool dumpBuffer(BufferInfo & info, void ** buffer);
    FWMEMORY_API bool restoreBuffer(BufferInfo & info, void ** buffer, SizeType size = 0);


    UpdatedSignalType m_updated;

    ::fwCore::LogicStamp m_lastAccess;
    BufferInfoMapType m_bufferInfos;

    ::fwMemory::IPolicy::sptr m_dumpPolicy;
};


}

#endif /* _FWMEMORY_BUFFERMANAGER_HPP_ */
