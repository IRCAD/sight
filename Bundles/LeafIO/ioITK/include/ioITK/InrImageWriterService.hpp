/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_INRIMAGEWRITERSERVICE_HPP_
#define _IOITK_INRIMAGEWRITERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>

#include <fwData/Image.hpp>
#include <fwData/Patient.hpp>

#include <io/IWriter.hpp>


#include "ioITK/export.hpp"


namespace ioITK
{

class IOITK_CLASS_API InrImageWriterService : public ::io::IWriter
{

public :
    fwCoreServiceClassDefinitionsMacro ( (InrImageWriterService)( ::io::IWriter) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API InrImageWriterService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~InrImageWriterService() throw();

    /// Override
    IOITK_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IOITK_API void updating() throw(::fwTools::Failed);

    /// Override
    IOITK_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) {} ;

    /// Override
    IOITK_API void info(std::ostream &_sstream ) ;

    /// Override
    IOITK_API virtual void configureWithIHM();

    /// Return managed file type, here FILE
    IOITK_API ::io::IOPathType getIOPathType() const;

private :

    void saveImage( const ::boost::filesystem::path inrFile, ::fwData::Image::sptr _pImage );

};

} // namespace ioITK

#endif //_IOITK_INRIMAGEWRITERSERVICE_HPP_
