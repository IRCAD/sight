#ifndef _IOXML_FWXMLPATIENTDBWRITERSERVICE_HPP_
#define _IOXML_FWXMLPATIENTDBWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioXML/config.hpp"



namespace fwData
{
    class PatientDB;
}


namespace ioXML
{

class IOXML_CLASS_API FwXMLPatientDBWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLPatientDBWriterService)(::io::IWriter::Baseclass) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLPatientDBWriterService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLPatientDBWriterService() throw();

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
    IOXML_API std::string getCfgExtensionPoint() ;

    /// Override
    IOXML_API virtual std::string getPersistanceId() ;


    /// Override
    IOXML_API virtual void configureWithIHM();

private :

    void savePatientDB( const ::boost::filesystem::path inrFileDir, ::boost::shared_ptr< ::fwData::PatientDB > _pPatientDB );

    bool isAnFwxmlArchive( const ::boost::filesystem::path filePath ) const;

    ::boost::filesystem::path correctFileFormat( const ::boost::filesystem::path _filePath ) const;

    void manageZipAndSavePatientDB( const ::boost::filesystem::path inrFileDir, ::fwData::PatientDB::sptr _pPatient );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsPatientDBPath;

};

} // namespace ioXML

#endif //_IOXML_FWXMLPATIENTDBWRITERSERVICE_HPP_
