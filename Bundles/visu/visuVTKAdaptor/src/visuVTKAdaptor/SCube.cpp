/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SCube.hpp"

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCubeSource.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SCube);

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

SCube::SCube() noexcept
{

}

//------------------------------------------------------------------------------

SCube::~SCube() noexcept
{

}

//------------------------------------------------------------------------------

void SCube::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SCube::starting()
{
    this->initialize();

    vtkCubeSource* cube       = vtkCubeSource::New();
    vtkPolyDataMapper* mapper = vtkPolyDataMapper::New();
    mapper->SetInputConnection(cube->GetOutputPort());
    vtkActor* actor = vtkActor::New();
    actor->SetMapper(mapper);
    this->addToRenderer(actor);
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SCube::updating()
{

}

//------------------------------------------------------------------------------

void SCube::stopping()
{
    this->removeAllPropFromRenderer();
    this->requestRender();
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
