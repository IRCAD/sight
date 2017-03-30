/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Line.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkTexture.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::Line, ::fwData::Object );

namespace visuVTKAdaptor
{
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType Line::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType Line::s_UPDATE_LENGTH_SLOT     = "updateLength";

//------------------------------------------------------------------------------

Line::Line() throw() :
    m_lineActor(vtkSmartPointer<vtkActor>::New()),
    m_mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
    m_vtkLine(vtkSmartPointer<vtkLineSource>::New()),
    m_length(1.),
    m_width(1.),
    m_transformLine(vtkSmartPointer<vtkTransform>::New()),
    m_dotLine(false)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &Line::updateVisibility, this);
    newSlot(s_UPDATE_LENGTH_SLOT, &Line::updateLength, this);
}

//------------------------------------------------------------------------------

Line::~Line() throw()
{
    m_lineActor = 0;
}

//------------------------------------------------------------------------------

void Line::doStart() throw(fwTools::Failed)
{
    this->buildPipeline();
    this->addToRenderer( m_lineActor );
}

//------------------------------------------------------------------------------

void Line::doStop() throw(fwTools::Failed)
{
    this->removeAllPropFromRenderer();
    this->getRenderer()->RemoveActor(m_lineActor);
}

//------------------------------------------------------------------------------

void Line::doSwap() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Line::doUpdate() throw(fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void Line::doConfigure() throw(fwTools::Failed)
{
    SLM_ASSERT( "Wrong config name specified.", m_configuration->getName() == "config" );
    if ( m_configuration->hasAttribute( "length" ) )
    {
        m_length = boost::lexical_cast<double>( m_configuration->getAttributeValue( "length" ) );
    }
    if ( m_configuration->hasAttribute( "width" ) )
    {
        m_width = boost::lexical_cast<double>( m_configuration->getAttributeValue( "width" ) );
    }
    if ( m_configuration->hasAttribute( "color" ) )
    {
        const std::string strColor = m_configuration->getAttributeValue("color");

        m_color = ::fwData::Color::New();
        m_color->setRGBA(strColor);
    }
    else
    {
        m_color = ::fwData::Color::New(1.0f, 1.0f, 1.0f, 1.0f);
    }
    if ( m_configuration->hasAttribute( "dotted" ) )
    {
        const std::string strLineOptions = m_configuration->getAttributeValue("dotted");
        if(strLineOptions == "true" || strLineOptions == "on" || strLineOptions == "1")
        {
            m_dotLine = true;
        }
    }
}

//------------------------------------------------------------------------------

void Line::updateVisibility(bool _isVisible)
{
    m_lineActor->SetVisibility(_isVisible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------
void Line::updateLine()
{
    m_vtkLine->SetPoint2(0.0, 0.0, m_length);
    m_vtkLine->Update();

    // create texture coordinates
    vtkSmartPointer<vtkPolyData> line                 = m_vtkLine->GetOutput();
    vtkSmartPointer<vtkFloatArray> textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
    textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");

    float tuple[2] = {0.0, 0.0};
    textureCoordinates->InsertNextTuple(tuple);
    tuple[0] = m_length/8.0; tuple[1] = 0.0;
    textureCoordinates->InsertNextTuple(tuple);

    line->GetPointData()->SetTCoords(textureCoordinates);

    m_mapper->SetInputData(line);
}

//------------------------------------------------------------------------------
void Line::buildPipeline()
{
    vtkTransform* transform = m_renderService.lock()->getOrAddVtkTransform(m_transformId);

    // create a line using vtkLineSource
    m_vtkLine->SetPoint1(0.0, 0.0, 0.0);
    m_vtkLine->SetPoint2(0.0, 0.0, m_length);
    m_vtkLine->Update();
    vtkSmartPointer<vtkPolyData> line = m_vtkLine->GetOutput();

    // create texture coordinates
    vtkSmartPointer<vtkFloatArray> textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
    textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");

    float tuple[2] = {0.0, 0.0};
    textureCoordinates->InsertNextTuple(tuple);
    tuple[0] = m_length/8.0; tuple[1] = 0.0;
    textureCoordinates->InsertNextTuple(tuple);

    line->GetPointData()->SetTCoords(textureCoordinates);

    m_mapper->SetInputData(line);

    m_lineActor->SetMapper(m_mapper);

    if(m_dotLine)
    {
        // create an image of two pixels for the dot effect
        vtkSmartPointer<vtkImageData> stippleImage = vtkSmartPointer<vtkImageData>::New();
        stippleImage->SetExtent( 0, 1, 0, 0, 0, 0 );
        stippleImage->SetOrigin( 0.0, 0.0, 0.0 );
        stippleImage->SetSpacing( 1.0, 1.0, 1.0 );
        stippleImage->AllocateScalars(VTK_UNSIGNED_CHAR, 4);

        // fill the image
        unsigned char* iptr = (unsigned char*) stippleImage->GetScalarPointer( 0, 0, 0 );

        std::memset(iptr, 0, 8*sizeof(unsigned char));
        iptr[0] = m_color->red()*255;
        iptr[1] = m_color->green()*255;
        iptr[2] = m_color->blue()*255;
        iptr[3] = 255;

        // create a texture from the image
        vtkSmartPointer<vtkTexture> vtkTex = vtkSmartPointer< vtkTexture >::New();
        vtkTex->SetInputData(stippleImage);
        vtkTex->SetRepeat(1);
        m_lineActor->GetProperty()->SetTexture("stipple", vtkTex);
    }
    else
    {
        m_lineActor->GetProperty()->SetColor(m_color->red(), m_color->green(), m_color->blue());
    }

    m_lineActor->GetProperty()->SetLineWidth(m_width);
    m_lineActor->SetUserTransform(transform);
    this->setVtkPipelineModified();
}
//------------------------------------------------------------------------------
void Line::updateLength(float length)
{
    m_length = length;
    this->updateLine();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------
} // namespace visuVTKAdaptor
