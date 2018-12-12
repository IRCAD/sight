/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "visuVTKAdaptor/SOrientationMarker.hpp"

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkAxesActor.h>
#include <vtkCubeSource.h>
#include <vtkGenericDataObjectReader.h>
#include <vtkOrientationMarkerWidget.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkRenderer.h>
#include <vtkSmartPointer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SOrientationMarker);

namespace visuVTKAdaptor
{

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() :
    m_hAlign("left")
{
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    auto filePath = ::fwRuntime::getBundleResourceFilePath("visuVTKAdaptor", "human.vtk");

    vtkSmartPointer< vtkGenericDataObjectReader > reader = vtkSmartPointer< vtkGenericDataObjectReader >::New();
    reader->SetFileName( filePath.string().c_str() );
    reader->Update();
    vtkDataObject* obj = reader->GetOutput();
    vtkPolyData* mesh  = vtkPolyData::SafeDownCast(obj);

    SLM_WARN_IF("Orientation marker load failed", !obj);

    vtkSmartPointer<vtkPolyDataMapper> mapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    mapper->SetInputData(mesh);

    vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
    actor->SetMapper(mapper);

    m_widget = vtkSmartPointer<vtkOrientationMarkerWidget>::New();
    m_widget->SetOrientationMarker( actor );
    m_widget->SetInteractor( this->getInteractor() );

    if(m_hAlign == "left")
    {
        m_widget->SetViewport( 0.0, 0.0, 0.1, 0.1 );
    }
    else if(m_hAlign == "right")
    {
        m_widget->SetViewport( 0.9, 0.0, 1, 0.1 );
    }

    m_widget->SetEnabled( 1 );
    m_widget->InteractiveOff();
    this->setVtkPipelineModified();
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::stopping()
{
    this->removeAllPropFromRenderer();
    m_widget = nullptr;
}

//-----------------------------------------------------------------------------

void SOrientationMarker::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    m_hAlign = config.get<std::string>("hAlign", "left");
    SLM_ASSERT("'hAlign' value must be 'left' or 'right'", m_hAlign == "left" || m_hAlign == "right");
}

} //namespace visuVTKAdaptor
