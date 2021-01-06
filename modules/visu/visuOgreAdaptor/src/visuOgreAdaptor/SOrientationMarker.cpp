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

#include "fwServices/IService.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace visuOgreAdaptor
{

static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_UPDATED_SIG = "visibilityUpdated";

static const std::string s_MATRIX_INOUT = "matrix";

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() noexcept
{
    m_mesh            = ::fwData::Mesh::New();
    m_cameraTransform = ::fwData::TransformationMatrix3D::New();

    newSignal< VisibilityUpdatedSignalType >(s_VISIBILITY_UPDATED_SIG);
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
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    auto meshReader = this->registerService< ::fwIO::IReader >("::ioVTK::SMeshReader");
    meshReader->registerInOut(m_mesh, "data", false);

    ::fwServices::IService::ConfigType config;
    config.add("resource", "visuOgreAdaptor-" VISUOGREADAPTOR_VER "/human.vtk");

    meshReader->setConfiguration(config);
    meshReader->configure();
    meshReader->start();
    SLM_ASSERT("SMeshReader is not started.", meshReader->isStarted());
    meshReader->update();

    auto meshAdaptor = this->registerService< ::fwRenderOgre::IAdaptor >("::visuOgreAdaptor::SMesh");
    meshAdaptor->setRenderService(this->getRenderService());
    meshAdaptor->registerInOut(m_mesh, "mesh", true);

    ::fwServices::IService::ConfigType configAdp;
    configAdp.add("config.<xmlattr>.layer", this->getLayerID());

    meshAdaptor->setConfiguration(configAdp);
    meshAdaptor->configure();
    meshAdaptor->start();
    SLM_ASSERT("SMesh is not started", meshAdaptor->isStarted());

    m_connections.connect(this->getSptr(), s_VISIBILITY_UPDATED_SIG,
                          meshAdaptor, ::fwRenderOgre::IAdaptor::s_UPDATE_VISIBILITY_SLOT);

    // Initialize the internal matrix for the first time
    this->updateCameraMatrix();

    ::fwServices::IService::ConfigType cameraConfigAdp;
    cameraConfigAdp.add("config.<xmlattr>.layer", this->getLayerID());

    auto cameraAdaptor = this->registerService< ::fwRenderOgre::IAdaptor >("::visuOgreAdaptor::SCamera");
    cameraAdaptor->setRenderService(this->getRenderService());
    cameraAdaptor->registerInOut(m_cameraTransform, "transform", true);
    cameraAdaptor->setConfiguration(cameraConfigAdp);
    cameraAdaptor->configure();
    cameraAdaptor->start();
    SLM_ASSERT("SCamera is not started", cameraAdaptor->isStarted());

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SOrientationMarker::getAutoConnections() const
{
    ::fwServices::IService::KeyConnectionsMap connections;
    connections.push(s_MATRIX_INOUT, ::fwData::Object::s_MODIFIED_SIG, s_UPDATE_SLOT);
    return connections;
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
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::setVisible(bool _visible)
{
    // Forward the visibility update to child services, if auto-connected
    auto sig = this->signal< VisibilityUpdatedSignalType >(s_VISIBILITY_UPDATED_SIG);
    sig->asyncEmit(_visible);

    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
