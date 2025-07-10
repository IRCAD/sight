/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#pragma once

#include <service/base.hpp>
#include <service/op.hpp>

#include <QWidget>

#include <future>
#include <tuple>

namespace sight::module::ui::qt::ut
{

// Utility class to clean a service when destroyed
struct service_cleaner final
{
    service_cleaner(service::base::sptr service) :
        m_service(std::move(service))
    {
    }

    ~service_cleaner()
    {
        if(m_service->started())
        {
            m_service->stop().wait();
        }

        service::remove(m_service);
    }

    service::base::sptr m_service;
};

/// Build a container service and start it
std::tuple<service::base::sptr, std::string> make_container(const std::string& _child_uuid = std::string());

/// Stop and unregister the container service.
void destroy_container(service::base::sptr& _container);

/// Click on a button on a widget
std::shared_future<bool> click_on_button(
    const std::string& _button_name,
    const std::string& _widget_name = std::string()
);

/// Wait for a widget with the predicate
std::shared_future<bool> wait_for_widget(
    std::function<bool(QWidget* widget)> _predicate
);

} // namespace sight::module::ui::qt::ut
