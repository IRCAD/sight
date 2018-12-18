/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
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

#include <iostream>

#include <fwCore/base.hpp>

#include <fwData/registry/detail.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/registry/ServiceFactory.hpp>

#include <fwGui/dialog/MessageDialog.hpp>

#include "monitor/action/fwMetrics.hpp"

namespace monitor
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::monitor::action::fwMetrics, ::fwData::Object );

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
    stream << ::fwData::registry::get()->getFactoryKeys().size();
    stream << std::endl;

    stream << "IService: ";
    stream << ::fwServices::registry::ServiceFactory::getDefault()->getFactoryKeys().size();

    ::fwGui::dialog::MessageDialog messageBox;
    messageBox.setTitle("FactoryRegistry Information");
    messageBox.setMessage( stream.str() );
    messageBox.setIcon(::fwGui::dialog::IMessageDialog::INFO);
    messageBox.addButton(::fwGui::dialog::IMessageDialog::OK);
    messageBox.show();
}

//------------------------------------------------------------------------------

void fwMetrics::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void fwMetrics::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();
}

//------------------------------------------------------------------------------

void fwMetrics::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------


} // namespace action
} // namespace monitor
