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

#include "fwMetrics.hpp"

#include <core/base.hpp>

#include <data/registry/detail.hpp>

#include <service/extension/Factory.hpp>
#include <service/macros.hpp>

#include <ui/__/dialog/message.hpp>

#include <iostream>

namespace sight::module::debug::action
{

//------------------------------------------------------------------------------

fwMetrics::fwMetrics() noexcept =
    default;

//------------------------------------------------------------------------------

fwMetrics::~fwMetrics() noexcept =
    default;

//------------------------------------------------------------------------------

void fwMetrics::updating()
{
    std::stringstream stream;

    stream << "Registered factory keys";
    stream << std::endl;

    stream << "fwData: ";
    stream << data::registry::get()->get_factory_keys().size();
    stream << std::endl;

    stream << "base: ";
    stream << service::extension::Factory::getDefault()->get_factory_keys().size();

    sight::ui::dialog::message messageBox;
    messageBox.setTitle("FactoryRegistry Information");
    messageBox.setMessage(stream.str());
    messageBox.setIcon(ui::dialog::message::INFO);
    messageBox.addButton(ui::dialog::message::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

void fwMetrics::configuring()
{
    this->sight::ui::action::initialize();
}

//------------------------------------------------------------------------------

void fwMetrics::starting()
{
    this->sight::ui::action::actionServiceStarting();
}

//------------------------------------------------------------------------------

void fwMetrics::stopping()
{
    this->sight::ui::action::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace sight::module::debug::action
