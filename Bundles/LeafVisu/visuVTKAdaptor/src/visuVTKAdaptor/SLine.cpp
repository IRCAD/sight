/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SLine.hpp"

#include <fwCom/Slots.hxx>

#include <fwServices/macros.hpp>

#include <boost/lexical_cast.hpp>

#include <vtkActor.h>
#include <vtkFloatArray.h>
#include <vtkImageData.h>
#include <vtkPointData.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkTexture.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SLine);

namespace visuVTKAdaptor
{
//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SLine::s_UPDATE_VISIBILITY_SLOT = "updateVisibility";
const ::fwCom::Slots::SlotKeyType SLine::s_UPDATE_LENGTH_SLOT     = "updateLength";

//------------------------------------------------------------------------------

SLine::SLine() noexcept :
    m_lineActor(vtkSmartPointer<vtkActor>::New()),
    m_vtkLine(vtkSmartPointer<vtkLineSource>::New()),
    m_mapper(vtkSmartPointer<vtkPolyDataMapper>::New()),
    m_length(1.f),
    m_width(1.f),
    m_transformLine(vtkSmartPointer<vtkTransform>::New()),
    m_dotLine(false)
{
    newSlot(s_UPDATE_VISIBILITY_SLOT, &SLine::updateVisibility, this);
    newSlot(s_UPDATE_LENGTH_SLOT, &SLine::updateLength, this);
}

//------------------------------------------------------------------------------

SLine::~SLine() noexcept
{
    m_lineActor = 0;
}

//------------------------------------------------------------------------------

void SLine::starting()
{
    this->initialize();

    this->buildPipeline();
    this->addToRenderer( m_lineActor );
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLine::stopping()
{
    this->removeAllPropFromRenderer();
    this->getRenderer()->RemoveActor(m_lineActor);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLine::updating()
{
}

//------------------------------------------------------------------------------

void SLine::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("service.config.<xmlattr>");

    m_length = config.get("length", 1.f);
    m_width  = config.get("width", 1.f);

    const std::string strColor = config.get("color", "#FFFFFF");

    m_color = ::fwData::Color::New();
    m_color->setRGBA(strColor);

    const std::string strLineOptions = config.get("dotted", "false");
    if(strLineOptions == "true" || strLineOptions == "yes" || strLineOptions == "on" || strLineOptions == "1")
    {
        m_dotLine = true;
    }
}

//------------------------------------------------------------------------------

void SLine::updateVisibility(bool _isVisible)
{
    m_lineActor->SetVisibility(_isVisible);

    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SLine::updateLine()
{
    m_vtkLine->SetPoint2(0.0, 0.0, m_length);
    m_vtkLine->Update();

    // create texture coordinates
    vtkSmartPointer<vtkPolyData> line                 = m_vtkLine->GetOutput();
    vtkSmartPointer<vtkFloatArray> textureCoordinates = vtkSmartPointer<vtkFloatArray>::New();
    textureCoordinates->SetNumberOfComponents(2);
    textureCoordinates->SetName("TextureCoordinates");

    float tuple[2] = {0.0f, 0.0f};
    textureCoordinates->InsertNextTuple(tuple);
    tuple[0] = m_length / 8.0f;
    tuple[1] = 0.0f;
    textureCoordinates->InsertNextTuple(tuple);

    line->GetPointData()->SetTCoords(textureCoordinates);

    m_mapper->SetInputData(line);
}

//------------------------------------------------------------------------------

void SLine::buildPipeline()
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

    float tuple[2] = {0.0f, 0.0f};
    textureCoordinates->InsertNextTuple(tuple);
    tuple[0] = m_length / 8.0f;
    tuple[1] = 0.0f;
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
        unsigned char* iptr = static_cast<unsigned char*>(stippleImage->GetScalarPointer( 0, 0, 0 ));

        std::memset(iptr, 0, 8 );
        iptr[0] = static_cast<unsigned char>(m_color->red() * 255.0f);
        iptr[1] = static_cast<unsigned char>(m_color->green() * 255.0f);
        iptr[2] = static_cast<unsigned char>(m_color->blue() * 255.0f);
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

void SLine::updateLength(float length)
{
    m_length = length;
    this->updateLine();
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------
} // namespace visuVTKAdaptor
