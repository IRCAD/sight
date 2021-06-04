/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
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
#include <core/runtime/operations.hpp>
#include <core/tools/dateAndTime.hpp>
#include <core/tools/UUID.hpp>

#include <data/Composite.hpp>
#include <data/reflection/getObject.hpp>
#include <data/String.hpp>

#include <service/macros.hpp>

namespace sight::ui::base
{

namespace view
{

const core::com::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SLOT        = "launchActivity";
const core::com::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";

//-----------------------------------------------------------------------------

IActivityView::IActivityView()
{
    newSlot(s_LAUNCH_ACTIVITY_SLOT, &IActivityView::launchActivity, this);
    newSlot(s_LAUNCH_ACTIVITY_SERIES_SLOT, &IActivityView::launchActivitySeries, this);
}

//-----------------------------------------------------------------------------

IActivityView::~IActivityView()
{
}

//------------------------------------------------------------------------------

void IActivityView::configuring()
{
    this->ui::base::IGuiContainer::initialize();

    const ConfigType config = this->getConfigTree();

    this->parseConfiguration(config, this->getInOuts());
}

//------------------------------------------------------------------------------

void IActivityView::launchActivitySeries(data::Series::sptr series)
{
    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(series);
    if(activitySeries)
    {
        this->launchActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

bool IActivityView::validateActivity(data::ActivitySeries::sptr activitySeries) const
{
    bool isValid;
    std::string message;

    std::tie(isValid, message) = this->activity::IActivityLauncher::validateActivity(activitySeries);

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

data::ActivitySeries::sptr IActivityView::createMainActivity() const
{
    data::ActivitySeries::sptr actSeries = this->activity::IActivityLauncher::createMainActivity();

    if(nullptr == actSeries)
    {
        ui::base::dialog::MessageDialog::show(
            "Main activity",
            "The main activity " + m_mainActivityId + " can not be launched. \n"
                                                      "This activity needs parameters that cannot be defined",
            ui::base::dialog::IMessageDialog::INFO
        );
    }

    return actSeries;
}

} // namespace view

} // namespace sight::ui::base
