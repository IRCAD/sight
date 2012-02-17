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
    fwCoreServiceClassDefinitionsMacro ( (FwXMLImageWriterService)(::io::IWriter) ) ;

protected:

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
    IOXML_API virtual ::io::IOPathType getIOPathType() const;

    /// Override
    IOXML_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void updating() throw(::fwTools::Failed);

    /// Override
    IOXML_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOXML_API void info(std::ostream &_sstream ) ;

    /// Override
    IOXML_API virtual void configureWithIHM();

private :

    void saveImage( const ::boost::filesystem::path &file, ::boost::shared_ptr< ::fwData::Image > _pImage );

};

} // namespace ioXML

#endif //_IOXML_FWXMLIMAGEWRITERSERVICE_HPP_

