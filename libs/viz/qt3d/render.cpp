/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include "render.hpp"

#include "viz/qt3d/adaptor.hpp"
#include "viz/qt3d/core/GenericScene.hpp"
#include "viz/qt3d/registry/adaptor.hpp"

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

render::render()
{
    // Allow using GenericScene and FrameGraph as QML types when using render service in QML applications.
    qmlRegisterType<sight::viz::qt3d::core::GenericScene>("sight.viz.qt3d", 1, 0, "GenericScene");
    qmlRegisterType<sight::viz::qt3d::core::FrameGraph>("sight.viz.qt3d", 1, 0, "FrameGraph");
    qRegisterMetaType<sight::viz::qt3d::core::GenericScene*>("sight.viz.qt3d.GenericScene*");
    qRegisterMetaType<sight::viz::qt3d::core::FrameGraph*>("sight.viz.qt3d.FrameGraph*");
}

//-----------------------------------------------------------------------------

render::~render()
= default;

//-----------------------------------------------------------------------------

void render::configuring()
{
    this->initialize();

    // Get scene configuration
    const config_t config = this->get_config();
    const auto scene_cfg  = config.get_child_optional(s_SCENE_CONFIG);
    SIGHT_ASSERT("One scene must be configured.", scene_cfg);

    // Get background tag and its attributes if it is defined.
    const auto background_attr = scene_cfg->get_child_optional(s_BACKGROUND_CONFIG + ".<xmlattr>");
    if(background_attr)
    {
        const std::string color = background_attr->get<std::string>(s_BACKGROUND_COLOR_CONFIG, "#000000");
        m_backgroundColor = QColor(QString::fromStdString(color));
    }

    // Get adaptor tags.
    const auto adaptor_configs = scene_cfg->equal_range(s_ADAPTOR_CONFIG);
    for(auto it = adaptor_configs.first ; it != adaptor_configs.second ; ++it)
    {
        const auto uid = it->second.get<std::string>("<xmlattr>.uid");
        auto& registry = viz::qt3d::registry::get_adaptor_registry();
        registry[uid] = this->get_id();
    }
}

//------------------------------------------------------------------------------

void render::starting()
{
    this->create();

    // Instantiates render window manager.
    auto m_interactor_manager = viz::qt3d::window_interactor::createManager();
    m_interactor_manager->setRenderService(this->get_sptr());
    m_interactor_manager->createContainer(this->getContainer());

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
    m_interactor_manager->set3DView(m_3dView);
}

//------------------------------------------------------------------------------

void render::updating()
{
}

//------------------------------------------------------------------------------

void render::stopping()
{
    this->destroy();
}

//------------------------------------------------------------------------------

sight::viz::qt3d::core::GenericScene* render::getScene()
{
    return m_scene;
}

} // namespace sight::viz::qt3d
