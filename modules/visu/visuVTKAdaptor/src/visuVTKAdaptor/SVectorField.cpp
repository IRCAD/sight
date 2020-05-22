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

#include "visuVTKAdaptor/SVectorField.hpp"

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp>

#include <fwServices/macros.hpp>

#include <fwVtkIO/helper/TransferFunction.hpp>
#include <fwVtkIO/vtk.hpp>

#include <vtkActor.h>
#include <vtkArrowSource.h>
#include <vtkAssignAttribute.h>
#include <vtkGlyph3D.h>
#include <vtkGlyph3DMapper.h>
#include <vtkGlyphSource2D.h>
#include <vtkImageData.h>
#include <vtkMaskPoints.h>
#include <vtkPointData.h>
#include <vtkPolyDataMapper.h>

namespace visuVTKAdaptor
{

fwServicesRegisterMacro(::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SVectorField);

static const ::fwServices::IService::KeyType s_IMAGE_INPUT = "image";

//------------------------------------------------------------------------------

SVectorField::SVectorField() noexcept
{
    m_imageData = vtkSmartPointer<vtkImageData>::New();
}

//------------------------------------------------------------------------------

SVectorField::~SVectorField() noexcept
{
}

//------------------------------------------------------------------------------

void SVectorField::starting()
{
    this->initialize();
    this->updating();
}

//------------------------------------------------------------------------------

void SVectorField::stopping()
{
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void SVectorField::updating()
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid && image->getNumberOfComponents() == 3)
    {
        this->buildPipeline();
    }
    else
    {
        this->destroyPipeline();
    }
}

//------------------------------------------------------------------------------

void SVectorField::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SVectorField::buildPipeline( )
{
    ::fwData::Image::csptr image = this->getInput< ::fwData::Image >(s_IMAGE_INPUT);

    ::fwVtkIO::toVTKImage(image, m_imageData);

    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

    m_arrowSource = arrowSource;

    vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();

    ptMask->SetInputData(m_imageData);
    ptMask->SetOnRatio(1);
    ptMask->RandomModeOn();

// #define USE_GPU_GLYPH
#ifndef USE_GPU_GLYPH
    ptMask->SetMaximumNumberOfPoints(5000);

    vtkSmartPointer<vtkGlyph3D> glyphFilter = vtkSmartPointer<vtkGlyph3D>::New();
    glyphFilter->SetSourceConnection(m_arrowSource->GetOutputPort());

    glyphFilter->ScalingOn();

    glyphFilter->SetVectorModeToUseVector();

    glyphFilter->OrientOn();

    glyphFilter->SetInputConnection(ptMask->GetOutputPort());

    glyphFilter->SetInputArrayToProcess( 1, m_imageData->GetInformation());

    vtkSmartPointer<vtkPolyDataMapper> vectorMapper = vtkSmartPointer<vtkPolyDataMapper>::New();
    vectorMapper->SetInputConnection(glyphFilter->GetOutputPort());
#else
    ptMask->SetMaximumNumberOfPoints(15000);

    vtkSmartPointer<vtkGlyph3DMapper> vectorMapper = vtkSmartPointer<vtkGlyph3DMapper>::New();
    vectorMapper->SetOrientationArray(vtkDataSetAttributes::SCALARS);
    vectorMapper->SetSourceConnection(m_arrowSource->GetOutputPort());
    vectorMapper->OrientOn();
    vectorMapper->SetInputConnection(ptMask->GetOutputPort());

    SLM_DEBUG("USE_GPU");
#endif

    vtkSmartPointer<vtkActor> vectorActor = vtkSmartPointer<vtkActor>::New();
    vectorActor->SetMapper(vectorMapper);

    this->addToRenderer(vectorActor);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SVectorField::destroyPipeline( )
{
    this->removeAllPropFromRenderer();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SVectorField::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INPUT, ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
