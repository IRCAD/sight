/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOVTKGDCM_DICOMPATIENTWRITERSERVICE_HPP_
#define _IOVTKGDCM_DICOMPATIENTWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioVtkGdcm/config.hpp"

namespace fwData
{
    class Patient;
}


namespace ioVtkGdcm
{

class IOVTKGDCM_CLASS_API DicomPatientWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (DicomPatientWriterService)( ::io::IWriter) ) ;
    /**
     * @brief   constructor
     *
     */
    IOVTKGDCM_API DicomPatientWriterService() throw();

    /**
     * @brief   destructor
     */
    IOVTKGDCM_API virtual ~DicomPatientWriterService() throw();

protected:

    /// Override
    IOVTKGDCM_API virtual void configuring() throw(::fwTools::Failed) ;

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

private :

    void savePatient(   const ::boost::filesystem::path patientDBPath,
            ::boost::shared_ptr< ::fwData::Patient > _pPatient );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsPatientPath;

};

} // namespace ioVtkGdcm

#endif //_IOVTKGDCM_DICOMPATIENTWRITERSERVICE_HPP_
