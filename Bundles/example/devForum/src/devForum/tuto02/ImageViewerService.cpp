/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// vtk
#include <vtkCellPicker.h>
#include <vtkCommand.h>
#include <vtkImageData.h>
#include <vtkLookupTable.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkSmartPointer.h>

// Image object
#include <fwData/Image.hpp>

// Services tools
#include <fwServices/macros.hpp>

// fwVtkIO lib : intern lib used to convert image between vtk and fw4spl
#include <fwVtkIO/vtk.hpp>

#include "devForum/tuto02/ImageViewerService.hpp"

namespace devForum
{

namespace tuto02
{

//-----------------------------------------------------------------------------

// Declare a new service of type IRender for render Image
fwServicesRegisterMacro( ::fwRender::IRender, ::devForum::tuto02::ImageViewerService, ::fwData::Image );

//-----------------------------------------------------------------------------

ImageViewerService::ImageViewerService() noexcept
    :   ::devForum::tuto01::ImageViewerService()
{
}

//-----------------------------------------------------------------------------

ImageViewerService::~ImageViewerService() noexcept
{
}

//-----------------------------------------------------------------------------
void ImageViewerService::configuring()
{
    ::devForum::tuto01::ImageViewerService::configuring();
}

//-----------------------------------------------------------------------------
void ImageViewerService::updating()
{
    updateVTKPipeline();
}

//-----------------------------------------------------------------------------

void ImageViewerService::updateVTKPipeline()
{
    // Get associated image
    ::fwData::Image::sptr associatedImage = this->getObject< ::fwData::Image >();

    // Convert an ::fwDada::Image to a vtk Image
    vtkSmartPointer< vtkImageData > vtk_img = vtkSmartPointer< vtkImageData >::New();
    ::fwVtkIO::toVTKImage( associatedImage, vtk_img );

    // Change negato input
    m_negatoSagittal->SetInputData(vtk_img);
    m_negatoFrontal->SetInputData(vtk_img);
    m_negatoAxial->SetInputData(vtk_img);

    // Align negato on the new image center
    alignNegatoOnImageCenter();

    // Reset camera
    m_render->ResetCamera();

    // refresh vtk render
    (m_interactorManager->getInteractor())->Render();
}

//-----------------------------------------------------------------------------
void ImageViewerService::stopping()
{
    ::devForum::tuto01::ImageViewerService::stopping();
}
//-----------------------------------------------------------------------------

} // end of namespace tuto02

} // end of namespace devForum
