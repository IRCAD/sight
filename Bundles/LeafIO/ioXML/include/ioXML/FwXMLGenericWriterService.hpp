/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOXML_FWXMLGENERICWRITERSERVICE_HPP_
#define _IOXML_FWXMLGENERICWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwXML/writer/FwXMLObjectWriter.hpp>
#include <io/IWriter.hpp>

#include "ioXML/config.hpp"


namespace fwData
{
    class Generic;
}


namespace ioXML
{

class IOXML_CLASS_API FwXMLGenericWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLGenericWriterService)(::io::IWriter) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLGenericWriterService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLGenericWriterService() throw();

protected:

    /** Search if archive extension is changed
     * @verbatim
       <service ... >
         <archiveExtension>.fxz</archiveExtension>
       </service>
       @endverbatim
     */
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

    /// Change the extension of the compressed archive
    IOXML_API void setArchiveExtension( const std::string & _archiveExtenstion );

private :

    void saveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj );

    bool isAnFwxmlArchive( const ::boost::filesystem::path filePath );
    void manageZipAndSaveData( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj );
    ::boost::filesystem::path correctFileFormat( const ::boost::filesystem::path _filePath ) const;

    ::fwXML::writer::FwXMLObjectWriter m_writer;


    std::string m_archiveExtenstion;
};

} // namespace ioXML

#endif //_IOXML_FWXMLGENERICWRITERSERVICE_HPP_
