/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "SExport.hpp"

#include <activity/extension/Activity.hpp>

#include <core/base.hpp>
#include <core/com/Slot.hpp>
#include <core/com/Slot.hxx>
#include <core/com/Slots.hpp>
#include <core/com/Slots.hxx>
#include <core/tools/Os.hpp>

#include <data/ActivitySeries.hpp>
#include <data/SeriesDB.hpp>
#include <data/tools/helper/SeriesDB.hpp>

#include <service/macros.hpp>

#include <ui/base/dialog/InputDialog.hpp>

namespace sight::module::ui::base
{

namespace series
{
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::sight::ui::base::IAction, ::sight::module::ui::base::series::SExport,
                         ::sight::data::SeriesDB )

const core::com::Slots::SlotKeyType SExport::s_CHECK_ADDED_SERIES_SLOT = "checkAddedSeries";
const core::com::Slots::SlotKeyType SExport::s_CHECK_REMOVED_SERIES_SLOT = "CheckRemovesSeries";

const static std::string s_SERIESDB_INOUT = "seriesDB";

//------------------------------------------------------------------------------

SExport::SExport()
{
    newSlot(s_CHECK_ADDED_SERIES_SLOT, &SExport::checkAddedSeries, this);
    newSlot(s_CHECK_REMOVED_SERIES_SLOT, &SExport::checkRemovedSeries, this);
}

//------------------------------------------------------------------------------

SExport::~SExport() noexcept
{
}

//------------------------------------------------------------------------------

service::IService::KeyConnectionsMap SExport::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, data::SeriesDB::s_ADDED_SERIES_SIG, s_CHECK_ADDED_SERIES_SLOT );
    connections.push( s_SERIESDB_INOUT, data::SeriesDB::s_REMOVED_SERIES_SIG, s_CHECK_REMOVED_SERIES_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SExport::configuring()
{
    this->sight::ui::base::IAction::initialize();
}

//------------------------------------------------------------------------------

void SExport::starting()
{
    this->actionServiceStarting();
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB>(s_SERIESDB_INOUT);

    for( data::Series::sptr series :  seriesDB->getContainer() )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(false);
        }
    }
}

//------------------------------------------------------------------------------

void SExport::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExport::updating()
{
    data::SeriesDB::sptr seriesDB = this->getInOut< data::SeriesDB>(s_SERIESDB_INOUT);
    data::Series::sptr series     = this->getSeries();

    std::string description = series->getDescription();

    data::ActivitySeries::sptr activitySeries = data::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        activity::extension::Activity::sptr registry = activity::extension::Activity::getDefault();
        std::string id                              = activitySeries->getActivityConfigId();
        SLM_ASSERT("Activity information not found for" << id, registry->hasInfo(id));

        activity::extension::ActivityInfo activityInfo;
        activityInfo = registry->getInfo(id);

        description = activitySeries->getDescription();
        if(description.empty())
        {
            description = activityInfo.description;
        }
    }

    description = sight::ui::base::dialog::InputDialog::showInputDialog(
        "Export activity", "Enter the series description", description);

    if(!description.empty())
    {
        data::DicomValuesType physicians = series->getPerformingPhysiciansName();
        if(physicians.empty())
        {
            std::string username = core::tools::os::getEnv("USERNAME", core::tools::os::getEnv("LOGNAME", "Unknown"));
            physicians.push_back(username);
        }

        series->setPerformingPhysiciansName(physicians);
        series->setDescription(description);

        data::tools::helper::SeriesDB seriesDBHelper(seriesDB);
        seriesDBHelper.add(series);
        seriesDBHelper.notify();
        this->setIsExecutable(false);
    }
}

//------------------------------------------------------------------------------

void SExport::info(std::ostream& _sstream )
{
    // Update message
    _sstream << std::string("SExport");
}

//------------------------------------------------------------------------------

data::Series::sptr SExport::getSeries()
{
    data::Series::sptr series = this->getInOut< data::Series>("series");
    return series;
}

//------------------------------------------------------------------------------

void SExport::checkAddedSeries(data::SeriesDB::ContainerType addedSeries)
{
    for( data::Series::sptr series :  addedSeries )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(false);
        }
    }
}

//------------------------------------------------------------------------------

void SExport::checkRemovedSeries(data::SeriesDB::ContainerType removedSeries)
{
    for( data::Series::sptr series :  removedSeries )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace series
} // namespace sight::module::ui::base
