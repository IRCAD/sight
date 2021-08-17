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

#include "SSeriesDBReader.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/log/Logger.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/helper/SeriesDB.hpp>
#include <data/mt/ObjectWriteLock.hpp>
#include <data/SeriesDB.hpp>
#include <data/String.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dicom/reader/SeriesDB.hpp>

#include <service/base.hpp>
#include <service/extension/Config.hpp>
#include <service/macros.hpp>

#include <ui/base/Cursor.hpp>
#include <ui/base/dialog/LocationDialog.hpp>
#include <ui/base/dialog/LoggerDialog.hpp>
#include <ui/base/dialog/MessageDialog.hpp>
#include <ui/base/dialog/ProgressDialog.hpp>
#include <ui/base/dialog/PulseProgressDialog.hpp>

namespace sight::module::io::dicom
{

static const core::com::Signals::SignalKeyType JOB_CREATED_SIGNAL = "jobCreated";

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

sight::io::base::service::IOPathType SSeriesDBReader::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configureWithIHM()
{
    this->openLocationDialog();
}

//------------------------------------------------------------------------------

void SSeriesDBReader::openLocationDialog()
{
    static auto defaultDirectory = std::make_shared<core::location::SingleFolder>();

    sight::ui::base::dialog::LocationDialog dialogFile;
    dialogFile.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialogFile.setDefaultLocation(defaultDirectory);
    dialogFile.setOption(ui::base::dialog::ILocationDialog::READ);
    dialogFile.setType(ui::base::dialog::LocationDialog::FOLDER);

    auto result = core::location::SingleFolder::dynamicCast(dialogFile.show());
    if(result)
    {
        this->setFolder(result->getFolder());
        defaultDirectory->setFolder(result->getFolder());
        dialogFile.saveDefaultLocation(defaultDirectory);
    }
    else
    {
        this->clearLocations();
    }

    // Select filter
    if(!m_filterConfig.empty())
    {
        // Get the config
        core::runtime::ConfigurationElement::csptr filterSelectorConfig;
        filterSelectorConfig = service::extension::Config::getDefault()->getServiceConfig(
            m_filterConfig,
            "::sight::module::ui::dicom::SFilterSelectorDialog"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_filterConfig
            << " for module::ui::dicom::SFilterSelectorDialog",
            filterSelectorConfig
        );

        // Init and execute the service
        service::IService::sptr filterSelectorSrv;
        data::String::sptr key = data::String::New();
        filterSelectorSrv = service::add("sight::module::ui::dicom::SFilterSelectorDialog");
        filterSelectorSrv->setInOut(key, "filter");
        filterSelectorSrv->setConfiguration(core::runtime::ConfigurationElement::constCast(filterSelectorConfig));
        filterSelectorSrv->configure();
        filterSelectorSrv->start();
        filterSelectorSrv->update();
        filterSelectorSrv->stop();
        service::remove(filterSelectorSrv);

        m_filterType = key->getValue();

        m_readFailed = false;
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::configuring()
{
    sight::io::base::service::IReader::configuring();

    const service::IService::ConfigType config = this->getConfigTree();

    // Use filter selector
    m_filterConfig = config.get<std::string>("filterConfig", "");

    // Set filter
    m_filterType = config.get<std::string>("filterType", "");

    // Show log dialog
    m_showLogDialog = config.get<bool>("showLogDialog", true);

    // Enable buffer rotation
    m_enableBufferRotation = config.get<bool>("enableBufferRotation", true);

    // Enable dicomdir
    const std::string dicomDirStr = config.get<std::string>("dicomdirSupport", "user_selection");
    SIGHT_ASSERT(
        "<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
        dicomDirStr == "always" || dicomDirStr == "never" || dicomDirStr == "user_selection"
    );
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
    if(config.count("SOPClassSelection") == 1)
    {
        const auto sopClassSelectionConfig = config.get_child("SOPClassSelection");
        const auto sopClassRange           = sopClassSelectionConfig.equal_range("SOPClass");
        for(auto sopClassIter = sopClassRange.first ; sopClassIter != sopClassRange.second ; ++sopClassIter)
        {
            const service::IService::ConfigType& sopClassConfig = sopClassIter->second;
            const service::IService::ConfigType& sopClassAttr   = sopClassConfig.get_child("<xmlattr>");

            SIGHT_ASSERT("Missing attribute 'uid' in element '<SOPClass>'", sopClassAttr.count("uid") == 1);
            m_supportedSOPClassSelection.push_back(sopClassAttr.get<std::string>("uid"));
        }
    }
}

//------------------------------------------------------------------------------

void SSeriesDBReader::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesDBReader::info(std::ostream& _sstream)
{
    _sstream << "SSeriesDBReader::info";
}

//------------------------------------------------------------------------------

std::string SSeriesDBReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::SeriesDB::sptr SSeriesDBReader::createSeriesDB(const std::filesystem::path& dicomDir)
{
    auto reader                = sight::io::dicom::reader::SeriesDB::New();
    data::SeriesDB::sptr dummy = data::SeriesDB::New();
    reader->setObject(dummy);
    reader->setFolder(dicomDir);
    reader->setDicomFilterType(m_filterType);
    reader->setBufferRotationEnabled(m_enableBufferRotation);
    reader->setSupportedSOPClassContainer(m_supportedSOPClassSelection);
    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        sight::ui::base::dialog::MessageDialog messageBox;
        messageBox.setTitle("Dicomdir file");
        messageBox.setMessage(
            "There is a dicomdir file in the root folder. "
            "Would you like to use it for the reading process ?"
        );
        messageBox.setIcon(ui::base::dialog::IMessageDialog::QUESTION);
        messageBox.addButton(ui::base::dialog::IMessageDialog::YES_NO);
        sight::ui::base::dialog::IMessageDialog::Buttons button = messageBox.show();

        reader->setDicomdirActivated(button == sight::ui::base::dialog::IMessageDialog::YES);
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
        core::log::Logger::sptr logger = reader->getLogger();
        logger->sort();

        // Set default cursor
        sight::ui::base::Cursor cursor;
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
                result = sight::ui::base::dialog::LoggerDialog::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancelRequested())
            {
                data::helper::SeriesDB sDBhelper(*dummy);
                sDBhelper.clear();
            }
        }
    }
    catch(const std::exception& e)
    {
        m_readFailed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            ss.str(),
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }
    catch(...)
    {
        m_readFailed = true;
        sight::ui::base::dialog::MessageDialog::show(
            "Warning",
            "Warning during loading",
            sight::ui::base::dialog::IMessageDialog::WARNING
        );
    }

    return dummy;
}

//------------------------------------------------------------------------------

void SSeriesDBReader::updating()
{
    if(this->hasLocationDefined())
    {
        data::SeriesDB::sptr localSeriesDB = this->createSeriesDB(this->getFolder());

        if(!localSeriesDB->empty())
        {
            // Retrieve dataStruct associated with this service
            auto data                     = m_data.lock();
            data::SeriesDB::sptr seriesDB = std::dynamic_pointer_cast<data::SeriesDB>(data.get_shared());

            // Clear SeriesDB and add new series
            data::helper::SeriesDB sDBhelper(*seriesDB);
            sDBhelper.clear();

            // Notify removal.
            sDBhelper.notify();
            {
                seriesDB->shallowCopy(localSeriesDB);
            }

            data::SeriesDB::ContainerType addedSeries = seriesDB->getContainer();

            m_readFailed = false;

            auto sig = seriesDB->signal<data::SeriesDB::AddedSeriesSignalType>(
                data::SeriesDB::s_ADDED_SERIES_SIG
            );
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

} // namespace sight::module::io::dicom
