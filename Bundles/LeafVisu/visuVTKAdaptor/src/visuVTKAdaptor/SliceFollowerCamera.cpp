/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuVTKAdaptor/SliceFollowerCamera.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwDataTools/fieldHelper/MedicalImageHelpers.hpp>

#include <fwServices/macros.hpp>

#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkInteractorStyleImage.h>
#include <vtkMath.h>
#include <vtkMatrix4x4.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IVtkAdaptorService, ::visuVTKAdaptor::SliceFollowerCamera, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

//------------------------------------------------------------------------------

SliceFollowerCamera::SliceFollowerCamera() noexcept : m_camera(nullptr)
{
    m_comChannelPriority = 0.49;
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SliceFollowerCamera::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SliceFollowerCamera::updateSliceType, this);
}

//------------------------------------------------------------------------------

SliceFollowerCamera::~SliceFollowerCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doConfigure()
{
    SLM_TRACE_FUNC();

    assert(m_configuration->getName() == "config");
    if(m_configuration->hasAttribute("sliceIndex"))
    {
        std::string orientation = m_configuration->getAttributeValue("sliceIndex");
        if(orientation == "axial" )
        {
            m_orientation = Z_AXIS;
        }
        else if(orientation == "frontal" )
        {
            m_orientation = Y_AXIS;
        }
        else if(orientation == "sagittal" )
        {
            m_orientation = X_AXIS;
        }
    }
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doStart()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);

    m_camera = this->getRenderer()->GetActiveCamera();
    this->initializeCamera();
    this->doUpdate();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doUpdate()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    this->initializeCamera();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doSwap()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    this->updateImageInfos(image);
    this->initializeCamera();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::doStop()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SliceFollowerCamera::updateSliceIndex(int axial, int frontal, int sagittal)
{
    this->updateCamera();
}

//-----------------------------------------------------------------------------

void SliceFollowerCamera::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_orientation) )
    {
        setOrientation( static_cast< Orientation >( from ));
    }
    else if(from == static_cast<int>(m_orientation))
    {
        setOrientation( static_cast< Orientation >( to ));
    }
    this->initializeCamera();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::initializeCamera()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        const int orientationToAxe[3] = { 2, 2, 1 };
        double imageSize[3];
        this->getImageSize(imageSize);
        int orientation = orientationToAxe [m_orientation];
        double size     = imageSize[ orientation ];

        double distance = (1.1 * size)
                          / ( std::tan( m_camera->GetViewAngle() * (vtkMath::Pi() / 180.0) ) );

        m_camera->ParallelProjectionOn();
        setVtkPipelineModified();

        this->updateCamera(distance, size);
    }
}

//------------------------------------------------------------------------------

void SliceFollowerCamera::updateCamera(double distance, double size)
{
    SLM_ASSERT("No Camera", m_camera );

    if (distance > 0)
    {
        // Update position according to orientation
        double center[ 3 ];
        double focalPoint[ 3 ];
        double position[ 3 ];
        getCurrentSliceCenter( center );
        std::copy(center, center+3, focalPoint);

        double origin[ 3 ];
        getImageOrigin( origin );
        focalPoint[m_orientation] = origin[m_orientation];

        distance = m_camera->GetDistance();

        std::copy(focalPoint, focalPoint+3, position);

        position[ m_orientation ] -= distance;

        m_camera->SetParallelScale(.51*size);
        m_camera->SetFocalPoint( focalPoint );
        m_camera->SetPosition( position );
    }

    // m_orientation = 0 : 0, 0,1
    // m_orientation = 1 : 0, 0,1
    // m_orientation = 2 : 0,-1,0
    m_camera->SetViewUp(
        0,
        (m_orientation == 2 ? -1 : 0),
        (m_orientation <= 1 ?  1 : 0)
        );
    m_camera->OrthogonalizeViewUp();

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsType SliceFollowerCamera::getObjSrvConnections() const
{
    KeyConnectionsType connections;
    connections.push_back( std::make_pair( ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT ) );
    connections.push_back( std::make_pair( ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT ) );

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
