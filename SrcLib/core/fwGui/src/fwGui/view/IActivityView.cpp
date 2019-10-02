/************************************************************************
 *
 * Copyright (C) 2018-2019 IRCAD France
 * Copyright (C) 2018-2019 IHU Strasbourg
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

#include "fwGui/view/IActivityView.hpp"

#include "fwGui/dialog/MessageDialog.hpp"

#include <fwActivities/IActivityValidator.hpp>
#include <fwActivities/IValidator.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hxx>

#include <fwData/Composite.hpp>
#include <fwData/String.hpp>

#include <fwDataCamp/getObject.hpp>

#include <fwRuntime/operations.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/UUID.hpp>

namespace fwGui
{
namespace view
{

const ::fwCom::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SLOT        = "launchActivity";
const ::fwCom::Slots::SlotKeyType IActivityView::s_LAUNCH_ACTIVITY_SERIES_SLOT = "launchActivitySeries";

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
    this->::fwGui::IGuiContainerSrv::initialize();

    const ConfigType config = this->getConfigTree();

    this->parseConfiguration(config, this->getInOuts());
}

//------------------------------------------------------------------------------

void IActivityView::launchActivitySeries(fwMedData::Series::sptr series)
{
    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        this->launchActivity(activitySeries);
    }
}

//------------------------------------------------------------------------------

bool IActivityView::validateActivity(fwMedData::ActivitySeries::sptr activitySeries) const
{
    bool isValid;
    std::string message;

    std::tie(isValid, message) = this->::fwActivities::IActivityLauncher::validateActivity(activitySeries);

    if (!isValid)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog("Activity launch",
                                                          message,
                                                          ::fwGui::dialog::IMessageDialog::CRITICAL);
    }

    return isValid;
}

//------------------------------------------------------------------------------

fwMedData::ActivitySeries::sptr IActivityView::createMainActivity() const
{
    ::fwMedData::ActivitySeries::sptr actSeries = this->::fwActivities::IActivityLauncher::createMainActivity();

    if (nullptr == actSeries)
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Main activity",
            "The main activity " + m_mainActivityId + " can not be launched. \n"
            "This activity needs parameters that cannot be defined",
            ::fwGui::dialog::IMessageDialog::INFO);
    }

    return actSeries;
}

} // namespace view
} // namespace fwGui
