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
#include <fwCore/LogicStamp.hpp>

#include <fwTools/IBufferManager.hpp>

#include "fwMemory/config.hpp"

namespace fwMemory
{

class FWMEMORY_CLASS_API BufferManager : public ::fwTools::IBufferManager
{

public:

    typedef ::fwTools::IBufferManager::SizeType SizeType;

    typedef ::boost::signal<void ()> UpdatedSignalType;

    struct FWMEMORY_CLASS_API DumpedBufferInfo
    {
        FWMEMORY_API DumpedBufferInfo()
        {
            this->size = 0;
            this->lockCount = NULL;
            this->isDumped = false;
            this->lastAccess.modified();
        }

        SizeType size;
        bool     isDumped;
        ::boost::filesystem::path dumpedFile;
        long * lockCount;
        ::fwCore::LogicStamp lastAccess;
        ::fwTools::BufferAllocationPolicy::sptr bufferPolicy;
    };

    typedef std::map< void **,  DumpedBufferInfo > DumpedBufferInfoMapType;

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

    FWMEMORY_API bool dumpBuffer(void ** buffer);
    FWMEMORY_API bool restoreBuffer(void ** buffer, SizeType size = 0);

    FWMEMORY_API bool writeBuffer(const void * buffer, SizeType size, ::boost::filesystem::path &path);
    FWMEMORY_API bool readBuffer(void * buffer, SizeType size, ::boost::filesystem::path &path);


    FWMEMORY_API UpdatedSignalType &getUpdatedSignal(){return m_updated;};


    const DumpedBufferInfoMapType& getDumpedBufferInfoMap() const
    {
        return m_dumpedBufferInfos;
    }

protected:

    FWMEMORY_API BufferManager();
    FWMEMORY_API virtual ~BufferManager();


    UpdatedSignalType m_updated;

    ::fwCore::LogicStamp m_lastAccess;
    DumpedBufferInfoMapType m_dumpedBufferInfos;
};


}

#endif /* _FWMEMORY_BUFFERMANAGER_HPP_ */
