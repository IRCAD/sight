/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SImage3DCursor.hpp"

#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkSphereSource.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SImage3DCursor );

namespace visuVTKAdaptor
{

//------------------------------------------------------------------------------

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPHERE_SLOT      = "updateSphere";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SImage3DCursor::SImage3DCursor() noexcept
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SImage3DCursor::updateSliceIndex, this);
    newSlot(s_UPDATE_SPHERE_SLOT, &SImage3DCursor::updateSphere, this);
}

//------------------------------------------------------------------------------

SImage3DCursor::~SImage3DCursor() noexcept
{
}

//------------------------------------------------------------------------------
void SImage3DCursor::setVisibility( bool visibility )
{
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void SImage3DCursor::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SImage3DCursor::starting()
{
    this->initialize();

    m_cursorPolyData = vtkSmartPointer<vtkPolyData>::New();
    m_cursorMapper   = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_cursorActor    = vtkSmartPointer<vtkActor>::New();

    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    if ( img->getField("IMAGE3DCURSOR_RADIUS") && img->getField("IMAGE3DCURSOR_COLOR") )
    {
        ::fwData::Float::sptr radius = img->getField< ::fwData::Float >("IMAGE3DCURSOR_RADIUS");
        ::fwData::Color::sptr color  = img->getField< ::fwData::Color >("IMAGE3DCURSOR_COLOR");

        this->buildPolyData(radius->value());
        m_cursorActor->GetProperty()->SetColor( color->red(), color->green(), color->blue());
    }
    else
    {
        this->buildPolyData();
        m_cursorActor->GetProperty()->SetColor(.1, .6, 1.);
    }

    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);

    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);
    this->updating();
}

//------------------------------------------------------------------------------

void SImage3DCursor::stopping()
{
    this->removeAllPropFromRenderer();
    this->requestRender();
    m_cursorPolyData = 0;
    m_cursorMapper   = 0;
    m_cursorActor    = 0;
}

//------------------------------------------------------------------------------

void SImage3DCursor::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos(image);
    int index[3] = { *m_sagittalIndex, *m_frontalIndex, *m_axialIndex };
    double center[3];
    this->sliceIndexToWorld(index, center);
    this->updateCursorPosition(center);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage3DCursor::swapping()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    this->updateImageInfos(image);
}

//-----------------------------------------------------------------------------

void SImage3DCursor::updateSliceIndex(int axial, int frontal, int sagittal)
{
    m_axialIndex->value()    = axial;
    m_frontalIndex->value()  = frontal;
    m_sagittalIndex->value() = sagittal;

    int index[3] = {sagittal, frontal, axial};
    double center[3];
    this->sliceIndexToWorld(index, center);
    this->updateCursorPosition(center);
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage3DCursor::updateSphere(::fwData::Color::sptr color, float radius)
{
    ::fwData::Image::sptr img = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);

    m_cursorActor->GetProperty()->SetColor( color->red(), color->green(), color->blue());
    this->buildPolyData(radius);

    m_cursorMapper->SetInputData( m_cursorPolyData );
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SImage3DCursor::updateCursorPosition( double world[3] )
{
    m_cursorActor->SetPosition(world);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void SImage3DCursor::buildPolyData(float radius)
{
    // point are stored Left,right,up,down
    vtkSmartPointer<vtkSphereSource> polySource = vtkSmartPointer<vtkSphereSource>::New();
    polySource->SetCenter(0.0, 0.0, 0.0);
    polySource->SetRadius(radius);
    polySource->SetPhiResolution(8);
    polySource->SetThetaResolution(8);

    polySource->SetOutput(m_cursorPolyData);
    polySource->Update();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SImage3DCursor::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push("image", ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT );

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor
