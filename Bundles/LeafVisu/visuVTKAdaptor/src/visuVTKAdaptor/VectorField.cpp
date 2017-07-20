/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/VectorField.hpp"

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

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::VectorField, ::fwData::Image );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

VectorField::VectorField() noexcept
{
    m_imageData = vtkImageData::New();
}

//------------------------------------------------------------------------------

VectorField::~VectorField() noexcept
{
    m_imageData->Delete();
    m_imageData = NULL;
}

//------------------------------------------------------------------------------

void VectorField::doStart()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void VectorField::doStop()
{
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void VectorField::doSwap()
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void VectorField::doUpdate()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
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

void VectorField::doConfigure()
{
}

//------------------------------------------------------------------------------

void VectorField::buildPipeline( )
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::fwVtkIO::toVTKImage(image, m_imageData);

    // m_imageData->GetPointData()->SetActiveVectors("ImageScalars");

    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

    // vtkSmartPointer<vtkGlyphSource2D> arrowSource = vtkSmartPointer<vtkGlyphSource2D>::New();
    // arrowSource->SetGlyphTypeToArrow();
    // arrowSource->FilledOff();

    m_arrowSource = arrowSource;

    vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();

    ptMask->SetInputData(m_imageData);
    ptMask->SetOnRatio(1);
    ptMask->RandomModeOn();

    // vtkSmartPointer<vtkAssignAttribute> assignAttr = vtkSmartPointer<vtkAssignAttribute>::New();
    // assignAttr->SetInputConnection(ptMask->GetOutputPort());
    // assignAttr->Assign( vtkDataSetAttributes::SCALARS, vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA
    // );

// #define USE_GPU_GLYPH
#ifndef USE_GPU_GLYPH
    ptMask->SetMaximumNumberOfPoints(5000);

    vtkSmartPointer<vtkGlyph3D> glyphFilter = vtkSmartPointer<vtkGlyph3D>::New();
    glyphFilter->SetSourceConnection(m_arrowSource->GetOutputPort());

    // glyphFilter->SetScaleModeToScaleByVector();
    glyphFilter->ScalingOn();

    glyphFilter->SetVectorModeToUseVector();

    glyphFilter->OrientOn();

    glyphFilter->SetInputConnection(ptMask->GetOutputPort());
    // glyphFilter->SetInputConnection(assignAttr->GetOutputPort());
    // glyphFilter->SetInput(m_imageData);

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

    SLM_WARN("USE_GPU");
#endif

    vtkSmartPointer<vtkActor> vectorActor = vtkSmartPointer<vtkActor>::New();
    vectorActor->SetMapper(vectorMapper);

    this->addToRenderer(vectorActor);

    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void VectorField::destroyPipeline( )
{
    this->removeAllPropFromRenderer();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType VectorField::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_BUFFER_MODIFIED_SIG, s_UPDATE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
