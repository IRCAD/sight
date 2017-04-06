/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >( [&]
        {
            m_layoutManager->createLayout(menuBar);
        }) ).wait();

    m_registrar->manage(m_layoutManager->getMenus());
}

//-----------------------------------------------------------------------------

void IMenuBarSrv::destroy()
{
    m_registrar->unmanage();

    ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
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
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >( [&]
            {
                m_layoutManager->menuIsVisible(menu, false);
            }) ).wait();
    }
    else
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >(
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
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
            {
                m_layoutManager->menuIsVisible(menu, true);
            })).wait();
    }
    else
    {
        ::fwServices::registry::ActiveWorkers::getDefaultWorker()->postTask<void>(::boost::function< void() >([&]
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
