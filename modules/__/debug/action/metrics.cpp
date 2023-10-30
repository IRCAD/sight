/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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

#include "metrics.hpp"

#include <core/base.hpp>

#include <data/registry/detail.hpp>

#include <service/extension/factory.hpp>

#include <ui/__/dialog/message.hpp>

#include <iostream>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

metrics::metrics() noexcept =
    default;

//------------------------------------------------------------------------------

metrics::~metrics() noexcept =
    default;

//------------------------------------------------------------------------------

void metrics::updating()
{
    std::stringstream stream;

    stream << "Registered factory keys";
    stream << std::endl;

    stream << "fwData: ";
    stream << data::registry::get()->get_factory_keys().size();
    stream << std::endl;

    stream << "base: ";
    stream << service::extension::factory::get()->get_factory_keys().size();

    sight::ui::dialog::message message_box;
    message_box.set_title("FactoryRegistry Information");
    message_box.set_message(stream.str());
    message_box.set_icon(ui::dialog::message::info);
    message_box.add_button(ui::dialog::message::ok);
    message_box.show();
}

//------------------------------------------------------------------------------

void metrics::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void metrics::starting()
{
    this->sight::ui::action::action_service_starting();
}

//------------------------------------------------------------------------------

void metrics::stopping()
{
    this->sight::ui::action::action_service_stopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
