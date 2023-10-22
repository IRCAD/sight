/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "series_set_reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/location/single_folder.hpp>
#include <core/log/logger.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/series_set.hpp>
#include <data/string.hpp>

#include <io/__/service/reader.hpp>
#include <io/dicom/reader/series_set.hpp>

#include <service/extension/config.hpp>
#include <service/op.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/logger.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>
#include <ui/__/dialog/pulse_progress.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

series_set_reader::series_set_reader() noexcept :
    m_showLogDialog(true),
    m_enableBufferRotation(true),
    m_dicomDirSupport(USER_SELECTION)
{
    m_sigJobCreated = new_signal<JobCreatedSignal>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType series_set_reader::getIOPathType() const
{
    return sight::io::service::FOLDER;
}

//------------------------------------------------------------------------------

void series_set_reader::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? this->getSelectorDialogTitle() : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.setOption(ui::dialog::location::READ);
    dialog_file.setType(ui::dialog::location::FOLDER);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder());
        dialog_file.saveDefaultLocation(default_directory);
    }
    else
    {
        this->clearLocations();
    }

    // Select filter
    if(!m_filterConfig.empty())
    {
        // Get the config
        const auto filter_selector_config = service::extension::config::getDefault()->get_service_config(
            m_filterConfig,
            "sight::module::ui::dicom::filter_selector_dialog"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_filterConfig
            << " for module::ui::dicom::filter_selector_dialog",
            !filter_selector_config.empty()
        );

        // Init and execute the service
        service::base::sptr filter_selector_srv;
        data::string::sptr key = std::make_shared<data::string>();
        filter_selector_srv = service::add("sight::module::ui::dicom::filter_selector_dialog");
        filter_selector_srv->set_inout(key, "filter");
        filter_selector_srv->set_config(filter_selector_config);
        filter_selector_srv->configure();
        filter_selector_srv->start();
        filter_selector_srv->update();
        filter_selector_srv->stop();
        service::remove(filter_selector_srv);

        m_filterType = key->getValue();

        m_readFailed = false;
    }
    else
    {
        m_readFailed = true;
    }
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();

    const service::config_t config = this->get_config();

    // Use filter selector
    m_filterConfig = config.get<std::string>("filterConfig", "");

    // Set filter
    m_filterType = config.get<std::string>("filterType", "");

    // Show log dialog
    m_showLogDialog = config.get<bool>("showLogDialog", true);

    // Enable buffer rotation
    m_enableBufferRotation = config.get<bool>("enableBufferRotation", true);

    // Enable dicomdir
    const std::string dicom_dir_str = config.get<std::string>("dicomdirSupport", "user_selection");
    SIGHT_ASSERT(
        "<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
        dicom_dir_str == "always" || dicom_dir_str == "never" || dicom_dir_str == "user_selection"
    );
    if(dicom_dir_str == "always")
    {
        m_dicomDirSupport = ALWAYS;
    }
    else if(dicom_dir_str == "never")
    {
        m_dicomDirSupport = NEVER;
    }
    else if(dicom_dir_str == "user_selection")
    {
        m_dicomDirSupport = USER_SELECTION;
    }

    // Get SOP Class selection
    if(config.count("op_classSelection") == 1)
    {
        const auto sop_class_selection_config = config.get_child("op_classSelection");
        const auto sop_class_range            = sop_class_selection_config.equal_range("sop_class");
        for(auto sop_class_iter = sop_class_range.first ; sop_class_iter != sop_class_range.second ; ++sop_class_iter)
        {
            const service::config_t& sop_class_config = sop_class_iter->second;
            const service::config_t& sop_class_attr   = sop_class_config.get_child("<xmlattr>");

            SIGHT_ASSERT("Missing attribute 'uid' in element '<sop_class>'", sop_class_attr.count("uid") == 1);
            m_supportedSOPClassSelection.push_back(sop_class_attr.get<std::string>("uid"));
        }
    }
}

//------------------------------------------------------------------------------

void series_set_reader::starting()
{
}

//------------------------------------------------------------------------------

void series_set_reader::stopping()
{
}

//------------------------------------------------------------------------------

void series_set_reader::info(std::ostream& _sstream)
{
    _sstream << "series_set_reader::info";
}

//------------------------------------------------------------------------------

std::string series_set_reader::getSelectorDialogTitle()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::series_set::sptr series_set_reader::createSeriesSet(const std::filesystem::path& _dicom_dir)
{
    auto reader                  = std::make_shared<sight::io::dicom::reader::series_set>();
    data::series_set::sptr dummy = std::make_shared<data::series_set>();
    reader->set_object(dummy);
    reader->set_folder(_dicom_dir);
    reader->set_dicom_filter_type(m_filterType);
    reader->setBufferRotationEnabled(m_enableBufferRotation);
    reader->setsupportedSOPClassContainer(m_supportedSOPClassSelection);
    auto job = reader->getJob();
    m_sigJobCreated->emit(job);

    if(m_dicomDirSupport == USER_SELECTION && reader->isDicomDirAvailable())
    {
        sight::ui::dialog::message message_box;
        message_box.setTitle("Dicomdir file");
        message_box.setMessage(
            "There is a dicomdir file in the root folder. "
            "Would you like to use it for the reading process ?"
        );
        message_box.setIcon(ui::dialog::message::QUESTION);
        message_box.addButton(ui::dialog::message::YES_NO);
        sight::ui::dialog::message::Buttons button = message_box.show();

        reader->setDicomdirActivated(button == sight::ui::dialog::message::YES);
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
        core::log::logger::sptr logger = reader->getLogger();
        logger->sort();

        // Set default cursor
        sight::ui::cursor cursor;
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
            if(!job->cancel_requested())
            {
                result = sight::ui::dialog::logger::showLoggerDialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || job->cancel_requested())
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
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::WARNING
        );
    }
    catch(...)
    {
        m_readFailed = true;
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading",
            sight::ui::dialog::message::WARNING
        );
    }

    return dummy;
}

//------------------------------------------------------------------------------

void series_set_reader::updating()
{
    m_readFailed = true;

    if(hasLocationDefined())
    {
        auto local_series_set = createSeriesSet(get_folder());

        if(!local_series_set->empty())
        {
            // Retrieve dataStruct associated with this service
            const auto data       = m_data.lock();
            const auto series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());

            // Clear series_set and add new series
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
            series_set->shallow_copy(local_series_set);

            m_readFailed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
