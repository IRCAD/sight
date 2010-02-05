/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOXML_FWXMLIMAGEWRITERSERVICE_HPP_
#define _IOXML_FWXMLIMAGEWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IWriter.hpp>

#include "ioXML/config.hpp"



namespace fwData
{
    class Image;
}


namespace ioXML
{

class IOXML_CLASS_API FwXMLImageWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLImageWriterService)(::io::IWriter::Baseclass) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLImageWriterService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLImageWriterService() throw();

    /// Override
    IOXML_API virtual void configuring() throw(::fwTools::Failed) ;

    /// Override
    IOXML_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void updating() throw(::fwTools::Failed);

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

    void saveImage( const ::boost::filesystem::path inrFileDir, ::boost::shared_ptr< ::fwData::Image > _pImage );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsImagePath;

};

} // namespace ioXML

#endif //_IOXML_FWXMLIMAGEWRITERSERVICE_HPP_
