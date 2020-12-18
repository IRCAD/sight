/************************************************************************
 *
 * Copyright (C) 2020 IRCAD France
 * Copyright (C) 2020 IHU Strasbourg
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

#include "fwIO/IReader.hpp"

#include "fwServices/IService.hpp"

namespace visuOgreAdaptor
{

//-----------------------------------------------------------------------------

SOrientationMarker::SOrientationMarker() noexcept
{
    m_mesh = ::fwData::Mesh::New();
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

    // const ConfigType configType = this->getConfigTree();
    // const ConfigType config     = configType.get_child("config.<xmlattr>");
}

//-----------------------------------------------------------------------------

void SOrientationMarker::starting()
{
    this->initialize();

    auto meshReader = this->registerService< ::fwIO::IReader >("::ioVTK::SMeshReader");
    meshReader->registerInOut(m_mesh, "data", false);

    ::fwServices::IService::ConfigType config;
    // config.add("resource", filePath);
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

    // 1. Transfer camera motion from the main scene:
    // - Add a VisuOgreAdaptor::SCamera in xml, it will get the values from the default camera
    // - Pass the associated transformation matrix to this service -> TransformationMatrix3D as param
    // 2. Add an SCamera in the service to transform the mesh
    // - connect the service input matrix to the internal SCamera to allow the transofrmation of the scene to reflect on
    // the human mesh
    // 3. Do the reverse connection to update the main camera from the scene camera

    this->updateVisibility(m_isVisible);

    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::updating()
{
    this->requestRender();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::stopping()
{
    this->unregisterServices();
}

//-----------------------------------------------------------------------------

void SOrientationMarker::setVisible(bool _visible)
{
    this->updating();
}

//-----------------------------------------------------------------------------

} // namespace visuOgreAdaptor.
