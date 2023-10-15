/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "orientation_marker.hpp"

#include <data/matrix4.hpp>

#include <viz/scene3d/Layer.hpp>
#include <viz/scene3d/material.hpp>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <OGRE/OgreCamera.h>

#include <OgreEntity.h>

namespace sight::module::viz::scene3d::adaptor
{

//-----------------------------------------------------------------------------

void orientation_marker::configuring()
{
    this->configureParams();

    const config_t config = this->get_config();

    // Set the resource this use, if it has been set via xml
    m_patientMeshRc = config.get<std::string>(s_CONFIG + "resource", m_patientMeshRc);

    m_markerDepth = config.get<float>(s_CONFIG + "depth", m_markerDepth);
}

//-----------------------------------------------------------------------------

void orientation_marker::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    m_sceneNode = rootSceneNode->createChildSceneNode(this->get_id() + "_mainNode");

    Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    // Sets the material
    m_material = std::make_shared<data::material>();

    // Creates the material for the marker
    const sight::module::viz::scene3d::adaptor::material::sptr materialAdaptor =
        this->registerService<sight::module::viz::scene3d::adaptor::material>(
            "sight::module::viz::scene3d::adaptor::material"
        );
    materialAdaptor->set_inout(m_material, sight::module::viz::scene3d::adaptor::material::s_MATERIAL_INOUT, true);
    materialAdaptor->configure(
        this->get_id() + materialAdaptor->get_id(),
        this->get_id() + materialAdaptor->get_id(),
        this->getRenderService(),
        m_layerID
    );
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

void orientation_marker::updating()
{
    this->updateCameraMatrix();
    this->requestRender();
}

//------------------------------------------------------------------------------

void orientation_marker::updateCameraMatrix()
{
    // Copy orientation matrix to Ogre.
    Ogre::Matrix3 ogreMatrix;
    {
        const auto transform = m_matrix.lock();

        // Fill the ogreMatrix.
        for(std::size_t lt = 0 ; lt < 3 ; lt++)
        {
            for(std::size_t ct = 0 ; ct < 3 ; ct++)
            {
                ogreMatrix[ct][lt] = static_cast<Ogre::Real>((*transform)(ct, lt));
            }
        }
    }

    // Convert to quaterion.
    Ogre::Quaternion orientation(ogreMatrix);

    const Ogre::Quaternion rotateX(Ogre::Degree(180), Ogre::Vector3(1, 0, 0));

    // Reset the camera position & orientation, since s_MATRIX_IN is a global transform.
    m_sceneNode->setPosition(0, 0, 0);
    // Reverse X axis.
    m_sceneNode->setOrientation(rotateX);

    // Update the camera position
    // Inverse camera matrix (since we move the mesh)
    m_sceneNode->rotate(orientation.Inverse());
    // Place it at a fixed position
    m_sceneNode->translate(0.F, 0.F, m_markerDepth);
}

//-----------------------------------------------------------------------------

void orientation_marker::stopping()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void orientation_marker::setVisible(bool _visible)
{
    if(m_sceneNode != nullptr)
    {
        m_sceneNode->setVisible(_visible);
    }

    this->updating();
}

//-----------------------------------------------------------------------------

service::connections_t orientation_marker::auto_connections() const
{
    service::connections_t connections;
    connections.push(s_MATRIX_IN, data::matrix4::MODIFIED_SIG, service::slots::UPDATE);
    return connections;
}

//-----------------------------------------------------------------------------

} // namespace sight::module::viz::scene3d::adaptor.
