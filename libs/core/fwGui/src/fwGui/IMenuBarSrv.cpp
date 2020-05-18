/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGui/IMenuBarSrv.hpp"

#include <fwCore/base.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ActiveWorkers.hpp>

#include <fwThread/Worker.hpp>
#include <fwThread/Worker.hxx>

#include <fwTools/fwID.hpp>

namespace fwGui
{

IMenuBarSrv::IMenuBarSrv() :
    m_hideMenus(false)
{
}

//-----------------------------------------------------------------------------

IMenuBarSrv::~IMenuBarSrv()
{
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::initialize()
{

    m_registrar = ::fwGui::registrar::MenuBarRegistrar::New(this->getID());
    // find ViewRegistryManager configuration
    std::vector < ConfigurationType > vectRegistrar = m_configuration->find("registry");
    SLM_ASSERT("Registry section is mandatory.", !vectRegistrar.empty() );

    if(!vectRegistrar.empty())
    {
        m_registrarConfig = vectRegistrar.at(0);
        m_registrar->initialize(m_registrarConfig);
    }

    // find gui configuration
    std::vector < ConfigurationType > vectGui = m_configuration->find("gui");
    SLM_ASSERT("Gui section is mandatory.", !vectGui.empty() );

    if(!vectGui.empty())
    {
        // find LayoutManager configuration
        std::vector < ConfigurationType > vectLayoutMng = vectGui.at(0)->find("layout");
        SLM_ASSERT("layout section is mandatory.", !vectLayoutMng.empty() );
        if(!vectLayoutMng.empty())
        {
            m_layoutConfig = vectLayoutMng.at(0);
            this->initializeLayoutManager(m_layoutConfig);
        }
    }
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::create()
{
    ::fwGui::container::fwMenuBar::sptr menuBar = m_registrar->getParent();
    SLM_ASSERT("Parent menuBar is unknown.", menuBar);

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
        {
            m_layoutManager->createLayout(menuBar);
        }) ).wait();

    m_registrar->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::destroy()
{
    m_registrar->unmanage();

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
        {
            m_layoutManager->destroyLayout();
        })).wait();
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::menuServiceStopping(std::string menuSrvSID)
{
    ::fwGui::container::fwMenu::sptr menu = m_registrar->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if (m_hideMenus)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >( [&]
            {
                m_layoutManager->menuIsVisible(menu, false);
            }) ).wait();
    }
    else
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >(
                                                                                      [&] {
                m_layoutManager->menuIsEnabled(menu, false);
            })).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::menuServiceStarting(std::string menuSrvSID)
{
    ::fwGui::container::fwMenu::sptr menu = m_registrar->getFwMenu(menuSrvSID, m_layoutManager->getMenus());

    if (m_hideMenus)
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuIsVisible(menu, true);
            })).wait();
    }
    else
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(std::function< void() >([&]
            {
                m_layoutManager->menuIsEnabled(menu, true);
            }) ).wait();
    }
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::initializeLayoutManager(ConfigurationType layoutConfig)
{
    OSLM_ASSERT("Bad configuration name "<<layoutConfig->getName()<< ", must be layout",
                layoutConfig->getName() == "layout");

    ::fwGui::GuiBaseObject::sptr guiObj = ::fwGui::factory::New(
        ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY);
    m_layoutManager = ::fwGui::layoutManager::IMenuBarLayoutManager::dynamicCast(guiObj);
    OSLM_ASSERT("ClassFactoryRegistry failed for class "<< ::fwGui::layoutManager::IMenuBarLayoutManager::REGISTRY_KEY,
                m_layoutManager);

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

}
