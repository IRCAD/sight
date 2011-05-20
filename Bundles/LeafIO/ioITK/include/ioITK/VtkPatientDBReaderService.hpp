/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2011.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IOITK_VTKPATIENTDBREADERSERVICE_HPP_
#define _IOITK_VTKPATIENTDBREADERSERVICE_HPP_

#include <string>
#include <boost/filesystem/path.hpp>
#include <fwData/Image.hpp>

#include <io/IReader.hpp>

#include "ioITK/export.hpp"

namespace ioITK
{

class IOITK_CLASS_API VtkPatientDBReaderService : public ::io::IReader
{

public :
    fwCoreServiceClassDefinitionsMacro ( (VtkPatientDBReaderService)( ::io::IReader) ) ;

protected:

    /**
     * @brief   constructor
     *
     */
    IOITK_API VtkPatientDBReaderService() throw();

    /**
     * @brief   destructor
     */
    IOITK_API virtual ~VtkPatientDBReaderService() throw();

    IOITK_API virtual void configuring() throw(::fwTools::Failed) ;

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

private :

    void notificationOfDBUpdate();

    ::fwData::Image::sptr createImage( const ::boost::filesystem::path inrFileDir );

    bool m_bServiceIsConfigured;

    ::boost::filesystem::path m_fsImagePath;

};

} // namespace ioITK

#endif //_IOITK_VTKPATIENTDBREADERSERVICE_HPP_
