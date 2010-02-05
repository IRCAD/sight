/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _IOXML_FWXMLPATIENTDBREADERSERVICE_HPP_
#define _IOXML_FWXMLPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioXML/config.hpp"



namespace fwData
{
    class PatientDB;
    class Patient;
}


namespace ioXML
{

class IOXML_CLASS_API FwXMLPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLPatientDBReaderService)(::io::IReader::Baseclass) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLPatientDBReaderService() throw();

    /// Override
    IOXML_API virtual void configuring() throw(::fwTools::Failed) ;

    /// Override
    IOXML_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOXML_API void updating() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void updating( ::boost::shared_ptr< const ::fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOXML_API void info(std::ostream &_sstream ) ;

    /// Override
    IOXML_API virtual std::vector< std::string > getSupportedExtensions() ;

    /// Override
    IOXML_API virtual wxString getSelectorDialogTitle();

    /// Override
    IOXML_API virtual void configureWithIHM();

    /// Simple API to convert wxString to boost path and valid the
    //configuration
    void fixFilename(wxString _filename);

private :

    void notificationOfDBUpdate();

    ::boost::shared_ptr< ::fwData::PatientDB > createPatientDB( const ::boost::filesystem::path inrFileDir );

    bool isAnFwxmlArchive( const ::boost::filesystem::path filePath );

    ::fwData::PatientDB::sptr manageZipAndCreatePatientDB( const ::boost::filesystem::path _pArchivePath );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsPatientDBPath;

};

} // namespace ioDicom

#endif //_IOXML_FWXMLPATIENTDBREADERSERVICE_HPP_
