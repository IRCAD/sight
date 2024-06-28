/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include "modify_layout.hpp"

#include <service/op.hpp>

#include <ui/__/dialog/message.hpp>
#include <ui/__/registry.hpp>
#include <ui/__/service.hpp>
#include <ui/__/toolbar.hpp>

namespace sight::module::ui
{

//-----------------------------------------------------------------------------

void modify_layout::configuring()
{
    this->initialize();

    const auto& config = this->get_config();

    for(const auto& action_cfg : config.get_child("config"))
    {
        if(action_cfg.first == "move")
        {
            const auto uid = action_cfg.second.get<std::string>("<xmlattr>.uid");
            const auto wid = action_cfg.second.get<std::string>("<xmlattr>.wid");

            m_move_srv.emplace_back(uid, wid);
        }
        else if(action_cfg.first == "show"
                || action_cfg.first == "hide"
                || action_cfg.first == "show_or_hide"
                || action_cfg.first == "toggle")
        {
            visibility_t visibility = visibility_t::show;
            if(action_cfg.first == "show")
            {
                visibility = visibility_t::show;
            }
            else if(action_cfg.first == "hide")
            {
                visibility = visibility_t::hide;
            }
            else if(action_cfg.first == "show_or_hide")
            {
                visibility = visibility_t::show_or_hide;
            }
            else if(action_cfg.first == "toggle")
            {
                visibility = visibility_t::toggle;
            }
            else
            {
                SIGHT_FATAL("Unknown visiblity parameter value : " + action_cfg.first)
            }

            if(const auto wid = action_cfg.second.get_optional<std::string>("<xmlattr>.wid"); wid.has_value())
            {
                m_show_srv_wid.emplace_back(wid.value(), visibility);
            }
            else if(const auto sid = action_cfg.second.get_optional<std::string>("<xmlattr>.sid"); sid.has_value())
            {
                m_show_srv_sid.emplace_back(sid.value(), visibility);
            }
            else
            {
                SIGHT_ERROR("Attribute wid or sid missing");
            }
        }
        else if(action_cfg.first == "enable" || action_cfg.first == "disable")
        {
            const auto uid = action_cfg.second.get<std::string>("<xmlattr>.uid");
            bool is_enable = (action_cfg.first == "enable");

            m_enable_srv.emplace_back(uid, is_enable);
        }
        else
        {
            SIGHT_FATAL("Invalid tag name " << action_cfg.first);
        }
    }
}

//-----------------------------------------------------------------------------

void modify_layout::starting()
{
    this->action_service_starting();
}

//-----------------------------------------------------------------------------

void modify_layout::stopping()
{
    this->action_service_stopping();
}

//-----------------------------------------------------------------------------

void modify_layout::info(std::ostream& _sstream)
{
    _sstream << "Starter action" << std::endl;
}

//-----------------------------------------------------------------------------

void modify_layout::updating()
{
    for(const auto& elt : m_move_srv)
    {
        std::string uid = elt.first;
        std::string wid = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::id::exist(uid));
        service::base::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        auto container = std::dynamic_pointer_cast<sight::ui::service>(service);
        SIGHT_ASSERT("::ui::container dynamicCast failed", container);

        container->set_parent(wid);
        service->update();
    }

    for(const auto& elt : m_enable_srv)
    {
        std::string uid = elt.first;
        bool is_enable  = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::id::exist(uid));
        service::base::sptr service = service::get(uid);
        SIGHT_ASSERT("service not found", service);
        if(service->started())
        {
            auto container_srv = std::dynamic_pointer_cast<sight::ui::service>(service);
            if(container_srv)
            {
                container_srv->get_container()->set_enabled(is_enable);
            }

            auto action_srv = std::dynamic_pointer_cast<sight::ui::action>(service);
            if(action_srv)
            {
                action_srv->set_enabled(is_enable);
            }
        }
    }

    for(const auto& elt : m_show_srv_wid)
    {
        const std::string wid                        = elt.first;
        const auto visibility                        = elt.second;
        sight::ui::container::widget::sptr container =
            sight::ui::registry::get_wid_container(wid);
        SIGHT_ASSERT("::ui::container " << wid << " is unknown", container);

        if(visibility == visibility_t::show)
        {
            container->set_visible(true);
        }
        else if(visibility == visibility_t::hide)
        {
            container->set_visible(false);
        }
        else if(visibility == visibility_t::show_or_hide)
        {
            container->set_visible(this->checked());
        }
        else if(visibility == visibility_t::toggle)
        {
            container->set_visible(!container->is_shown_on_screen());
        }
        else
        {
            SIGHT_FATAL("Unknown visiblity parameter value");
        }
    }

    auto set_visible =
        [](service::base::sptr _service, bool _visible)
        {
            auto container_srv = std::dynamic_pointer_cast<sight::ui::service>(_service);
            if(container_srv)
            {
                sight::ui::container::widget::sptr container = container_srv->get_container();
                container->set_visible(_visible);
            }
            else
            {
                auto toolbar_srv = std::dynamic_pointer_cast<sight::ui::toolbar>(_service);
                if(toolbar_srv)
                {
                    toolbar_srv->set_visible(_visible);
                }
                else
                {
                    SIGHT_ASSERT(
                        "Cannot cast service " << std::quoted(_service->get_id()) << " as a UI service.",
                        false
                    );
                }
            }
        };

    auto visible =
        [](service::base::sptr _service)
        {
            if(auto container_srv = std::dynamic_pointer_cast<sight::ui::service>(_service); container_srv)
            {
                sight::ui::container::widget::sptr container = container_srv->get_container();
                return !container->is_shown_on_screen();
            }

            if(auto toolbar_srv = std::dynamic_pointer_cast<sight::ui::toolbar>(_service); toolbar_srv)
            {
                return toolbar_srv->visible();
            }

            SIGHT_ASSERT(
                "Cannot cast service " << std::quoted(_service->get_id()) << " as a UI service.",
                false
            );
            return false;
        };

    for(const auto& elt : m_show_srv_sid)
    {
        std::string uid       = elt.first;
        const auto visibility = elt.second;
        SIGHT_ASSERT(uid << " doesn't exist", core::id::exist(uid));
        service::base::sptr service = service::get(uid);

        if(visibility == visibility_t::show)
        {
            set_visible(service, true);
        }
        else if(visibility == visibility_t::hide)
        {
            set_visible(service, false);
        }
        else if(visibility == visibility_t::show_or_hide)
        {
            set_visible(service, this->checked());
        }
        else if(visibility == visibility_t::toggle)
        {
            set_visible(service, visible(service));
        }
        else
        {
            SIGHT_FATAL("Unknown visiblity parameter value");
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::ui
