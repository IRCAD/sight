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
#include "viz/qt3d/core/generic_scene.hpp"
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

static const std::string SCENE_CONFIG            = "scene";
static const std::string BACKGROUND_CONFIG       = "background";
static const std::string BACKGROUND_COLOR_CONFIG = "color";
static const std::string ADAPTOR_CONFIG          = "adaptor";

//-----------------------------------------------------------------------------

render::render()
{
    // Allow using GenericScene and FrameGraph as QML types when using render service in QML applications.
    qmlRegisterType<sight::viz::qt3d::core::generic_scene>("sight.viz.qt3d", 1, 0, "GenericScene");
    qmlRegisterType<sight::viz::qt3d::core::frame_graph>("sight.viz.qt3d", 1, 0, "FrameGraph");
    qRegisterMetaType<sight::viz::qt3d::core::generic_scene*>("sight.viz.qt3d.GenericScene*");
    qRegisterMetaType<sight::viz::qt3d::core::frame_graph*>("sight.viz.qt3d.FrameGraph*");
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
    const auto scene_cfg  = config.get_child_optional(SCENE_CONFIG);
    SIGHT_ASSERT("One scene must be configured.", scene_cfg);

    // Get background tag and its attributes if it is defined.
    const auto background_attr = scene_cfg->get_child_optional(BACKGROUND_CONFIG + ".<xmlattr>");
    if(background_attr)
    {
        const std::string color = background_attr->get<std::string>(BACKGROUND_COLOR_CONFIG, "#000000");
        m_background_color = QColor(QString::fromStdString(color));
    }

    // Get adaptor tags.
    const auto adaptor_configs = scene_cfg->equal_range(ADAPTOR_CONFIG);
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
    auto m_interactor_manager = viz::qt3d::window_interactor::create_manager();
    m_interactor_manager->set_render_service(this->get_sptr());
    m_interactor_manager->create_container(this->get_container());

    // Renders a Qt3DWindow which is then displayed as a QWidget.
    m_3d_view = new Qt3DExtras::Qt3DWindow();

    // Creates a Qt3D generic scene.
    m_scene = new sight::viz::qt3d::core::generic_scene(false);

    // Configures m_3dView camera and sets it as the scene's camera.
    QPointer<Qt3DRender::QCamera> const camera = m_3d_view->camera();
    camera->lens()->setPerspectiveProjection(45.0F, 16.0F / 9.0F, 0.1F, 10000.0F);
    camera->setUpVector(QVector3D(0.0F, 1.0F, 0.0F));
    camera->setPosition(QVector3D(0.0F, 10.0F, 40.0F));
    camera->setViewCenter(QVector3D(0, 0, 0));

    m_scene->set_camera(camera);
    m_scene->get_camera_controller()->setCamera(camera);

    // Associates Qt3D entity with Qt3D render view.
    m_3d_view->setRootEntity(m_scene);

    // Configures rendering parameters.
    m_scene->get_frame_graph()->set_clear_color(m_background_color);
    m_3d_view->setActiveFrameGraph(m_scene->get_frame_graph());
    m_3d_view->renderSettings()->setRenderPolicy(Qt3DRender::QRenderSettings::Always);

    // Converts Qt3D window to QWidget and places it in render service qt container.
    m_interactor_manager->set_3d_view(m_3d_view);
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

sight::viz::qt3d::core::generic_scene* render::get_scene()
{
    return m_scene;
}

} // namespace sight::viz::qt3d
