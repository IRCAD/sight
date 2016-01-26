/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_TUTO02_IMAGEVIEWERSERVICE_HPP__
#define __DEVFORUM_TUTO02_IMAGEVIEWERSERVICE_HPP__


// Include vtk tools used to render an image with a negato
#include  <vtkRenderer.h>
#include  <vtkImagePlaneWidget.h>

// Include abstract class
#include <fwRender/IRender.hpp>

// Include export definitions
#include "devForum/config.hpp"
#include "devForum/tuto01/ImageViewerService.hpp"

namespace devForum
{

namespace tuto02
{

/**
 * @brief   Service rendering a ::fwData::Image using VTK.
 * @class   ImageViewerService
 */
class DEVFORUM_CLASS_API ImageViewerService : public ::devForum::tuto01::ImageViewerService
{

public:

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageViewerService)(::fwRender::IRender) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageViewerService() throw();

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageViewerService() throw();


protected:

    /// Try to retrieve the guiContainerId defines in the service configuration
    DEVFORUM_API virtual void configuring() throw(::fwTools::Failed);

    /// This method refreshes the pipeline with the current image and update the vtk render.
    DEVFORUM_API virtual void updating() throw(fwTools::Failed);

    DEVFORUM_API virtual void stopping() throw(fwTools::Failed);

    /// This method get the new associated image and update the VTK pipeline
    DEVFORUM_API void updateVTKPipeline();
};

} // end namespace tuto02

} // end namespace devForum


#endif // __DEVFORUM_TUTO02_IMAGEVIEWERSERVICE_HPP__
