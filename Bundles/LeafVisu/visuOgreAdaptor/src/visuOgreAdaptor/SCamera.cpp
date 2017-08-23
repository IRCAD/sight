/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
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
    m_nearClipDistance(1.f),
    m_aspectRatio(0.f)
{
    newSlot(s_UPDATE_TF_SLOT, &SCamera::updateTF3D, this);
    newSlot(s_CALIBRATE_SLOT, &SCamera::calibrate, this);
}

//------------------------------------------------------------------------------

SCamera::~SCamera() noexcept
{
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

    this->createTransformService();

    m_layerConnection.connect(this->getLayer(), ::fwRenderOgre::Layer::s_CAMERA_UPDATED_SIG,
                              this->getSptr(), s_UPDATE_TF_SLOT);
    if (m_calibration)
    {
        this->calibrate();
    }
}

//------------------------------------------------------------------------------

void SCamera::updating()
{
}

//------------------------------------------------------------------------------

void SCamera::createTransformService()
{
    auto transform = this->getInput< ::fwData::TransformationMatrix3D >("transform");

    if(!transform)
    {
        transform = ::fwData::TransformationMatrix3D::New();
    }

    m_transformService = ::fwServices::add< ::fwRenderOgre::IAdaptor >(transform, "::visuOgreAdaptor::STransform");
    SLM_ASSERT("Transform service is null", m_transformService.lock());
    auto transformService = this->getTransformService();

    transformService->setID(this->getID() + "_" + transformService->getID());
    transformService->setRenderService(this->getRenderService());
    transformService->setLayerID(m_layerID);
    // For now we use the only one Ogre camera in the layer as input
    transformService->setTransformId(m_camera->getParentSceneNode()->getName());
    transformService->setParentTransformId(this->getParentTransformId());

    transformService->start();
    transformService->connect();
    this->registerService(transformService);

    this->attachNode(m_camera);
}

//-----------------------------------------------------------------------------

void SCamera::attachNode(::Ogre::MovableObject* _node)
{
    auto transformService = this->getTransformService();

    ::Ogre::SceneNode* transNode = transformService->getSceneNode();
    ::Ogre::SceneNode* node      = _node->getParentSceneNode();
    if ((node != transNode) && transNode)
    {
        _node->detachFromParent();
        transNode->attachObject(_node);
    }
}

//------------------------------------------------------------------------------

void SCamera::swapping()
{
    this->updating();
}

//------------------------------------------------------------------------------

void SCamera::stopping()
{
    m_layerConnection.disconnect();

    this->unregisterServices();
}

//------------------------------------------------------------------------------

void SCamera::updateTF3D()
{
    const ::Ogre::SceneNode* camNode      = m_camera->getParentSceneNode();
    const ::Ogre::Quaternion& orientation = camNode->getOrientation();

    ::Ogre::Matrix3 mat33;
    orientation.ToRotationMatrix(mat33);

    const ::Ogre::Vector3& position = camNode->getPosition();
    const ::Ogre::Vector3& scale    = camNode->getScale();

    ::Ogre::Matrix4 newTransMat;

    for( size_t i = 0; i < 3; i++)
    {
        for( size_t j = 0; j < 3; j++)
        {
            // Set the 3x3 matrix representing the rotation
            newTransMat[i][j] = mat33[i][j];

            if( i == j )
            {
                // Set the scale factor
                newTransMat[i][j] = newTransMat[i][j] * scale[i];
            }
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

    this->getTransformService()->setTransform(newTransMat);
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

void SCamera::setNearClipDistance(::Ogre::Real _nearClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_nearClipDistance = _nearClipDistance;
    m_camera->setNearClipDistance(m_nearClipDistance);
}

//------------------------------------------------------------------------------

void SCamera::setFarClipDistance(::Ogre::Real _farClipDistance)
{
    SLM_ASSERT("The associated camera doesn't exist.", m_camera);

    m_farClipDistance = _farClipDistance;
    m_camera->setFarClipDistance(m_farClipDistance);
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
    if ( m_calibration )
    {
        const double fy = m_calibration->getFy();
        m_camera->setFOVy(
            ::Ogre::Radian(static_cast< ::Ogre::Real >(2.0 *
                                                       atan(static_cast< double >(m_calibration->getHeight() / 2.0) /
                                                            fy))));

        this->updating();
    }
}

//------------------------------------------------------------------------------

} //namespace visuOgreAdaptor
