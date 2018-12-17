/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

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
