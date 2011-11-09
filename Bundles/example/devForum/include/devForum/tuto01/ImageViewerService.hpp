/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DEVFORUM_TUTO1_IMAGEVIEWERSERVICE_HPP_
#define _DEVFORUM_TUTO1_IMAGEVIEWERSERVICE_HPP_

// Include wx to manage layout IHM
#include <wx/wxprec.h>
#include <wx/aui/aui.h>

// Include vtk tools used to render an image with a negato
#include  <vtkRenderer.h>
#include  <vtkImagePlaneWidget.h>

// Include vtkinria3d interactor
#include <vtkinria3d/wxVTKRenderWindowInteractor.h>

// Include abstract class
#include <fwRender/IRender.hpp>

// Include export definitions
#include "devForum/config.hpp"

namespace devForum
{

namespace tuto01
{

/**
 * @brief   Service rendering a ::fwData::Image using VTK.
 * @class   ImageViewerService.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
class DEVFORUM_CLASS_API ImageViewerService : public ::fwRender::IRender
{

public :

    // Build some class definition like sptr, etc.
    fwCoreServiceClassDefinitionsMacro ( (ImageViewerService)(::fwRender::IRender) );

    /// Constructor, does nothing.
    DEVFORUM_API ImageViewerService() throw() ;

    /// Destructor, does nothing.
    DEVFORUM_API virtual ~ImageViewerService() throw() ;


protected :

    /**
    * @brief This method is called when the service starts.
    *
    * This method builds le layout manager of panel, create the vtk render and the vtk interactor and create a vtk pipeline to visualizate associated image in the vtk render.
    * @post if the associated image was not allocated, then an synthetic image is created.
    */
    DEVFORUM_API virtual void starting() throw(fwTools::Failed);

    /**
    * @brief This method is called when the service stops.
    *
    * This method destroys vtk components and wxWidget containers
    */
    DEVFORUM_API virtual void stopping() throw(fwTools::Failed);

    /// This method does nothing here.
    DEVFORUM_API virtual void updating() throw(fwTools::Failed);

    /// Try to retrieve the guiContainerId defines in the service configuration
    DEVFORUM_API virtual void configuring() throw(::fwTools::Failed);

    /// This method is called on msg notification, here does nothing
    DEVFORUM_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed);



    /// Initialize image with a synthetic image which represents a sphere.
    DEVFORUM_API void createSyntheticImage( ::fwData::Image::sptr _pImage );

    /// Align negato to image center
    DEVFORUM_API void alignNegatoOnImageCenter();

    /// This method get associated image and initializes a VTK pipeline to render this image
    DEVFORUM_API void initVTKPipeline();



    /// Required to facilitate resize of an empty vtk rendering window : why ?
    wxAuiManager* m_wxmanager;

    /// @brief vtk renderer
    vtkRenderer * m_render ;

    /// VTK Interactor window
    ::wxVTKRenderWindowInteractor* m_interactor;

    /// vtk plane widget used in the negatoscope axial plane.
    vtkImagePlaneWidget* m_negatoAxial;

    /// vtk plane widget used in the negatoscope sagittal plane.
    vtkImagePlaneWidget* m_negatoSagittal;

    /// vtk plane widget used in the negatoscope frontal plane.
    vtkImagePlaneWidget* m_negatoFrontal;
};

} // end namespace tuto01

} // end namespace devForum


#endif // _DEVFORUM_TUTO1_IMAGEVIEWERSERVICE_HPP_
