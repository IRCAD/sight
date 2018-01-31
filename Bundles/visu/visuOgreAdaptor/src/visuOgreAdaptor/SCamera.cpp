/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "visuOgreAdaptor/SCamera.hpp"

#include <arData/Camera.hpp>

#include <fwCom/Slots.hxx>

#include <fwData/mt/ObjectReadLock.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/TransformationMatrix3D.hpp>

#include <fwRenderOgre/SRender.hpp>
#include <fwRenderOgre/Utils.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>

#include <OgreCamera.h>
#include <OgreMatrix4.h>
#include <OgreSceneManager.h>
#include <OgreSceneNode.h>
#include <OgreVector3.h>

namespace visuOgreAdaptor
{

fwServicesRegisterMacro(::fwRenderOgre::IAdaptor, ::visuOgreAdaptor::SCamera, ::fwData::TransformationMatrix3D);

//------------------------------------------------------------------------------

const ::fwCom::Slots::SlotKeyType SCamera::s_CALIBRATE_SLOT = "calibrate";
const ::fwCom::Slots::SlotKeyType SCamera::s_UPDATE_TF_SLOT = "updateTransformation";

//------------------------------------------------------------------------------

SCamera::SCamera() noexcept :
    m_camera(nullptr),
    m_aspectRatio(0.f)
{
    newSlot(s_UPDATE_TF_SLOT, &SCamera::updateTF3D, this);
    newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
}

//-----------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap visuOgreAdaptor::SCamera::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push( "transform", ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT );
    connections.push( "calibration", ::arData::Camera::s_INTRINSIC_CALIBRATED_SIG, s_CALIBRATE_SLOT );
    return connections;
}

//------------------------------------------------------------------------------

void SCamera::configuring()
{
    this->configureParams();
}

//------------------------------------------------------------------------------

void SCamera::starting()
{
    this->initialize();

    m_camera      = this->getLayer()->getDefaultCamera();
    m_calibration = this->getInput< ::arData::Camera >("calibration");

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_TF_SLOT);
    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_RANGE_UPDATED_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_RESIZE_LAYER_SIG,
                              this->getSptr(), s_CALIBRATE_SLOT);
    if (m_calibration)
    {
        this->calibrate();
    }

    this->updating();
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    m_layerConnection.disconnect();

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
    ::Ogre::Matrix4 ogreMatrix;

    auto transform = this->getInOut< ::fwData::TransformationMatrix3D >("transform");
    {
        ::fwData::mt::ObjectReadLock lock(transform);

        // Received input lign and column data from f4s transformation matrix
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                ogreMatrix[ct][lt] = static_cast<Ogre::Real>(transform->getCoefficient(ct, lt));
            }
        }
    }

    // Decompose the camera matrix
    ::Ogre::Vector3 position;
    ::Ogre::Vector3 scale;
    ::Ogre::Quaternion orientation;
    ogreMatrix.decomposition(position, scale, orientation);

    // Reverse view-up and direction for AR
    const ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0, 1, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
    orientation = orientation * rotateZ * rotateY;

    ::Ogre::Node* parent = m_camera->getParentNode();

    // Reset the camera position
    parent->setPosition(0, 0, 0);
    parent->setOrientation(Ogre::Quaternion::IDENTITY);

    // Update the camera position
    parent->rotate(orientation);
    parent->translate(position);

    this->requestRender();
}

//------------------------------------------------------------------------------

void SCamera::updateTF3D()
{
    const ::Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
    const ::Ogre::Quaternion& orientation = camNode->getOrientation();

    ::Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    const ::Ogre::Vector3& position = camNode->getPosition();

    ::Ogre::Matrix4 newTransMat;

    for( size_t i = 0; i < 3; i++)
    {
        for( size_t j = 0; j < 3; j++)
        {
            // Set the 3x3 matrix representing the rotation
            newTransMat[i][j] = mat33[i][j];
        }
    }

    for( size_t i = 0; i < 3; i++)
    {
        // Last column represents translation
        newTransMat[i][3] = position[i];

        // Set the last lines to 0
        newTransMat[3][i] = 0;
    }

    // And the last  value
    newTransMat[3][3] = 1;

    // Now nullify the reverse of the view-up and direction
    ::Ogre::Quaternion rotate;
    const ::Ogre::Quaternion rotateY(::Ogre::Degree(180), ::Ogre::Vector3(0, 1, 0));
    const ::Ogre::Quaternion rotateZ(::Ogre::Degree(180), ::Ogre::Vector3(0, 0, 1));
    rotate = rotateZ * rotateY;
    rotate = rotate.Inverse();

    newTransMat = newTransMat * rotate;

    auto transform = this->getInOut< ::fwData::TransformationMatrix3D >("transform");
    {
        ::fwData::mt::ObjectWriteLock lock(transform);

        // Received input lign and column data from f4s transformation matrix
        for (size_t lt = 0; lt < 4; lt++)
        {
            for (size_t ct = 0; ct < 4; ct++)
            {
                transform->setCoefficient(ct, lt, static_cast<double>(newTransMat[ct][lt]));
            }
        }
    }

    auto sig = transform->signal< ::fwData::Object::ModifiedSignalType >(::fwData::Object::s_MODIFIED_SIG);
    {
        ::fwCom::Connection::Blocker block(sig->getConnection(m_slotUpdate));
        sig->asyncEmit();
    }
}

//------------------------------------------------------------------------------

void SCamera::setNearClipDistance(::Ogre::Real _nearClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setNearClipDistance(_nearClipDistance);
}

//------------------------------------------------------------------------------

void SCamera::setFarClipDistance(::Ogre::Real _farClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_camera->setFarClipDistance(_farClipDistance);
}

//-----------------------------------------------------------------------------

void SCamera::setAspectRatio(::Ogre::Real _ratio)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_aspectRatio = _ratio;
    m_camera->setAspectRatio(m_aspectRatio);
}

//-----------------------------------------------------------------------------

void SCamera::calibrate()
{
    if ( m_calibration && m_calibration->getIsCalibrated() )
    {
        const float fx = static_cast< float >(m_calibration->getFx());
        const float fy = static_cast< float >(m_calibration->getFy());

        const float cx = static_cast< float >( m_calibration->getCx() );
        const float cy = static_cast< float >( m_calibration->getCy() );

        //calibration images size
        const float imW = static_cast< float >( m_calibration->getWidth() );
        const float imH = static_cast< float >( m_calibration->getHeight() );

        //displayed image size
        const float winW = static_cast< float >( m_camera->getViewport()->getActualWidth() );
        const float winH = static_cast< float >( m_camera->getViewport()->getActualHeight() );

        //compute ratio between calibration image height & displayed image height
        const float ratioH = winH / imH;

        //compute new fx, fy
        const float nfx = fx * ratioH;
        const float nfy = fy * ratioH;

        const float znear = m_camera->getNearClipDistance();
        const float zfar  = m_camera->getFarClipDistance();

        // compute principle point offset according to size of displayed image
        float px       = ratioH * cx;
        const float py = ratioH * cy;

        const long expectedWindowSize = std::lround(ratioH * imW);

        if( expectedWindowSize != static_cast<long>(winW))
        {
            const long diffX = (static_cast<long>(winW) - expectedWindowSize) / 2;
            px += static_cast<float>(diffX);
        }

        const float cx1 = winW - px;
        const float cy1 = winH - py;

        // compute the offset according to current size
        const float wcx = cx1 / ( (winW - 1.f) / 2.f) -1.f;
        const float wcy = cy1 / ( (winH - 1.f) / 2.f) -1.f;

        // setup projection matrix
        ::Ogre::Matrix4 m = m_camera->getProjectionMatrixWithRSDepth();

        m[0][0] = 2.f * nfx / winW;
        m[0][1] = 0.f;
        m[0][2] = wcx;
        m[0][3] = 0.f;

        m[1][0] = 0.f;
        m[1][1] = 2.f * nfy / winH;
        m[1][2] = -wcy;
        m[1][3] = 0.f;

        m[2][0] = 0.f;
        m[2][1] = 0.f;
        m[2][2] = -(zfar + znear) / (zfar - znear);
        m[2][3] = -2.f * zfar * znear / (zfar - znear);

        m[3][0] = 0.f;
        m[3][1] = 0.f;
        m[3][2] = -1.f;
        m[3][3] = 0.f;

        m_camera->setCustomProjectionMatrix(true, m);

        this->updating();
    }
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
