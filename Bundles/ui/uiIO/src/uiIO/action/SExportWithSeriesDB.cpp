/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "uiIO/action/SExportWithSeriesDB.hpp"

#include <fwCom/Signal.hxx>
#include <fwCom/Slots.hpp>
#include <fwCom/Slots.hxx>

#include <fwGui/Cursor.hpp>

#include <fwIO/ioTypes.hpp>

#include <fwJobs/IJob.hpp>

#include <fwMedData/Series.hpp>
#include <fwMedData/SeriesDB.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

namespace uiIO
{
namespace action
{

fwServicesRegisterMacro( ::fwGui::IActionSrv, ::uiIO::action::SExportWithSeriesDB, ::fwData::Object );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";
static const ::fwCom::Slots::SlotKeyType FORWARD_JOB_SLOT       = "forwardJob";

static const ::fwServices::IService::KeyType s_SERIES_INOUT = "series";

//------------------------------------------------------------------------------

SExportWithSeriesDB::SExportWithSeriesDB( ) noexcept :
    m_ioSelectorSrvConfig("IOSelectorServiceConfigVRRenderReader")
{
    m_sigJobCreated  = newSignal< JobCreatedSignalType >( JOB_CREATED_SIGNAL );
    m_slotForwardJob = newSlot( FORWARD_JOB_SLOT, &SExportWithSeriesDB::forwardJob, this );
}

//------------------------------------------------------------------------------

SExportWithSeriesDB::~SExportWithSeriesDB() noexcept
{
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::info(std::ostream& _sstream )
{
    _sstream << "Action for add SeriesDB" << std::endl;
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::configuring()
{
    this->::fwGui::IActionSrv::initialize();
    std::vector < ConfigurationType > vectConfig = m_configuration->find("IOSelectorSrvConfig");
    if(!vectConfig.empty())
    {
        ConfigurationType selectorConfig = vectConfig.at(0);
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_ioSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::updating( )
{
    ::fwGui::LockAction lock(this->getSptr());

    ::fwMedData::Series::sptr series = this->getInOut< ::fwMedData::Series >(s_SERIES_INOUT);

    if (!series)
    {
        series = this->getObject< ::fwMedData::Series >();
        FW_DEPRECATED_MSG("input 'series' is not correctly set");
    }
    // Create a new SeriesDB
    ::fwMedData::SeriesDB::sptr localSeriesDB = ::fwMedData::SeriesDB::New();
    localSeriesDB->getContainer().push_back(series);

    /// Create IOSelectorService on the new SeriesDB and execute it.

    // Get the config
    ::fwRuntime::ConfigurationElement::csptr ioCfg;
    ioCfg = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(m_ioSelectorSrvConfig,
                                                                                  "::uiIO::editor::SIOSelector");
    SLM_ASSERT("There is no service configuration "
               << m_ioSelectorSrvConfig
               << " for ::uiIO::editor::SIOSelector", ioCfg);

    // Init and execute the service
    ::fwServices::IService::sptr ioSelectorSrv;
    ioSelectorSrv = ::fwServices::add( "::uiIO::editor::SIOSelector");
    ioSelectorSrv->registerInOut(localSeriesDB, ::fwIO::s_DATA_KEY);

    ioSelectorSrv->setWorker(m_associatedWorker);

    auto jobCreatedSignal = ioSelectorSrv->signal("jobCreated");
    if(jobCreatedSignal)
    {
        jobCreatedSignal->connect(m_slotForwardJob);
    }

    ioSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(ioCfg) );
    ioSelectorSrv->configure();
    ioSelectorSrv->start();
    ioSelectorSrv->update();
    ioSelectorSrv->stop();
    ::fwServices::OSR::unregisterService( ioSelectorSrv );
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::starting()
{
    this->::fwGui::IActionSrv::actionServiceStarting();

    ::fwMedData::Series::sptr series = this->getInOut< ::fwMedData::Series >(s_SERIES_INOUT);

    if (!series)
    {
        series = this->getObject< ::fwMedData::Series >();
        FW_DEPRECATED_MSG("input 'series' is not correctly set");
    }
    SLM_FATAL_IF( "The associated object must be a ::fwMedData::Series.", !series);
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::stopping()
{
    this->::fwGui::IActionSrv::actionServiceStopping();
}

//------------------------------------------------------------------------------

void SExportWithSeriesDB::forwardJob(::fwJobs::IJob::sptr iJob)
{
    m_sigJobCreated->emit(iJob);
}

//------------------------------------------------------------------------------
//
} // namespace action
} // namespace uiIO
