/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUMQT_TUTO2_IMAGEVIEWERSERVICE_HPP_
#define _DEVFORUMQT_TUTO2_IMAGEVIEWERSERVICE_HPP_

// Include wx to manage layout IHM
//#include <wx/wxprec.h>
//#include <wx/aui/aui.h>

// Include vtk tools used to render an image with a negato
#include  <vtkRenderer.h>
#include  <vtkImagePlaneWidget.h>

// Include vtkinria3d interactor
//#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

// Include abstract class
#include <fwRender/IRender.hpp>

// Include export definitions
#include "devForumQt/config.hpp"
#include "devForumQt/tuto01/ImageViewerService.hpp"

namespace devForum
{

namespace tuto02
{

/**
 * @brief   Service rendering a ::fwData::Image using VTK.
 * @class   ImageViewerService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUMQT_CLASS_API ImageViewerService : public ::devForum::tuto01::ImageViewerService
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageViewerService)(::fwRender::IRender) );

    /// Constructor, does nothing.
    DEVFORUMQT_API ImageViewerService() throw() ;

    /// Destructor, does nothing.
    DEVFORUMQT_API virtual ~ImageViewerService() throw() ;


protected :

    /// Try to retrieve the guiContainerId defines in the service configuration
    DEVFORUMQT_API virtual void configuring() throw(::fwTools::Failed);

    /// This method refreshes the pipeline with the current image and update the vtk render.
    DEVFORUMQT_API virtual void updating() throw(fwTools::Failed);

    DEVFORUMQT_API virtual void stopping() throw(fwTools::Failed);

    /// This method get the new associated image and update the VTK pipeline
    DEVFORUMQT_API void updateVTKPipeline();
};

} // end namespace tuto02

} // end namespace devForum


#endif // _DEVFORUMQT_TUTO2_IMAGEVIEWERSERVICE_HPP_
