/************************************************************************
 *
 * Copyright (C) 2020-2022 IRCAD France
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

#include "SRender.hpp"

#include "viz/qt3d/core/GenericScene.hpp"
#include "viz/qt3d/IAdaptor.hpp"
#include "viz/qt3d/registry/Adaptor.hpp"

#include <core/runtime/operations.hpp>

#include <service/helper/Config.hpp>
#include <service/macros.hpp>
#include <service/op/Add.hpp>

#include <QColor>
#include <QQmlEngine>
#include <QString>

#include <Qt3DExtras/QAbstractCameraController>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/Qt3DWindow>

#include <Qt3DRender/QCamera>
#include <Qt3DRender/QRenderSettings>

namespace sight::viz::qt3d
{

static const std::string s_SCENE_CONFIG            = "scene";
static const std::string s_BACKGROUND_CONFIG       = "background";
static const std::string s_BACKGROUND_COLOR_CONFIG = "color";
static const std::string s_ADAPTOR_CONFIG          = "adaptor";

//-----------------------------------------------------------------------------

SRender::SRender()
{
    // Allow using GenericScene and FrameGraph as QML types when using SRender service in QML applications.
    qmlRegisterType<sight::viz::qt3d::core::GenericScene>("sight.viz.qt3d", 1, 0, "GenericScene");
    qmlRegisterType<sight::viz::qt3d::core::FrameGraph>("sight.viz.qt3d", 1, 0, "FrameGraph");
    qRegisterMetaType<sight::viz::qt3d::core::GenericScene*>("sight.viz.qt3d.GenericScene*");
    qRegisterMetaType<sight::viz::qt3d::core::FrameGraph*>("sight.viz.qt3d.FrameGraph*");
}

//-----------------------------------------------------------------------------

SRender::~SRender()
= default;

//-----------------------------------------------------------------------------

void SRender::configuring()
{
    this->initialize();

    // Get scene configuration
    const ConfigType config = this->getConfigTree();
    const auto sceneCfg     = config.get_child_optional(s_SCENE_CONFIG);
    SIGHT_ASSERT("One scene must be configured.", sceneCfg);

    // Get background tag and its attributes if it is defined.
    const auto backgroundAttr = sceneCfg->get_child_optional(s_BACKGROUND_CONFIG + ".<xmlattr>");
    if(backgroundAttr)
    {
        const std::string color = backgroundAttr->get<std::string>(s_BACKGROUND_COLOR_CONFIG, "#000000");
        m_backgroundColor = QColor(QString::fromStdString(color));
    }

    // Get adaptor tags.
    const auto adaptorConfigs = sceneCfg->equal_range(s_ADAPTOR_CONFIG);
    for(auto it = adaptorConfigs.first ; it != adaptorConfigs.second ; ++it)
    {
        const auto uid = it->second.get<std::string>("<xmlattr>.uid");
        auto& registry = viz::qt3d::registry::getAdaptorRegistry();
        registry[uid] = this->getID();
    }
}

//------------------------------------------------------------------------------

void SRender::starting()
{
    this->create();

    // Instantiates render window manager.
    auto m_interactorManager = viz::qt3d::IWindowInteractor::createManager();
    m_interactorManager->setRenderService(this->getSptr());
    m_interactorManager->createContainer(this->getContainer());

    // Renders a Qt3DWindow which is then displayed as a QWidget.
    m_3dView = new Qt3DExtras::Qt3DWindow();

    // Creates a Qt3D generic scene.
    m_scene = new sight::viz::qt3d::core::GenericScene(false);

    // Configures m_3dView camera and sets it as the scene's camera.
    QPointer<Qt3DRender::QCamera> const camera = m_3dView->camera();
    camera->lens()->setPerspectiveProjection(45.0F, 16.0F / 9.0F, 0.1F, 10000.0F);
    camera->setUpVector(QVector3D(0.0F, 1.0F, 0.0F));
    camera->setPosition(QVector3D(0.0F, 10.0F, 40.0F));
    camera->setViewCenter(QVector3D(0, 0, 0));

    m_scene->setCamera(camera);
    m_scene->getCameraController()->setCamera(camera);

    // Associates Qt3D entity with Qt3D render view.
    m_3dView->setRootEntity(m_scene);

    // Configures rendering parameters.
    m_scene->getFrameGraph()->setClearColor(m_backgroundColor);
    m_3dView->setActiveFrameGraph(m_scene->getFrameGraph());
    m_3dView->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);

    // Converts Qt3D window to QWidget and places it in render service qt container.
    m_interactorManager->set3DView(m_3dView);
}

//------------------------------------------------------------------------------

void SRender::updating()
{
}

//------------------------------------------------------------------------------

void SRender::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::GenericScene* SRender::getScene()
{
    return m_scene;
}

} // namespace sight::viz::qt3d
