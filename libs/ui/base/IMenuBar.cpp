/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "ui/base/IMenuBar.hpp"

#include <core/base.hpp>
#include <core/thread/Worker.hpp>
#include <core/thread/Worker.hxx>
#include <core/tools/fwID.hpp>

#include <service/macros.hpp>
#include <service/registry/ActiveWorkers.hpp>

namespace sight::ui::base
{

IMenuBar::IMenuBar() :
    m_hideMenus(false)
{
}

//-----------------------------------------------------------------------------

IMenuBar::~IMenuBar()
{
}

//-----------------------------------------------------------------------------

void IMenuBar::initialize()
{

    m_registrar = ui::base::registrar::MenuBarRegistrar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    SLM_ASSERT("["+this->getID()+"'] <registry> section is mandatory.", !vectRegistrar.empty() );

    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_registrar->initialize(m_registrarConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    SLM_ASSERT("["+this->getID()+"'] <gui> section is mandatory.", !vectGui.empty() );

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        SLM_ASSERT("["+this->getID()+"'] <layout> section is mandatory.", !vectLayoutMng.empty() );
        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::create()
{
    ui::base::container::fwMenuBar::sptr menuBar = m_registrar->getParent();
    SLM_ASSERT("Parent menuBar is unknown.", menuBar);

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
        {
            m_layoutManager->createLayout(menuBar);
        }) ).wait();

    m_registrar->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenuBar::destroy()
{
    m_registrar->unmanage();

    service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenuBar::menuServiceStopping(std::string menuSrvSID)
{
    ui::base::container::fwMenu::sptr menu = m_registrar->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if (m_hideMenus)
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
            {
                m_layoutManager->menuIsVisible(menu, false);
            }) ).wait();
    }
    else
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >(
                                                                                 [&]
            {
                m_layoutManager->menuIsEnabled(menu, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::menuServiceStarting(std::string menuSrvSID)
{
    ui::base::container::fwMenu::sptr menu = m_registrar->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if (m_hideMenus)
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuIsVisible(menu, true);
            })).wait();
    }
    else
    {
        service::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuIsEnabled(menu, true);
            }) ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBar::initializeLayoutManager(ConfigurationType layoutConfig)
{
    SLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
               layoutConfig->getName() == "layout");

    ui::base::GuiBaseObject::sptr guiObj = ui::base::factory::New(
        ui::base::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY);
    m_layoutManager = ui::base::layoutManager::IMenuBarLayoutManager::dynamicCast(guiObj);
    SLM_ASSERT("ClassFactoryRegistry failed for class "<< ui::base::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY,
               m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
