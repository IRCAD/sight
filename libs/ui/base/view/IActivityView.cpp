/************************************************************************
 *
 * Copyright (C) 2018-2022 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "ui/base/view/IActivityView.hpp"

#include "ui/base/dialog/MessageDialog.hpp"

#include <activity/IActivityValidator.hpp>
#include <activity/IValidator.hpp>

#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hxx>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>

namespace sight::ui::base::view
{

const core::com::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SLOT = "launchActivity";

//-----------------------------------------------------------------------------

IActivityView::IActivityView()
{
    newSlot(s_LAUNCH_ACTIVITY_SLOT, &IActivityView::launchActivity, this);
}

//-----------------------------------------------------------------------------

IActivityView::~IActivityView()
= default;

//------------------------------------------------------------------------------

void IActivityView::configuring()
{
    this->ui::base::IGuiContainer::initialize();

    const ConfigType config = this->getConfigTree();

    this->parseConfiguration(config, this->getInOuts());
}

//------------------------------------------------------------------------------

bool IActivityView::validateActivity(data::Activity::sptr activity) const
{
    bool isValid = false;
    std::string message;

    std::tie(isValid, message) = activity::IActivityLauncher::validateActivity(activity);

    if(!isValid)
    {
        ui::base::dialog::MessageDialog::show(
            "Activity launch",
            message,
            ui::base::dialog::IMessageDialog::CRITICAL
        );
    }

    return isValid;
}

//------------------------------------------------------------------------------

data::Activity::sptr IActivityView::createMainActivity() const
{
    auto activity = this->activity::IActivityLauncher::createMainActivity();

    if(!activity)
    {
        ui::base::dialog::MessageDialog::show(
            "Main activity",
            "The main activity " + m_mainActivityId + " can not be launched. \n"
                                                      "This activity needs parameters that cannot be defined",
            ui::base::dialog::IMessageDialog::INFO
        );
    }

    return activity;
}

} // namespace sight::ui::base::view
