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

#include "visuOgreAdaptor/SOrientationMarker.hpp"

#include "fwDataTools/TransformationMatrix3D.hpp"

#include "fwIO/IReader.hpp"

#include "fwRenderOgre/Layer.hpp"

#include "fwServices/IService.hpp"

#include "visuOgreAdaptor/STransform.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <OGRE/OgreCamera.h>

#include <OgreEntity.h>

namespace visuOgreAdaptor
{

static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_UPDATED_SIG = "visibilityUpdated";

static const std::string s_MATRIX_INOUT = "matrix";

struct SOrientationMarker::CameraListener : public ::Ogre::Camera::Listener
{
    ::Ogre::SceneNode* m_transformNode;

    CameraListener(::Ogre::SceneNode* transformNode)
    {
        m_transformNode = transformNode;
    }

    //------------------------------------------------------------------------------

    void cameraPreRenderScene(::Ogre::Camera* _cam) final
    {
        m_transformNode->setPosition(0.0, 0.0, -24.0);
    }
};

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() noexcept
{
    // newSignal< VisibilityUpdatedSignalType >(s_VISIBILITY_UPDATED_SIG);
}

//-----------------------------------------------------------------------------

SOrientationMarker::~SOrientationMarker() noexcept
{
    m_mesh.reset();
    m_mesh = nullptr;
}

//-----------------------------------------------------------------------------

void SOrientationMarker::configuring()
{
    this->configureParams();

    const ConfigType configType = this->getConfigTree();
    const ConfigType config     = configType.get_child("config.<xmlattr>");

    const std::string transformId = config.get<std::string>(::visuOgreAdaptor::STransform::s_TRANSFORM_CONFIG,
                                                            this->getID() + "_transform");

    this->setTransformId(transformId);

    // Set the resource this use, if it has been set via xml
    m_patientMeshRes = config.get<std::string>("resource", "human.mesh");
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    this->getRenderService()->makeCurrent();

    ::Ogre::SceneNode* const rootSceneNode = this->getSceneManager()->getRootSceneNode();
    // m_sceneNode                            = rootSceneNode->createChildSceneNode(this->getID() + "_mainNode");
    ::Ogre::SceneNode* const transformNode = this->getTransformNode(rootSceneNode);
    m_sceneNode                            = transformNode->createChildSceneNode(this->getID() + "_mainNode");

    ::Ogre::SceneManager* const sceneMgr = this->getSceneManager();

    // Sets the material
    m_material = ::fwData::Material::New();

    // Creates the material for the marker
    const ::visuOgreAdaptor::SMaterial::sptr materialAdaptor = this->registerService< ::visuOgreAdaptor::SMaterial >(
        "::visuOgreAdaptor::SMaterial");
    materialAdaptor->registerInOut(m_material, ::visuOgreAdaptor::SMaterial::s_MATERIAL_INOUT, true);
    materialAdaptor->setID(this->getID() + materialAdaptor->getID());
    materialAdaptor->setMaterialName(this->getID() + materialAdaptor->getID());
    materialAdaptor->setRenderService( this->getRenderService() );
    materialAdaptor->setLayerID(m_layerID);
    materialAdaptor->setMaterialTemplateName(::fwRenderOgre::Material::DEFAULT_MATERIAL_TEMPLATE_NAME);
    materialAdaptor->start();
    materialAdaptor->update();

    // Loads and attaches the marker
    m_patientMesh = sceneMgr->createEntity("human.mesh");
    m_patientMesh->setMaterialName(materialAdaptor->getMaterialName());
    m_sceneNode->attachObject(m_patientMesh);

    // Sets up the camera listener to update the input matrix
    ::Ogre::Camera* m_camera = this->getLayer()->getDefaultCamera();
    m_cameraListener         = new CameraListener(transformNode);
    m_camera->addListener(m_cameraListener);

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::updating()
{
    this->requestRender();
}

//------------------------------------------------------------------------------

void SOrientationMarker::updateCameraMatrix()
{
    const auto inMatrix = this->getLockedInOut< ::fwData::TransformationMatrix3D >(s_MATRIX_INOUT);
    if (inMatrix)
    {
        m_cameraTransform->shallowCopy(inMatrix.get_shared());

        // Convert the matrix to the glm format
        ::glm::dmat4x4 mat = ::fwDataTools::TransformationMatrix3D::getMatrixFromTF3D(inMatrix.get_shared());

        // Nullify the translation vector, to reset the camera to the origin
        mat[3][0] = mat[3][1] = mat[3][2] = 0.0;

        // Translate the camera position to a fixed position
        ::glm::dvec3 v1(0.0, 0.0, -32.0);
        mat = ::glm::translate(mat, v1);

        ::fwDataTools::TransformationMatrix3D::setTF3DFromMatrix(m_cameraTransform, mat);

        auto sig = m_cameraTransform->signal< ::fwData::TransformationMatrix3D::ModifiedSignalType >(
            ::fwData::TransformationMatrix3D::s_MODIFIED_SIG);
        sig->asyncEmit();
    }
}

//-----------------------------------------------------------------------------

void SOrientationMarker::stopping()
{
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

} // namespace visuOgreAdaptor.
