/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwServices/IEditionService.hpp>

#include <fwComEd/Dictionary.hpp>
#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/TransferFunctionMsg.hpp>

#include <fwServices/macros.hpp>

#include <fwData/Boolean.hpp>
#include <fwData/Color.hpp>
#include <fwData/Image.hpp>
#include <fwData/String.hpp>
#include <fwData/TransferFunction.hpp>

#include <vtkIO/vtk.hpp>
#include <vtkIO/helper/TransferFunction.hpp>

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

#include <fwRenderVTK/vtk/fwVtkWindowLevelLookupTable.hpp>

#include "visuVTKAdaptor/VectorField.hpp"


fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::VectorField, ::fwData::Image ) ;

namespace visuVTKAdaptor
{


//------------------------------------------------------------------------------

VectorField::VectorField() throw()
{
    m_imageData  = vtkImageData::New();


    // Manage events
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER                     );
    addNewHandledEvent( ::fwComEd::ImageMsg::MODIFIED                   );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE                  );
}

//------------------------------------------------------------------------------

VectorField::~VectorField() throw()
{
    m_imageData->Delete();
    m_imageData = NULL;
}

//------------------------------------------------------------------------------

void VectorField::doStart() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void VectorField::doStop() throw(fwTools::Failed)
{
    this->destroyPipeline();
}

//------------------------------------------------------------------------------

void VectorField::doSwap() throw(fwTools::Failed)
{
    this->doUpdate();
}

//------------------------------------------------------------------------------

void VectorField::doUpdate() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

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

void VectorField::doUpdate(::fwServices::ObjectMsg::csptr msg) throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid && image->getNumberOfComponents() == 3)
    {
        if ( msg->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( msg->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            this->doUpdate();
        }

        if ( msg->hasEvent( ::fwComEd::ImageMsg::MODIFIED ) )
        {
            m_imageData->Modified();

            this->setVtkPipelineModified();
        }
    }
    else
    {
        this->destroyPipeline();
    }
}

//------------------------------------------------------------------------------

void VectorField::configuring() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void VectorField::buildPipeline( )
{
    SLM_TRACE_FUNC();

    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();

    ::vtkIO::toVTKImage(image, m_imageData);

    // m_imageData->GetPointData()->SetActiveVectors("ImageScalars");

    vtkSmartPointer<vtkArrowSource> arrowSource = vtkSmartPointer<vtkArrowSource>::New();

    // vtkSmartPointer<vtkGlyphSource2D> arrowSource = vtkSmartPointer<vtkGlyphSource2D>::New();
    // arrowSource->SetGlyphTypeToArrow();
    // arrowSource->FilledOff();

    m_arrowSource = arrowSource;

    vtkSmartPointer<vtkMaskPoints> ptMask = vtkSmartPointer<vtkMaskPoints>::New();

    ptMask->SetInput(m_imageData);
    ptMask->SetOnRatio(1);
    ptMask->RandomModeOn();


    // vtkSmartPointer<vtkAssignAttribute> assignAttr = vtkSmartPointer<vtkAssignAttribute>::New();
    // assignAttr->SetInputConnection(ptMask->GetOutputPort());
    // assignAttr->Assign( vtkDataSetAttributes::SCALARS, vtkDataSetAttributes::VECTORS, vtkAssignAttribute::POINT_DATA );


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

    OSLM_WARN("USE_GPU");
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


} //namespace visuVTKAdaptor
