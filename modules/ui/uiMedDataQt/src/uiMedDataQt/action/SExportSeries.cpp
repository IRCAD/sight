/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "uiMedDataQt/action/SExportSeries.hpp"

#include <fwActivities/registry/Activities.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slot.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwCore/base.hpp>

#include <fwGui/dialog/InputDialog.hpp>

#include <fwMedData/ActivitySeries.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>

#include <fwTools/Os.hpp>

namespace uiMedDataQt
{

namespace action
{
//------------------------------------------------------------------------------

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiMedDataQt::action::SExportSeries, ::fwMedData::SeriesDB );

const ::fwCom::Slots::SlotKeyType SExportSeries::s_CHECK_ADDED_SERIES_SLOT   = "checkAddedSeries";
const ::fwCom::Slots::SlotKeyType SExportSeries::s_CHECK_REMOVED_SERIES_SLOT = "CheckRemovesSeries";

const static std::string s_SERIESDB_INOUT = "seriesDB";

//------------------------------------------------------------------------------

SExportSeries::SExportSeries()
{
    newSlot(s_CHECK_ADDED_SERIES_SLOT, &SExportSeries::checkAddedSeries, this);
    newSlot(s_CHECK_REMOVED_SERIES_SLOT, &SExportSeries::checkRemovedSeries, this);
}

//------------------------------------------------------------------------------

SExportSeries::~SExportSeries() noexcept
{
}

//------------------------------------------------------------------------------

::fwServices::IService::KeyConnectionsMap SExportSeries::getAutoConnections() const
{
    KeyConnectionsMap connections;
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG, s_CHECK_ADDED_SERIES_SLOT );
    connections.push( s_SERIESDB_INOUT, ::fwMedData::SeriesDB::s_REMOVED_SERIES_SIG, s_CHECK_REMOVED_SERIES_SLOT );

    return connections;
}

//------------------------------------------------------------------------------

void SExportSeries::configuring()
{
    this->::fwGui::IActionSrv::initialize();
}

//------------------------------------------------------------------------------

void SExportSeries::starting()
{
    this->actionServiceStarting();
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB>(s_SERIESDB_INOUT);

    for( ::fwMedData::Series::sptr series :  seriesDB->getContainer() )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(false);
        }
    }
}

//------------------------------------------------------------------------------

void SExportSeries::stopping()
{
    this->actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExportSeries::updating()
{
    ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB>(s_SERIESDB_INOUT);
    ::fwMedData::Series::sptr series     = this->getSeries();

    std::string description = series->getDescription();

    ::fwMedData::ActivitySeries::sptr activitySeries = ::fwMedData::ActivitySeries::dynamicCast(series);
    if (activitySeries)
    {
        ::fwActivities::registry::Activities::sptr registry = ::fwActivities::registry::Activities::getDefault();
        std::string id = activitySeries->getActivityConfigId();
        OSLM_ASSERT("Activity information not found for" << id, registry->hasInfo(id));

        ::fwActivities::registry::ActivityInfo activityInfo;
        activityInfo = registry->getInfo(id);

        description = activitySeries->getDescription();
        if(description.empty())
        {
            description = activityInfo.description;
        }
    }

    description = ::fwGui::dialog::InputDialog::showInputDialog(
        "Export activity", "Enter the series description", description);

    if(!description.empty())
    {
        ::fwMedData::DicomValuesType physicians = series->getPerformingPhysiciansName();
        if(physicians.empty())
        {
            std::string username = ::fwTools::os::getEnv("USERNAME", fwTools::os::getEnv("LOGNAME", "Unknown"));
            physicians.push_back(username);
        }

        series->setPerformingPhysiciansName(physicians);
        series->setDescription(description);

        ::fwMedDataTools::helper::SeriesDB seriesDBHelper(seriesDB);
        seriesDBHelper.add(series);
        seriesDBHelper.notify();
        this->setIsExecutable(false);
    }
}

//------------------------------------------------------------------------------

void SExportSeries::info(std::ostream& _sstream )
{
    // Update message
    _sstream << std::string("SExportSeries");
}

//------------------------------------------------------------------------------

::fwMedData::Series::sptr SExportSeries::getSeries()
{
    ::fwMedData::Series::sptr series = this->getInOut< ::fwMedData::Series>("series");
    return series;
}

//------------------------------------------------------------------------------

void SExportSeries::checkAddedSeries(::fwMedData::SeriesDB::ContainerType addedSeries)
{
    for( ::fwMedData::Series::sptr series :  addedSeries )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(false);
        }
    }
}

//------------------------------------------------------------------------------

void SExportSeries::checkRemovedSeries(::fwMedData::SeriesDB::ContainerType removedSeries)
{
    for( ::fwMedData::Series::sptr series :  removedSeries )
    {
        if(series == this->getSeries())
        {
            this->setIsExecutable(true);
        }
    }
}

//------------------------------------------------------------------------------

} // namespace action
} // namespace uiMedDataQt
