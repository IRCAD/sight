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

#include "TutoSceneQt3DCpp/Plugin.hpp"

#include <fwRuntime/utils/GenericExecutableFactoryRegistrar.hpp>

#include <QApplication>

#include <Qt3DExtras/Qt3DWindow>

#include <QWidget>

namespace TutoSceneQt3DCpp
{

static ::fwRuntime::utils::GenericExecutableFactoryRegistrar<Plugin> registrar("::TutoSceneQt3DCpp::Plugin");

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
    m_appManager = std::unique_ptr< ::fwServices::AppManager >(new ::fwServices::AppManager );
    m_appManager->create();

    auto frameSrv   = m_appManager->addService("::gui::frame::SDefaultFrame", true, false);
    auto menuBar    = m_appManager->addService("::gui::aspect::SDefaultMenuBar", "menuBar", true, false);
    auto menu       = m_appManager->addService("::gui::aspect::SDefaultMenu", "menuFile", true, false);
    auto actionQuit = m_appManager->addService("::gui::action::SQuit", "actionQuit", true, false);
    auto mainView   = m_appManager->addService("::gui::view::SDefaultView", "mainView", true, false);

    auto renderSrv = m_appManager->addService("::fwRenderQt3D::SRender", "genericScene", true, false);

    // create the frame configuration
    ::fwServices::IService::ConfigType frameConfig;
    frameConfig.put("gui.frame.name", "TutoSceneQt3DCpp");
    frameConfig.put("gui.frame.icon", "TutoSceneQt3DCpp-0.1/tuto.ico");
    frameConfig.put("gui.menuBar", "");
    frameConfig.put("registry.menuBar.<xmlattr>.sid", "menuBar");
    frameConfig.put("registry.view.<xmlattr>.sid", "mainView");
    frameSrv->configure(frameConfig);

    // create the menuBar configuration
    ::fwServices::IService::ConfigType menuBarConfig;
    menuBarConfig.put("gui.layout.menu.<xmlattr>.name", "File");
    menuBarConfig.put("registry.menu.<xmlattr>.sid", "menuFile");
    menuBar->configure(menuBarConfig);

    // create the menu configuration
    ::fwServices::IService::ConfigType menuConfig;
    ::fwServices::IService::ConfigType menuItem;
    menuItem.put("<xmlattr>.name", "Quit");
    menuItem.put("<xmlattr>.shortcut", "Ctrl+Q");
    menuItem.put("<xmlattr>.specialAction", "QUIT");

    menuConfig.add_child("gui.layout.menuItem", menuItem);

    ::fwServices::IService::ConfigType menuItemReg;
    menuItemReg.put("<xmlattr>.sid", "actionQuit");
    menuConfig.add_child("registry.menuItem", menuItemReg);
    menu->configure(menuConfig);

    actionQuit->configure();

    // Main view config
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

    // Render service configuration
    ::fwServices::IService::ConfigType renderConfig;
    ::fwServices::IService::ConfigType backgroundConfig;
    backgroundConfig.put("<xmlattr>.color", "#2d2d2d");
    renderConfig.add_child("scene.background", backgroundConfig);
    renderSrv->configure(renderConfig);

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

} // namespace TutoSceneQt3DCpp
