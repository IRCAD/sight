/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOXML_FWXMLIMAGEREADERSERVICE_HPP_
#define _IOXML_FWXMLIMAGEREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <io/IReader.hpp>

#include "ioXML/config.hpp"



namespace fwData
{
    class Image;
    class Patient;
}


namespace ioXML
{

class IOXML_CLASS_API FwXMLImageReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (FwXMLImageReaderService)(::io::IReader::Baseclass) ) ;

    /**
     * @brief   constructor
     *
     */
    IOXML_API FwXMLImageReaderService() throw();

    /**
     * @brief   destructor
     */
    IOXML_API virtual ~FwXMLImageReaderService() throw();

protected:

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
    IOXML_API virtual void configureWithIHM();

private :

    void notificationOfDBUpdate();

    ::boost::shared_ptr< ::fwData::Image > createImage( const ::boost::filesystem::path inrFileDir );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsImagePath;

};

} // namespace ioXML

#endif //_IOXML_FWXMLIMAGEREADERSERVICE_HPP_
