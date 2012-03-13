/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUM_TUTO3_IMAGEVIEWERSERVICE_HPP_
#define _DEVFORUM_TUTO3_IMAGEVIEWERSERVICE_HPP_


// Include vtk tools used to render an image with a negato
#include  <vtkRenderer.h>
#include  <vtkImagePlaneWidget.h>

// Include abstract class
#include <fwRender/IRender.hpp>

// Include export definitions
#include "devForum/config.hpp"
#include "devForum/tuto02/ImageViewerService.hpp"

namespace devForum
{

namespace tuto03
{

/**
 * @brief   Service rendering a ::fwData::Image using VTK.
 * @class   ImageViewerService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUM_CLASS_API ImageViewerService : public ::devForum::tuto02::ImageViewerService
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageViewerService)(::fwRender::IRender) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageViewerService() throw() ;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageViewerService() throw() ;

protected :

    /// This method is called on msg notification, force updating of vtk render from current image.
    DEVFORUM_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);
    DEVFORUM_API virtual void configuring() throw(::fwTools::Failed);
    DEVFORUM_API virtual void stopping() throw(::fwTools::Failed);

};

} // end namespace tuto03

} // end namespace devForum


#endif // _DEVFORUM_TUTO3_IMAGEVIEWERSERVICE_HPP_
