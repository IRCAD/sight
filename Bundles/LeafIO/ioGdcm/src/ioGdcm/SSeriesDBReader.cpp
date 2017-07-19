/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "ioGdcm/SSeriesDBReader.hpp"

#include <fwCom/HasSignals.hpp>
#include <fwCom/Signal.hpp>
#include <fwCom/Signal.hxx>

#include <fwCore/base.hpp>

#include <fwData/location/Folder.hpp>
#include <fwData/mt/ObjectWriteLock.hpp>
#include <fwData/String.hpp>

#include <fwGdcmIO/reader/SeriesDB.hpp>

#include <fwGui/Cursor.hpp>
#include <fwGui/dialog/LocationDialog.hpp>
#include <fwGui/dialog/LoggerDialog.hpp>
#include <fwGui/dialog/MessageDialog.hpp>
#include <fwGui/dialog/ProgressDialog.hpp>
#include <fwGui/dialog/PulseProgressDialog.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/ProgressToLogger.hpp>

#include <io/IReader.hpp>

#include <boost/bind.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::io::IReader, ::ioGdcm::SSeriesDBReader, ::fwMedData::SeriesDB );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() throw() :
    m_filterSelectorSrvConfig(""),
    m_filterType(""),
    m_sigJobCreated(JobCreatedSignal::New()),
    m_showLogDialog(true),
    m_enableBufferRotation(true),
    m_dicomDirSupport(USER_SELECTION)
{
    ::fwCom::HasSignals::m_signals
        ( JOB_CREATED_SIGNAL, m_sigJobCreated );
}

//------------------------------------------------------------------------------

SSeriesDBReader::~SSeriesDBReader() throw()
{
}

//------------------------------------------------------------------------------

::io::IOPathType SSeriesDBReader::getIOPathType() const
{
    return ::io::FOLDER;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    static ::boost::filesystem::path _sDefaultPath;

    ::fwGui::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    dialogFile.setOption(::fwGui::dialog::ILocationDialog::READ);
    dialogFile.setType(::fwGui::dialog::LocationDialog::FOLDER);

    ::fwData::location::Folder::sptr result;
    result = ::fwData::location::Folder::dynamicCast( dialogFile.show() );
    if (result)
    {
        _sDefaultPath = result->getFolder();
        this->setFolder( _sDefaultPath );
        dialogFile.saveDefaultLocation( ::fwData::location::Folder::New(_sDefaultPath) );
    }
    else
    {
        this->clearLocations();
    }

    // Select filter
    if(!m_filterSelectorSrvConfig.empty())
    {
        // Get the config
        ::fwRuntime::ConfigurationElement::csptr filterSelectorConfig;
        filterSelectorConfig = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
            m_filterSelectorSrvConfig, "::ioDicom::SFilterSelectorDialog");

        SLM_ASSERT("Sorry, there is no service configuration "
                   << m_filterSelectorSrvConfig
                   << " for ::ioDicom::SFilterSelectorDialog", filterSelectorConfig);

        // Init and execute the service
        ::fwServices::IService::sptr filterSelectorSrv;
        ::fwData::String::sptr key = ::fwData::String::New();
        filterSelectorSrv          = ::fwServices::add(key,
                                                       "::gui::editor::IDialogEditor",
                                                       "::ioDicom::SFilterSelectorDialog");
        filterSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(filterSelectorConfig) );
        filterSelectorSrv->configure();
        filterSelectorSrv->start();
        filterSelectorSrv->update();
        filterSelectorSrv->stop();
        ::fwServices::OSR::unregisterService( filterSelectorSrv );

        m_filterType = key->getValue();

    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring() throw (fwTools::Failed)
{
    ::io::IReader::configuring();

    // Use filter selector
    ::fwRuntime::ConfigurationElement::sptr selectorConfig =
        m_configuration->findConfigurationElement("FilterSelectorSrvConfig");
    if(selectorConfig)
    {
        SLM_ASSERT("Missing 'name' attribute", selectorConfig->hasAttribute("name"));
        m_filterSelectorSrvConfig = selectorConfig->getAttributeValue("name");
    }

    // Get SOP Class selection
    const ::fwRuntime::ConfigurationElement::sptr sopClassSelection =
        m_configuration->findConfigurationElement("SOPClassSelection");
    if(sopClassSelection)
    {
        ::fwRuntime::ConfigurationElementContainer sopClassElements =
            sopClassSelection->findAllConfigurationElement("SOPClass");

        for(::fwRuntime::ConfigurationElementContainer::Iterator it = sopClassElements.begin();
            it != sopClassElements.end(); ++it)
        {
            const ::fwRuntime::ConfigurationElement::AttributePair attributePair = (*it)->getSafeAttributeValue("uid");
            if(attributePair.first)
            {
                SLM_TRACE("New SOP class supported : " + attributePair.second);
                m_supportedSOPClassSelection.push_back(attributePair.second);
            }
        }
    }

    // Set filter
    ::fwRuntime::ConfigurationElement::sptr config = m_configuration->findConfigurationElement("config");
    if(config)
    {
        m_filterType = config->getAttributeValue("filterType");
    }

    // Show log dialog
    ::fwRuntime::ConfigurationElement::sptr logDialog = m_configuration->findConfigurationElement("showLogDialog");
    if(logDialog)
    {
        std::string logDialogStr = logDialog->getValue();
        SLM_ASSERT("<showLogDialog> value must be 'yes' or 'no'", logDialogStr == "yes" || logDialogStr == "no");
        m_showLogDialog = (logDialogStr == "yes");
    }

    // Enable buffer rotation
    ::fwRuntime::ConfigurationElement::sptr bufRot = m_configuration->findConfigurationElement("enableBufferRotation");
    if(bufRot)
    {
        std::string bufRotStr = bufRot->getValue();
        SLM_ASSERT("<enableBufferRotation> value must be 'yes' or 'no'", bufRotStr == "yes" || bufRotStr == "no");
        m_enableBufferRotation = (bufRotStr == "yes");
    }

    // Enable dicomdir
    ::fwRuntime::ConfigurationElement::sptr dicomDir = m_configuration->findConfigurationElement("dicomdirSupport");
    if(dicomDir)
    {
        std::string dicomDirStr = dicomDir->getValue();
        SLM_ASSERT("<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
                   dicomDirStr == "always" || dicomDirStr == "never" || dicomDirStr == "user_selection");

        if(dicomDirStr == "always")
        {
            m_dicomDirSupport = ALWAYS;
        }
        else if(dicomDirStr == "never")
        {
            m_dicomDirSupport = NEVER;
        }
        else if(dicomDirStr == "user_selection")
        {
            m_dicomDirSupport = USER_SELECTION;
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::info(std::ostream& _sstream )
{
    _sstream << "SSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

::fwMedData::SeriesDB::sptr SSeriesDBReader::createSeriesDB( const ::boost::filesystem::path& dicomDir)
{
    SLM_TRACE_FUNC();
    ::fwGdcmIO::reader::SeriesDB::sptr reader = ::fwGdcmIO::reader::SeriesDB::New();
    ::fwMedData::SeriesDB::sptr dummy         = ::fwMedData::SeriesDB::New();
    reader->setObject(dummy);
    reader->setFolder(dicomDir);
    reader->setDicomFilterType(m_filterType);
    reader->setBufferRotationEnabled(m_enableBufferRotation);
    reader->setSupportedSOPClassContainer(m_supportedSOPClassSelection);
    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        ::fwGui::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage( "There is a dicomdir file in the root folder. "
                               "Would you like to use it for the reading process ?" );
        messageBox.setIcon(::fwGui::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(::fwGui::dialog::IMessageDialog::YES_NO);
        ::fwGui::dialog::IMessageDialog::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == ::fwGui::dialog::IMessageDialog::YES);
    }
    else if(m_dicomDirSupport == ALWAYS)
    {
        reader->setDicomdirActivated(true);
    }
    else //m_dicomDirSupport == NEVER
    {
        reader->setDicomdirActivated(false);
    }

    try
    {
        reader->read();

        // Retrieve logger
        ::fwLog::Logger::sptr logger = reader->getLogger();
        logger->sort();

        // Set default cursor
        ::fwGui::Cursor cursor;
        cursor.setDefaultCursor();

        // Display logger dialog if enabled
        if(m_showLogDialog && !logger->empty())
        {

            std::stringstream ss;
            if(dummy->size() > 1)
            {
                ss << "The reading process is over : <b>" << dummy->size() << " series</b> have been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << dummy->size() << " series</b> has been found. "
                    "<br>Please verify the log report to be informed of the potential errors.";
            }

            bool result = false;
            if(!job->cancelRequested())
            {
                result = ::fwGui::dialog::LoggerDialog::showLoggerDialog("Reading process over", ss.str(), logger);
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancelRequested())
            {
                ::fwMedDataTools::helper::SeriesDB sDBhelper(dummy);
                sDBhelper.clear();
            }
        }
    }
    catch (const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return dummy;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwMedData::SeriesDB::sptr localSeriesDB = this->createSeriesDB(this->getFolder());

        if( !localSeriesDB->empty() )
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr seriesDB = this->getObject< ::fwMedData::SeriesDB >();
            // seriesDB->shallowCopy( localSeriesDB ) ;

            ::fwMedDataTools::helper::SeriesDB sDBhelper(seriesDB);

            ::fwData::mt::ObjectWriteLock lock(seriesDB);
            sDBhelper.merge(localSeriesDB);
            sDBhelper.notify();
        }
    }
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
