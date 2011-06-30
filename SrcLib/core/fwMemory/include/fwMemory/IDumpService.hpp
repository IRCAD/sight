/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWMEMORY_IDUMPSERVICE_HPP_
#define _FWMEMORY_IDUMPSERVICE_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include <fwTools/Failed.hpp>
#include <fwServices/IService.hpp>

#include "fwMemory/config.hpp"

namespace fwData
{
    class Object;
}

namespace fwMemory
{

class FWMEMORY_CLASS_API IDumpService : public ::fwServices::IService
{
public:

    fwCoreServiceClassDefinitionsMacro( (IDumpService)( ::fwServices::IService) )

    typedef enum { NOTSET,
                   DUMPED,
                   RESTORED,
                   DUMPING, // Buffer is valid
                   RESTORING // Buffer is not valid
    } DumpStatus;

    FWMEMORY_API IDumpService();

    FWMEMORY_API virtual ~IDumpService();


    /// ask require to perform dump : return true iff dump action is really performed
    FWMEMORY_API virtual void dump()= 0;

    /// ask require to perform restore : return true iff restore action is really performed
    FWMEMORY_API virtual void restore() =0;

    FWMEMORY_API DumpStatus getStatus();

    // use semaphore,
    FWMEMORY_API virtual void setDumpIsLocked( bool _bDumpIsLocked );

    FWMEMORY_API virtual bool getDumpIsLocked() const;


    FWMEMORY_API static const ::boost::filesystem::path  & getDumpFolder();

    FWMEMORY_API static void setDumpFolder(::boost::filesystem::path );

    /// return the real state of RAM used
    FWMEMORY_API virtual ::boost::uint64_t  getCurrentSize() = 0;

    FWMEMORY_API unsigned long  getDumpLockCount() const;


protected :

    FWMEMORY_API virtual void starting() throw(::fwTools::Failed) {} ;
    FWMEMORY_API virtual void stopping() throw(::fwTools::Failed) {} ;
    FWMEMORY_API virtual void configuring() throw(::fwTools::Failed) {} ;
    FWMEMORY_API virtual void updating() throw(::fwTools::Failed) {} ;
    FWMEMORY_API virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;
    FWMEMORY_API virtual void info(std::ostream &_sstream );

    /// increase semaphore lock count
    FWMEMORY_API void requestLock();
    //TODO more robust use of semaphore : IDumpService::requestLock(obj) to store obj that request a lock
    // to checkconsisterncy ( m_dumpLockCount -> std::set< Object::sptr > )
    /// decrease semaphore lock count
    FWMEMORY_API void releaseLock();


    DumpStatus m_status;

    unsigned long m_dumpLockCount;

    static ::boost::filesystem::path m_dumpFolder;

    /**
     *  Reference to avoid systematic search in OSR
     * @note to put in IService
     */
//   ::boost::weak_ptr< ::fwData::Object > m_relatedObject ;
};

} // namespace fwMemory

#endif //_FWMEMORY_IDUMPSERVICE_HPP_
