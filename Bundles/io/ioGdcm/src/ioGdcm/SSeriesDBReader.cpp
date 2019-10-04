/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ioGdcm/SSeriesDBReader.hpp"

#include <fwCom/Signal.hxx>

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

#include <fwIO/IReader.hpp>

#include <fwJobs/IJob.hpp>
#include <fwJobs/Observer.hpp>

#include <fwLog/Logger.hpp>

#include <fwMedData/SeriesDB.hpp>

#include <fwMedDataTools/helper/SeriesDB.hpp>

#include <fwServices/macros.hpp>
#include <fwServices/op/Add.hpp>
#include <fwServices/registry/ServiceConfig.hpp>

#include <fwTools/ProgressToLogger.hpp>

namespace ioGdcm
{

fwServicesRegisterMacro( ::fwIO::IReader, ::ioGdcm::SSeriesDBReader, ::fwMedData::SeriesDB );

static const ::fwCom::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

SSeriesDBReader::SSeriesDBReader() noexcept :
    m_showLogDialog(true),
    m_enableBufferRotation(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

SSeriesDBReader::~SSeriesDBReader() noexcept
{
}

//------------------------------------------------------------------------------

::fwIO::IOPathType SSeriesDBReader::getIOPathType() const
{
    return ::fwIO::FOLDER;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    static std::filesystem::path _sDefaultPath;

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
    if(!m_filterConfig.empty())
    {
        // Get the config
        ::fwRuntime::ConfigurationElement::csptr filterSelectorConfig;
        filterSelectorConfig = ::fwServices::registry::ServiceConfig::getDefault()->getServiceConfig(
            m_filterConfig, "::ioDicom::SFilterSelectorDialog");

        SLM_ASSERT("Sorry, there is no service configuration "
                   << m_filterConfig
                   << " for ::ioDicom::SFilterSelectorDialog", filterSelectorConfig);

        // Init and execute the service
        ::fwServices::IService::sptr filterSelectorSrv;
        ::fwData::String::sptr key = ::fwData::String::New();
        filterSelectorSrv          = ::fwServices::add(key,
                                                       "::fwGui::editor::IDialogEditor",
                                                       "::ioDicom::SFilterSelectorDialog");
        filterSelectorSrv->setConfiguration( ::fwRuntime::ConfigurationElement::constCast(filterSelectorConfig) );
        filterSelectorSrv->configure();
        filterSelectorSrv->start();
        filterSelectorSrv->update();
        filterSelectorSrv->stop();
        ::fwServices::OSR::unregisterService( filterSelectorSrv );

        m_filterType = key->getValue();
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    ::fwIO::IReader::configuring();

    const ::fwServices::IService::ConfigType config = this->getConfigTree();

    // Use filter selector
    m_filterConfig = config.get<std::string>("filterConfig", "");

    // Set filter
    m_filterType = config.get<std::string>("filterConfig", "");

    // Show log dialog
    m_showLogDialog = config.get<bool>("showLogDialog", true);

    // Enable buffer rotation
    m_enableBufferRotation = config.get<bool>("enableBufferRotation", true);

    // Enable dicomdir
    const std::string dicomDirStr = config.get<std::string>("dicomdirSupport", "user_selection");
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

    // Get SOP Class selection
    if(config.count("SOPClassSelection") == 1 )
    {
        const auto sopClassSelectionConfig = config.get_child("SOPClassSelection");
        const auto sopClassRange           = sopClassSelectionConfig.equal_range("SOPClass");
        for(auto sopClassIter = sopClassRange.first; sopClassIter != sopClassRange.second; ++sopClassIter)
        {
            const ::fwServices::IService::ConfigType& sopClassConfig = sopClassIter->second;
            const ::fwServices::IService::ConfigType& sopClassAttr   = sopClassConfig.get_child("<xmlattr>");

            SLM_ASSERT("Missing attribute 'uid' in element '<SOPClass>'", sopClassAttr.count("uid") == 1);
            m_supportedSOPClassSelection.push_back(sopClassAttr.get<std::string>("uid"));
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
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

::fwMedData::SeriesDB::sptr SSeriesDBReader::createSeriesDB( const std::filesystem::path& dicomDir)
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
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", ss.str(), ::fwGui::dialog::IMessageDialog::WARNING);
    }
    catch( ... )
    {
        m_readFailed = true;
        ::fwGui::dialog::MessageDialog::showMessageDialog(
            "Warning", "Warning during loading", ::fwGui::dialog::IMessageDialog::WARNING);
    }

    return dummy;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    SLM_TRACE_FUNC();
    if( this->hasLocationDefined() )
    {
        ::fwMedData::SeriesDB::sptr localSeriesDB = this->createSeriesDB(this->getFolder());

        if( !localSeriesDB->empty() )
        {
            // Retrieve dataStruct associated with this service
            ::fwMedData::SeriesDB::sptr seriesDB = this->getInOut< ::fwMedData::SeriesDB >(::fwIO::s_DATA_KEY);

            // Clear SeriesDB and add new series
            ::fwMedDataTools::helper::SeriesDB sDBhelper(seriesDB);
            ::fwData::mt::ObjectWriteLock lock(seriesDB);
            sDBhelper.clear();
            // Notify removal.
            sDBhelper.notify();
            {
                ::fwData::mt::ObjectWriteLock lock(localSeriesDB);
                seriesDB->shallowCopy(localSeriesDB);
            }

            ::fwMedData::SeriesDB::ContainerType addedSeries = seriesDB->getContainer();

            auto sig = seriesDB->signal< ::fwMedData::SeriesDB::AddedSeriesSignalType >(
                ::fwMedData::SeriesDB::s_ADDED_SERIES_SIG);
            sig->asyncEmit(addedSeries);
        }
        else
        {
            m_readFailed = true;
        }
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

} // namespace ioGdcm
