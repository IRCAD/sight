/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __DEVFORUM_TUTO03_IMAGEVIEWERSERVICE_HPP__
#define __DEVFORUM_TUTO03_IMAGEVIEWERSERVICE_HPP__


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
 * @class   ImageViewerService
 */
class DEVFORUM_CLASS_API ImageViewerService : public ::devForum::tuto02::ImageViewerService
{

public:

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageViewerService)(::fwRender::IRender) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageViewerService() noexcept;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageViewerService() noexcept;

protected:

    DEVFORUM_API virtual void configuring();
    DEVFORUM_API virtual void stopping();

};

} // end namespace tuto03

} // end namespace devForum


#endif // __DEVFORUM_TUTO03_IMAGEVIEWERSERVICE_HPP__
