/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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

    if(!this->isVersion2())
    {
        std::vector < ::fwRuntime::ConfigurationElement::sptr > seriesCfg = m_configuration->find("seriesId");
        m_seriesId = seriesCfg.front()->getValue();
        SLM_ASSERT("seriesId must not be empty", !m_seriesId.empty());
    }
}

//------------------------------------------------------------------------------

void SExportSeries::starting()
{
    this->actionServiceStarting();
    ::fwMedData::SeriesDB::sptr seriesDB;
    if(this->isVersion2())
    {
        seriesDB = this->getInOut< ::fwMedData::SeriesDB>(s_SERIESDB_INOUT);
    }
    else
    {
        seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    }

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
    ::fwMedData::SeriesDB::sptr seriesDB;
    if(this->isVersion2())
    {
        seriesDB = this->getInOut< ::fwMedData::SeriesDB>(s_SERIESDB_INOUT);
    }
    else
    {
        seriesDB = this->getObject< ::fwMedData::SeriesDB >();
    }
    ::fwMedData::Series::sptr series = this->getSeries();

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
    ::fwMedData::Series::sptr series;

    if(this->isVersion2())
    {
        series = this->getInOut< ::fwMedData::Series>("series");
    }
    else
    {
        SLM_ASSERT("Object " << m_seriesId << " doesn't exist", ::fwTools::fwID::exist(m_seriesId));
        ::fwTools::Object::sptr obj = ::fwTools::fwID::getObject(m_seriesId);
        series                      = ::fwMedData::Series::dynamicCast(obj);
        SLM_ASSERT("Object " << m_seriesId << " is not a '::fwMedData::Series'", series);
    }
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
