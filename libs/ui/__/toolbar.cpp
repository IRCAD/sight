/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/__/toolbar.hpp"

#include "ui/__/action.hpp"
#include "ui/__/menu_item_callback.hpp"

#include <core/com/slot.hxx>
#include <core/com/slots.hxx>
#include <core/thread/worker.hpp>
#include <core/thread/worker.hxx>
#include <core/tools/id.hpp>

#include <service/macros.hpp>
#include <service/op.hpp>

namespace sight::ui
{

//-----------------------------------------------------------------------------

toolbar::toolbar()
{
    new_slot(slots::SET_VISIBLE_SLOT, &toolbar::setVisible, this);
    new_slot(slots::SET_VISIBLE_BY_PARAM_SLOT, &toolbar::setVisibleByParameter, this);
    new_slot(slots::SHOW_SLOT, &toolbar::show, this);
    new_slot(slots::HIDE_SLOT, &toolbar::hide, this);
}

//-----------------------------------------------------------------------------

void toolbar::initialize()
{
    m_registry = ui::detail::registry::ToolBar::make(this->get_id());

    const auto& config = this->get_config();

    // find ViewRegistryManager configuration
    if(const auto registryConfig = config.get_child_optional("registry"); registryConfig.has_value())
    {
        m_registry->initialize(registryConfig.value());
    }

    // find layout configuration
    if(const auto layoutConfig = config.get_child_optional("gui.layout"); layoutConfig.has_value())
    {
        this->initializeLayoutManager(layoutConfig.value());

        m_hideActions = layoutConfig->get<bool>("hideActions", m_hideActions);
    }
}

//-----------------------------------------------------------------------------

void toolbar::create()
{
    ui::container::toolbar::sptr toolBar                = m_registry->getParent();
    std::vector<ui::menu_item_callback::sptr> callbacks = m_registry->getCallbacks();

    SIGHT_ASSERT("Parent toolBar is unknown.", toolBar);
    m_layoutManager->setCallbacks(callbacks);

    const std::string serviceID = get_id().substr(get_id().find_last_of('_') + 1);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->createLayout(toolBar, serviceID);
        })
    ).wait();

    m_registry->manage(m_layoutManager->getMenuItems());
    m_registry->manage(m_layoutManager->getMenus());
    m_registry->manage(m_layoutManager->getContainers());
}

//-----------------------------------------------------------------------------

void toolbar::destroy()
{
    m_registry->unmanage();

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->destroyLayout();
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceStopping(std::string actionSrvSID)
{
    ui::container::menu_item::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    if(m_hideActions)
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetVisible(menuItem, false);
            })
        ).wait();
    }
    else
    {
        core::thread::get_default_worker()->post_task<void>(
            std::function<void()>(
                [&]
            {
                m_layoutManager->menuItemSetEnabled(menuItem, false);
            })
        ).wait();
    }
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceStarting(std::string actionSrvSID)
{
    ui::container::menu_item::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    const service::base::csptr service = service::get(actionSrvSID);
    const ui::action::csptr actionSrv  = std::dynamic_pointer_cast<const ui::action>(service);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, actionSrv->enabled());
            const bool inverted  = actionSrv->inverted();
            const bool isChecked = actionSrv->checked();
            m_layoutManager->menuItemSetChecked(menuItem, inverted ? !isChecked : isChecked);
            m_layoutManager->menuItemSetVisible(menuItem, actionSrv->visible());
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceSetChecked(std::string actionSrvSID, bool isChecked)
{
    ui::container::menu_item::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    const service::base::csptr service = service::get(actionSrvSID);
    const ui::action::csptr actionSrv  = std::dynamic_pointer_cast<const ui::action>(service);

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            const bool inverted = actionSrv->inverted();
            m_layoutManager->menuItemSetChecked(menuItem, inverted ? !isChecked : isChecked);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceSetEnabled(std::string actionSrvSID, bool isEnabled)
{
    ui::container::menu_item::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetEnabled(menuItem, isEnabled);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::actionServiceSetVisible(std::string actionSrvSID, bool isVisible)
{
    ui::container::menu_item::sptr menuItem = m_registry->getFwMenuItem(
        actionSrvSID,
        m_layoutManager->getMenuItems()
    );

    core::thread::get_default_worker()->post_task<void>(
        std::function<void()>(
            [&]
        {
            m_layoutManager->menuItemSetVisible(menuItem, isVisible);
        })
    ).wait();
}

//-----------------------------------------------------------------------------

void toolbar::initializeLayoutManager(const ui::config_t& layoutConfig)
{
    ui::object::sptr guiObj = ui::factory::make(
        ui::layout::toolbar_manager::REGISTRY_KEY
    );
    m_layoutManager = std::dynamic_pointer_cast<ui::layout::toolbar_manager>(guiObj);
    SIGHT_ASSERT(
        "ClassFactoryRegistry failed for class " << ui::layout::toolbar_manager::REGISTRY_KEY,
        m_layoutManager
    );

    m_layoutManager->initialize(layoutConfig);
}

//-----------------------------------------------------------------------------

void toolbar::setVisible(bool isVisible)
{
    m_layoutManager->setVisible(isVisible);
}

//-----------------------------------------------------------------------------

void toolbar::setVisibleByParameter(ui::parameter_t isVisible)
{
    // Only consider boolean alternative, skip all other type of the variant.
    if(std::holds_alternative<bool>(isVisible))
    {
        this->setVisible(std::get<bool>(isVisible));
    }
}

//-----------------------------------------------------------------------------

void toolbar::show()
{
    this->setVisible(true);
}

//-----------------------------------------------------------------------------

void toolbar::hide()
{
    this->setVisible(false);
}

} // namespace sight::ui
