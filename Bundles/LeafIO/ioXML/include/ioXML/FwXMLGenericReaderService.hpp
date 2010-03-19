/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _IOXML_FWXMLGENERICREADERSERVICE_HPP_
#define _IOXML_FWXMLGENERICREADERSERVICE_HPP_

#include <wx/string.h>
#include <string>
#include <boost/filesystem/path.hpp>

#include <fwXML/reader/FwXMLObjectReader.hpp>
#include <io/IReader.hpp>
#include <fwTools/Object.hpp>

#include "ioXML/config.hpp"





namespace ioXML
{

class IOXML_CLASS_API FwXMLGenericReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLGenericReaderService)(::io::IReader::Baseclass) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLGenericReaderService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLGenericReaderService() throw();

    /// Simple API to convert wxString to boost path and valid the
    //configuration
    void fixFilename(wxString _filename);

protected:

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
    IOXML_API virtual void configureWithIHM();

private :

    void notificationOfUpdate();

    ::boost::shared_ptr< ::fwTools::Object > loadData( const ::boost::filesystem::path inrFileDir );

    bool isAnFwxmlArchive( const ::boost::filesystem::path filePath );


    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsObjectPath;

    //void createObject( const ::boost::filesystem::path path, ::fwTools::Object::sptr _obj );

    ::boost::shared_ptr< ::fwTools::Object > manageZipAndLoadData( const ::boost::filesystem::path path );
    ::boost::filesystem::path correctFileFormat( const ::boost::filesystem::path _filePath ) const;

    ::fwXML::reader::FwXMLObjectReader m_reader;

};

} // namespace ioXML

#endif //_IOXML_FWXMLGENERICREADERSERVICE_HPP_
