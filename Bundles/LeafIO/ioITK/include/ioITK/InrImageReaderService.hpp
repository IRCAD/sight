/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_INRIMAGEREADERSERVICE_HPP_
#define _IOITK_INRIMAGEREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwData/Image.hpp>

#include <io/IReader.hpp>

#include "ioITK/export.hpp"


namespace ioITK
{

class IOITK_CLASS_API InrImageReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (InrImageReaderService)( ::io::IReader) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API InrImageReaderService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~InrImageReaderService() throw();

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed){};

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed){};

    IOITK_API virtual void updating() throw(::fwTools::Failed) ;

    /// Override
    IOITK_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FILE
    IOITK_API ::io::IOPathType getIOPathType() const;

private :

    void notificationOfDBUpdate();

    bool createImage( const ::boost::filesystem::path inrFileDir, ::fwData::Image::sptr _pImg );
};

} // namespace ioITK

#endif //_IOITK_INRIMAGEREADERSERVICE_HPP_
