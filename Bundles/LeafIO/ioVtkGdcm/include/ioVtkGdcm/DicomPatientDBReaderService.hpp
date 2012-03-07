/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTKGDCM_DICOMPATIENTDBREADERSERVICE_HPP_
#define _IOVTKGDCM_DICOMPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioVtkGdcm/config.hpp"

namespace fwData
{
    class PatientDB;
    class Patient;
}


namespace ioVtkGdcm
{

class IOVTKGDCM_CLASS_API DicomPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (DicomPatientDBReaderService)( ::io::IReader) ) ;
    /**
     * @brief   constructor
     *
     */
    IOVTKGDCM_API DicomPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~DicomPatientDBReaderService() throw();

protected:

    /// Override
    IOVTKGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOVTKGDCM_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOVTKGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOVTKGDCM_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOVTKGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOVTKGDCM_API virtual void configureWithIHM();

    /// Return path type managed by the service, here FOLDER
    IOVTKGDCM_API ::io::IOPathType getIOPathType() const;

private :

    void notificationOfDBUpdate();

    ::boost::shared_ptr< ::fwData::PatientDB > createPatientDB( const ::boost::filesystem::path dicomDir );

};

} // namespace ioDicom

#endif //_IOVTKGDCM_INRPATIENTDBREADERSERVICE_HPP_
