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

#include "TutoSceneQt3DQml/AppManager.hpp"

#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwMedData/ModelSeries.hpp>

#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ObjectService.hpp>

namespace TutoSceneQt3DQml
{

static const std::string s_MODELSERIES_ID = "modelSeries";

//------------------------------------------------------------------------------

AppManager::AppManager()
{
}

//------------------------------------------------------------------------------

AppManager::~AppManager()
{
}

//------------------------------------------------------------------------------

void AppManager::initialize()
{
    this->create();
}

//------------------------------------------------------------------------------

void AppManager::uninitialize()
{
    this->stopAndUnregisterServices();
}

//------------------------------------------------------------------------------

::fwRenderQt3D::data::Mesh* AppManager::getMesh()
{
    return m_mesh;
}

//------------------------------------------------------------------------------

::fwRenderQt3D::core::GenericScene* AppManager::getScene()
{
    return m_scene;
}

//------------------------------------------------------------------------------

void AppManager::setMesh(::fwRenderQt3D::data::Mesh* _mesh)
{
    m_mesh = _mesh;
}

//------------------------------------------------------------------------------

void AppManager::setScene(::fwRenderQt3D::core::GenericScene* _scene)
{
    m_scene = _scene;
}

//------------------------------------------------------------------------------

void AppManager::onOpenModel()
{
    // Reads a mesh.
    const auto modelSeriesReader = ::fwServices::add("::uiIO::editor::SIOSelector");
    ::fwServices::IService::ConfigType modelSeriesReaderConfig;
    modelSeriesReaderConfig.put("type.<xmlattr>.mode", "reader");
    modelSeriesReaderConfig.put("type.<xmlattr>.class", "::fwMedData::ModelSeries");
    modelSeriesReader->setConfiguration(modelSeriesReaderConfig);
    modelSeriesReader->configure();
    modelSeriesReader->start();
    modelSeriesReader->update();
    auto modelSeries = modelSeriesReader->getOutput< ::fwMedData::ModelSeries >("data");
    this->addObject(modelSeries, s_MODELSERIES_ID);

    // Associates the mesh with the one declared in 'ui.qml'.
    if(modelSeries->getReconstructionDB().size() != 0)
    {
        m_mesh->setScene(m_scene);
        m_mesh->setMesh(modelSeries->getReconstructionDB()[0]->getMesh());
        m_mesh->centerCameraOnMesh();
    }

    modelSeriesReader->stop();
    ::fwServices::OSR::unregisterService(modelSeriesReader);
}

//------------------------------------------------------------------------------

} // namespace TutoSceneQt3DQml.
