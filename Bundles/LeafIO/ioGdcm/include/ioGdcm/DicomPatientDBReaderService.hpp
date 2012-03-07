/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOGDCM_DICOMPATIENTDBREADERSERVICE_HPP_
#define _IOGDCM_DICOMPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioGdcm/config.hpp"

namespace fwData
{
    class PatientDB;
    class Patient;
}

namespace ioGdcm
{

class IOGDCM_CLASS_API DicomPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (DicomPatientDBReaderService)( ::io::IReader) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOGDCM_API DicomPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOGDCM_API virtual ~DicomPatientDBReaderService() throw();


    typedef ::fwRuntime::ConfigurationElement::sptr ConfigurationType;

    /// Override
    IOGDCM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API void updating() throw(::fwTools::Failed);

    /// Override
    IOGDCM_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOGDCM_API void info(std::ostream &_sstream ) ;

    /// Override
    IOGDCM_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOGDCM_API virtual std::string getSelectorDialogTitle();

    /// Override
    IOGDCM_API virtual void configureWithIHM();

    /// Return managed file type, here FOLDER
    IOGDCM_API ::io::IOPathType getIOPathType() const;

private :

    void notificationOfDBUpdate();

    ::boost::shared_ptr< ::fwData::PatientDB > createPatientDB( const ::boost::filesystem::path & dicomFile );

};

} // namespace ioGdcm

#endif //_IOGDCM_DICOMPATIENTDBREADERSERVICE_HPP_
