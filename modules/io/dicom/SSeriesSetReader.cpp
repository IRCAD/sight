/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
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

#include "SSeriesSetReader.hpp"

#include <core/com/Signal.hxx>
#include <core/jobs/IJob.hpp>
#include <core/jobs/Observer.hpp>
#include <core/location/SingleFolder.hpp>
#include <core/log/Logger.hpp>
#include <core/tools/ProgressToLogger.hpp>

#include <data/SeriesSet.hpp>
#include <data/String.hpp>

#include <io/base/service/IReader.hpp>
#include <io/dicom/reader/SeriesSet.hpp>

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

SSeriesSetReader::SSeriesSetReader() noexcept :
    m_showLogDialog(true),
    m_enableBufferRotation(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = newSignal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::base::service::IOPathType SSeriesSetReader::getIOPathType() const
{
    return sight::io::base::service::FOLDER;
}

//------------------------------------------------------------------------------

void SSeriesSetReader::openLocationDialog()
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
        const auto filterSelectorConfig = service::extension::Config::getDefault()->getServiceConfig(
            m_filterConfig,
            "sight::module::ui::dicom::SFilterSelectorDialog"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_filterConfig
            << " for module::ui::dicom::SFilterSelectorDialog",
            !filterSelectorConfig.empty()
        );

        // Init and execute the service
        service::IService::sptr filterSelectorSrv;
        data::String::sptr key = data::String::New();
        filterSelectorSrv = service::add("sight::module::ui::dicom::SFilterSelectorDialog");
        filterSelectorSrv->setInOut(key, "filter");
        filterSelectorSrv->setConfiguration(filterSelectorConfig);
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

void SSeriesSetReader::configuring()
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

void SSeriesSetReader::starting()
{
}

//------------------------------------------------------------------------------

void SSeriesSetReader::stopping()
{
}

//------------------------------------------------------------------------------

void SSeriesSetReader::info(std::ostream& _sstream)
{
    _sstream << "SSeriesSetReader::info";
}

//------------------------------------------------------------------------------

std::string SSeriesSetReader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::SeriesSet::sptr SSeriesSetReader::createSeriesSet(const std::filesystem::path& dicomDir)
{
    auto reader                 = sight::io::dicom::reader::SeriesSet::New();
    data::SeriesSet::sptr dummy = data::SeriesSet::New();
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
                dummy->clear();
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

void SSeriesSetReader::updating()
{
    m_readFailed = true;

    if(hasLocationDefined())
    {
        auto local_series_set = createSeriesSet(getFolder());

        if(!local_series_set->empty())
        {
            // Retrieve dataStruct associated with this service
            const auto data       = m_data.lock();
            const auto series_set = std::dynamic_pointer_cast<data::SeriesSet>(data.get_shared());

            // Clear SeriesSet and add new series
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
            series_set->shallowCopy(local_series_set);

            m_readFailed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
