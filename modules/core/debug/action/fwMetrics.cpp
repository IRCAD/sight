/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <service/macros.hpp>
#include <service/extension/Factory.hpp>

#include <ui/base/dialog/MessageDialog.hpp>

#include <iostream>

namespace sight::module::debug
{
namespace action
{


//------------------------------------------------------------------------------

fwMetrics::fwMetrics( ) noexcept
{
}

//------------------------------------------------------------------------------

fwMetrics::~fwMetrics() noexcept
{
}

//------------------------------------------------------------------------------

void fwMetrics::updating()
{
    std::stringstream stream;

    stream << "Registered factory keys";
    stream << std::endl;

    stream << "fwData: ";
    stream << data::registry::get()->getFactoryKeys().size();
    stream << std::endl;

    stream << "IService: ";
    stream << service::extension::Factory::getDefault()->getFactoryKeys().size();

    sight::ui::base::dialog::MessageDialog messageBox;
    messageBox.setTitle("FactoryRegistry Information");
    messageBox.setMessage( stream.str() );
    messageBox.setIcon(ui::base::dialog::IMessageDialog::INFO);
    messageBox.addButton(ui::base::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

void fwMetrics::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void fwMetrics::starting()
{
    this->sight::ui::base::IAction::actionServiceStarting();
}

//------------------------------------------------------------------------------

void fwMetrics::stopping()
{
    this->sight::ui::base::IAction::actionServiceStopping();
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace sight::module::debug
