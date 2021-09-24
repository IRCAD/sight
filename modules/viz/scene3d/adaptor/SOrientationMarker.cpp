/************************************************************************
 *
 * Copyright (C) 2020-2021 IRCAD France
 * Copyright (C) 2020-2021 IHU Strasbourg
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

#include "SOrientationMarker.hpp"

#include <data/Matrix4.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <viz/scene3d/Layer.hpp>
#include <viz/scene3d/Material.hpp>

#include <OGRE/OgreCamera.h>

#include <OgreEntity.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() noexcept
{
}

//-----------------------------------------------------------------------------

SOrientationMarker::~SOrientationMarker() noexcept
{
}

//-----------------------------------------------------------------------------

void SOrientationMarker::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    // Create the transform node.
    this->setTransformId(this->getID() + "_transform");

    // Set the resource this use, if it has been set via xml
    m_patientMeshRc = config.get<std::string>("resource", m_patientMeshRc);

    m_markerDepth = config.get<float>("depth", m_markerDepth);
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    // Sets the material
    m_material = data::Material::New();

    // Creates the material for the marker
    const sight::module::viz::scene3d::adaptor::SMaterial::sptr materialAdaptor =
        this->registerService<sight::module::viz::scene3d::adaptor::SMaterial>(
            "sight::module::viz::scene3d::adaptor::SMaterial"
        );
    materialAdaptor->setInOut(m_material, sight::module::viz::scene3d::adaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService(this->getRenderService());
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(sight::viz::scene3d::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();
    materialAdaptor->update();

    // Loads and attaches the marker
    m_patientMesh = sceneMgr->createEntity(m_patientMeshRc);
    m_patientMesh->setMaterialName(materialAdaptor->getMaterialName(), sight::viz::scene3d::RESOURCE_GROUP);
    m_sceneNode->attachObject(m_patientMesh);

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::updating()
{
    this->updateCameraMatrix();
    this->requestRender();
}

//------------------------------------------------------------------------------

void SOrientationMarker::updateCameraMatrix()
{
    // Copy orientation matrix to Ogre.
    ::Ogre::Matrix3 ogreMatrix;
    {
        const auto transform = m_matrix.lock();

        // Fill the ogreMatrix.
        for(size_t lt = 0 ; lt < 3 ; lt++)
        {
            for(size_t ct = 0 ; ct < 3 ; ct++)
            {
                ogreMatrix[ct][lt] = static_cast< ::Ogre::Real>(transform->getCoefficient(ct, lt));
            }
        }
    }

    // Get the transformation as sceneNode.
    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);

    // Convert to quaterion.
    ::Ogre::Quaternion orientation(ogreMatrix);

    const ::Ogre::Quaternion rotateX(::Ogre::Degree(180), ::Ogre::Vector3(1, 0, 0));

    // Reset the camera position & orientation, since s_MATRIX_IN is a global transform.
    transformNode->setPosition(0, 0, 0);
    // Reverse X axis.
    transformNode->setOrientation(rotateX);

    // Update the camera position
    // Inverse camera matrix (since we move the mesh)
    transformNode->rotate(orientation.Inverse());
    // Place it at a fixed position
    transformNode->translate(0.f, 0.f, m_markerDepth);
}

//-----------------------------------------------------------------------------

void SOrientationMarker::stopping()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::setVisible(bool _visible)
{
    if(m_sceneNode)
    {
        m_sceneNode->setVisible(_visible);
    }

    this->updating();
}

//-----------------------------------------------------------------------------

service::IService::KeyConnectionsMap SOrientationMarker::getAutoConnections() const
{
    service::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_IN, data::Matrix4::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
