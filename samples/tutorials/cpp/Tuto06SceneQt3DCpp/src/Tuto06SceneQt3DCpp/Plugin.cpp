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

#include "Tuto06SceneQt3DCpp/Plugin.hpp"

#include <fwData/Mesh.hpp>

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

namespace Tuto06SceneQt3DCpp
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::Tuto06SceneQt3DCpp::Plugin");

//------------------------------------------------------------------------------

Plugin::Plugin() noexcept
{
}

//------------------------------------------------------------------------------

Plugin::~Plugin() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::start()
{
}

//------------------------------------------------------------------------------

void Plugin::initialize()
{
    m_appManager = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager);
    m_appManager->create();

    /* **************************************************************************************
    *              create and register the services in the OSR
    ****************************************************************************************/

    const auto frameSrv       = m_appManager->addService("::gui::frame::SDefaultFrame", true, false);
    const auto menuBar        = m_appManager->addService("::gui::aspect::SDefaultMenuBar", "menuBar", true, false);
    const auto menu           = m_appManager->addService("::gui::aspect::SDefaultMenu", "menuFile", true, false);
    const auto actionOpenMesh = m_appManager->addService("::gui::action::SStarter", "actionOpenMesh", true, false);
    const auto actionQuit     = m_appManager->addService("::gui::action::SQuit", "actionQuit", true, false);
    const auto mainView       = m_appManager->addService("::gui::view::SDefaultView", "mainView", true, false);

    const auto meshReaderSrv = m_appManager->addService("::uiIO::editor::SIOSelector", "meshReader", true, false);
    const auto renderSrv     = m_appManager->addService("::fwRenderQt3D::SRender", "genericScene", true, false);
    const auto meshAdaptor   = m_appManager->addService("::visuQt3DAdaptor::SMesh", "meshAdaptor", true, false);

    /* **************************************************************************************
    *              GUI configuration
    ****************************************************************************************/

    // Creates the frame configuration.
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "Tuto06SceneQt3DCpp");
    frameConfig.put("gui.frame.icon", "Tuto06SceneQt3DCpp-0.2/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameSrv->configure(frameConfig);

    // Creates the menuBar configuration.
    ::fwServices::IService::ConfigType menuBarConfig;
    menuBarConfig.put("gui.layout.menu.<xmlattr>.name", "File");
    menuBarConfig.put("registry.menu.<xmlattr>.sid", "menuFile");
    menuBar->configure(menuBarConfig);

    // Creates the menu configuration.
    ::fwServices::IService::ConfigType menuConfig;
    ::fwServices::IService::ConfigType menuItem1;
    menuItem1.put("<xmlattr>.name", "Open mesh");
    menuItem1.put("<xmlattr>.shortcut", "Ctrl+O");
    ::fwServices::IService::ConfigType menuItem2;
    menuItem2.put("<xmlattr>.name", "Quit");
    menuItem2.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuItem2.put("<xmlattr>.specialAction", "QUIT");

    menuConfig.add_child("gui.layout.menuItem", menuItem1);
    menuConfig.add("gui.layout", "separator");
    menuConfig.add_child("gui.layout.menuItem", menuItem2);

    ::fwServices::IService::ConfigType menuItem1Reg;
    menuItem1Reg.put("<xmlattr>.sid", "actionOpenMesh");
    ::fwServices::IService::ConfigType menuItem2Reg;
    menuItem2Reg.put("<xmlattr>.sid", "actionQuit");
    menuConfig.add_child("registry.menuItem", menuItem1Reg);
    menuConfig.add_child("registry.menuItem", menuItem2Reg);
    menu->configure(menuConfig);

    // Defines main view configuration.
    ::fwServices::IService::ConfigType mainViewConfig;
    ::fwServices::IService::ConfigType mainViewLayoutConfig;
    mainViewLayoutConfig.put("<xmlattr>.type", "::fwGui::CardinalLayoutManager");
    ::fwServices::IService::ConfigType mainView1;
    mainView1.put("<xmlattr>.align", "center");
    mainView1.put("<xmlattr>.minWidth", "1080");
    mainView1.put("<xmlattr>.minHeight", "720");
    mainViewLayoutConfig.add_child("view", mainView1);
    mainViewConfig.add_child("gui.layout", mainViewLayoutConfig);
    ::fwServices::IService::ConfigType mainView1Reg;
    mainView1Reg.put("<xmlattr>.sid", "genericScene");
    mainViewConfig.add_child("registry.view", mainView1Reg);
    mainView->configure(mainViewConfig);

    /* **************************************************************************************
    *              actions configuration
    ****************************************************************************************/

    // Opens a mesh.
    ::fwServices::IService::ConfigType actionOpenMeshConfig;
    actionOpenMeshConfig.add("start.<xmlattr>.uid", "meshReader");
    actionOpenMesh->configure(actionOpenMeshConfig);

    // Quits the application.
    actionQuit->configure();

    /* **************************************************************************************
    *              readers configuration
    ****************************************************************************************/

    ::fwServices::IService::ConfigType meshReaderConfig;
    meshReaderConfig.put("type.<xmlattr>.mode", "reader");
    meshReaderSrv->configure(meshReaderConfig);

    /* **************************************************************************************
    *              genericScene configuration
    ****************************************************************************************/

    // Configures render service.
    ::fwServices::IService::ConfigType renderConfig;
    ::fwServices::IService::ConfigType backgroundConfig;
    backgroundConfig.put("<xmlattr>.color", "#2d2d2d");
    renderConfig.add_child("scene.background", backgroundConfig);
    ::fwServices::IService::ConfigType adpt1Config;
    adpt1Config.put("<xmlattr>.uid", "meshAdaptor");
    renderConfig.add_child("scene.adaptor", adpt1Config);
    renderSrv->configure(renderConfig);

    // Configures mesh adaptor.
    ::fwServices::IService::ConfigType meshAdaptorConfig;
    meshAdaptorConfig.add("config.<xmlattr>.autoresetcamera", "true");
    meshAdaptorConfig.add("config.<xmlattr>.visible", "true");
    meshAdaptor->configure(meshAdaptorConfig);

    /* **************************************************************************************
    *              register inputs/inouts
    ****************************************************************************************/

    ::fwData::Mesh::sptr mesh = ::fwData::Mesh::New();

    meshReaderSrv->registerInOut(mesh, "data");
    meshAdaptor->registerInOut(mesh, "mesh", true);

    m_appManager->addObject(mesh, mesh->getID());

    /* **************************************************************************************
    *              start the services
    ****************************************************************************************/

    m_appManager->startServices();
}

//------------------------------------------------------------------------------

void Plugin::stop() noexcept
{
}

//------------------------------------------------------------------------------

void Plugin::uninitialize() noexcept
{
    m_appManager->destroy();
    m_appManager.reset();
}

//------------------------------------------------------------------------------

} // namespace Tuto06SceneQt3DCpp.
