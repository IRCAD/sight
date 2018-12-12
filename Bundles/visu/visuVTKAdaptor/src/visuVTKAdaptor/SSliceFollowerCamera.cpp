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

#include "visuVTKAdaptor/SSliceFollowerCamera.hpp"

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
#include <vtkRenderer.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkTransform.h>

fwServicesRegisterMacro( ::fwRenderVTK::IAdaptor, ::visuVTKAdaptor::SSliceFollowerCamera, ::fwData::Image );

namespace visuVTKAdaptor
{

static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_INDEX_SLOT = "updateSliceIndex";
static const ::fwCom::Slots::SlotKeyType s_UPDATE_SLICE_TYPE_SLOT  = "updateSliceType";

static const ::fwServices::IService::KeyType s_IMAGE_INOUT = "image";

//------------------------------------------------------------------------------

SSliceFollowerCamera::SSliceFollowerCamera() noexcept :
    m_camera(nullptr)
{
    newSlot(s_UPDATE_SLICE_INDEX_SLOT, &SSliceFollowerCamera::updateSliceIndex, this);
    newSlot(s_UPDATE_SLICE_TYPE_SLOT, &SSliceFollowerCamera::updateSliceType, this);
}

//------------------------------------------------------------------------------

SSliceFollowerCamera::~SSliceFollowerCamera() noexcept
{
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::configuring()
{
    this->configureParams();

    const ConfigType config = this->getConfigTree().get_child("config.<xmlattr>");

    const std::string orientation = config.get<std::string>("sliceIndex", "axial");
    if(orientation == "axial" )
    {
        m_helper.setOrientation(Orientation::Z_AXIS);
    }
    else if(orientation == "frontal" )
    {
        m_helper.setOrientation(Orientation::Y_AXIS);
    }
    else if(orientation == "sagittal" )
    {
        m_helper.setOrientation(Orientation::X_AXIS);
    }
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::starting()
{
    this->initialize();

    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    m_helper.updateImageInfos(image);

    m_camera = this->getRenderer()->GetActiveCamera();
    this->initializeCamera();
    this->updating();
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::updating()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    m_helper.updateImageInfos(image);
    this->initializeCamera();
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::stopping()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SSliceFollowerCamera::updateSliceIndex(int /*axial*/, int /*frontal*/, int /*sagittal*/)
{
    this->updateCamera();
}

//-----------------------------------------------------------------------------

void SSliceFollowerCamera::updateSliceType(int from, int to)
{
    if( to == static_cast<int>(m_helper.getOrientation()) )
    {
        m_helper.setOrientation( static_cast< Orientation >( from ));
    }
    else if(from == static_cast<int>(m_helper.getOrientation()))
    {
        m_helper.setOrientation( static_cast< Orientation >( to ));
    }
    this->initializeCamera();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::initializeCamera()
{
    ::fwData::Image::sptr image = this->getInOut< ::fwData::Image >(s_IMAGE_INOUT);
    SLM_ASSERT("Missing image", image);

    bool imageIsValid = ::fwDataTools::fieldHelper::MedicalImageHelpers::checkImageValidity( image );

    if (imageIsValid)
    {
        const int orientationToAxe[3] = { 2, 2, 1 };
        double imageSize[3];
        m_helper.getImageSize(imageSize);
        const int orientation = orientationToAxe [m_helper.getOrientation()];
        const double size     = imageSize[ orientation ];

        const double distance = (1.1 * size)
                                / ( std::tan( m_camera->GetViewAngle() * (vtkMath::Pi() / 180.0) ) );

        m_camera->ParallelProjectionOn();
        setVtkPipelineModified();

        this->updateCamera(distance, size);
    }
}

//------------------------------------------------------------------------------

void SSliceFollowerCamera::updateCamera(double distance, double size)
{
    SLM_ASSERT("No Camera", m_camera );

    if (distance > 0)
    {
        // Update position according to orientation
        double center[ 3 ];
        double focalPoint[ 3 ];
        double position[ 3 ];
        m_helper.getCurrentSliceCenter( center );
        std::copy(center, center+3, focalPoint);

        double origin[ 3 ];
        m_helper.getImageOrigin( origin );
        focalPoint[m_helper.getOrientation()] = origin[m_helper.getOrientation()];

        distance = m_camera->GetDistance();

        std::copy(focalPoint, focalPoint+3, position);

        position[ m_helper.getOrientation() ] -= distance;

        m_camera->SetParallelScale(.51*size);
        m_camera->SetFocalPoint( focalPoint );
        m_camera->SetPosition( position );
    }

    // m_orientation = 0 : 0, 0,1
    // m_orientation = 1 : 0, 0,1
    // m_orientation = 2 : 0,-1,0
    m_camera->SetViewUp(
        0,
        (m_helper.getOrientation() == 2 ? -1 : 0),
        (m_helper.getOrientation() <= 1 ?  1 : 0)
        );
    m_camera->OrthogonalizeViewUp();

    this->getRenderer()->ResetCameraClippingRange();
    this->setVtkPipelineModified();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SSliceFollowerCamera::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_MODIFIED_SIG, s_UPDATE_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_INDEX_MODIFIED_SIG, s_UPDATE_SLICE_INDEX_SLOT);
    connections.push(s_IMAGE_INOUT, ::fwData::Image::s_SLICE_TYPE_MODIFIED_SIG, s_UPDATE_SLICE_TYPE_SLOT);

    return connections;
}

//------------------------------------------------------------------------------

} //namespace visuVTKAdaptor
