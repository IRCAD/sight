/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/Image3DCursor.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwData/Color.hpp>
#include <fwData/Float.hpp>
#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/TransferFunction.hpp>

#include <fwDataTools/fieldHelper/Image.hpp>
#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwRenderVTK/vtk/Helpers.hpp>

#include <fwServices/macros.hpp>

#include <boost/format.hpp>

#include <vtkActor.h>
#include <vtkParametricBoy.h>
#include <vtkParametricFunctionSource.h>
#include <vtkPolyData.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkSphereSource.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::Image3DCursor, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SPHERE_SLOT      = "updateSphere";

//------------------------------------------------------------------------------

Image3DCursor::Image3DCursor() noexcept :
    m_priority(.6)
{
    ////handlingEventOff();
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &Image3DCursor::updateSliceIndex, this);
    newSlot(s_UPDATE_SPHERE_SLOT, &Image3DCursor::updateSphere, this);
}

//------------------------------------------------------------------------------

Image3DCursor::~Image3DCursor() noexcept
{
}

//------------------------------------------------------------------------------
void Image3DCursor::setVisibility( bool visibility )
{
    m_cursorActor->SetVisibility(visibility);
    this->setVtkPipelineModified();
    this->updating();
}

//------------------------------------------------------------------------------

void Image3DCursor::doConfigure()
{
}

//------------------------------------------------------------------------------

void Image3DCursor::doStart()
{
    m_cursorPolyData = vtkSmartPointer<vtkPolyData>::New();
    m_cursorMapper   = vtkSmartPointer<vtkPolyDataMapper>::New();
    m_cursorActor    = vtkSmartPointer<vtkActor>::New();

    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();

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
        m_cursorActor->GetProperty()->SetColor(1, 1, 1);
    }

    m_cursorMapper->SetInputData( m_cursorPolyData );
    m_cursorActor->SetMapper(m_cursorMapper);

    if(!this->getTransformId().empty())
    {
        m_cursorActor->SetUserTransform(this->getTransform());
    }
    this->addToRenderer(m_cursorActor);
    doUpdate();
}

//------------------------------------------------------------------------------

void Image3DCursor::doUpdate()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    int index[3] = {
        *m_sagittalIndex,
        *m_frontalIndex,
        *m_axialIndex
    };
    double center[3];
    sliceIndexToWorld(index, center);
    this->updateCursorPosition(center);
}

//------------------------------------------------------------------------------

void Image3DCursor::doSwap()
{
    SLM_TRACE_FUNC();
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
}

//------------------------------------------------------------------------------

void Image3DCursor::doStop()
{
    this->removeAllPropFromRenderer();
    m_cursorPolyData = 0;
    m_cursorMapper   = 0;
    m_cursorActor    = 0;
}

//-----------------------------------------------------------------------------

void Image3DCursor::updateSliceIndex(int axial, int frontal, int sagittal)
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

void Image3DCursor::updateSphere(::fwData::Color::sptr color, float radius)
{
    ::fwData::Image::sptr img = this->getObject< ::fwData::Image >();

    m_cursorActor->GetProperty()->SetColor( color->red(), color->green(), color->blue());
    this->buildPolyData(radius);

    m_cursorMapper->SetInputData( m_cursorPolyData );
    this->setVtkPipelineModified();
    this->requestRender();
}

//------------------------------------------------------------------------------

void Image3DCursor::updateCursorPosition( double world[3] )
{
    m_cursorActor->SetPosition(world);
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

void Image3DCursor::buildPolyData(float radius)
{
    // point are stored Left,right,up,down
    vtkSmartPointer<vtkSphereSource> polySource = vtkSmartPointer<vtkSphereSource>::New();
    polySource->SetCenter(0.0, 0.0, 0.0);
    polySource->SetRadius(radius);
    polySource->SetPhiResolution(8);
    polySource->SetThetaResolution(8);

    //vtkSmartPointer<vtkParametricBoy> boyFunc = vtkSmartPointer<vtkParametricBoy>::New();
    //vtkSmartPointer<vtkParametricFunctionSource> polySource = vtkSmartPointer<vtkParametricFunctionSource>::New();
    //polySource->SetParametricFunction(boyFunc);

    polySource->SetOutput(m_cursorPolyData);
    polySource->Update();
    //this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType Image3DCursor::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );

    return connections;
}

//-----------------------------------------------------------------------------

} //namespace visuVTKAdaptor
