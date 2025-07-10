/************************************************************************
 *
 * Copyright (C) 2009-2025 IRCAD France
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

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "job_created";

//------------------------------------------------------------------------------

series_set_reader::series_set_reader() noexcept :
    reader("Choose a directory with DICOM images"),
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL)),
    m_show_log_dialog(true),
    m_enable_buffer_rotation(true),
    m_dicom_dir_support(user_selection)
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t series_set_reader::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void series_set_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::read);
    dialog_file.set_type(ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }

    // Select filter
    if(!m_filter_config.empty())
    {
        // Get the config
        const auto filter_selector_config = service::extension::config::get_default()->get_service_config(
            m_filter_config,
            "sight::module::ui::dicom::filter_selector_dialog"
        );

        SIGHT_ASSERT(
            "Sorry, there is no service configuration "
            << m_filter_config
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

        m_filter_type = key->get_value();

        m_read_failed = false;
    }
    else
    {
        m_read_failed = true;
    }
}

//------------------------------------------------------------------------------

void series_set_reader::configuring()
{
    sight::io::service::reader::configuring();

    const service::config_t config = this->get_config();

    // Use filter selector
    m_filter_config = config.get<std::string>("filterConfig", "");

    // Set filter
    m_filter_type = config.get<std::string>("filterType", "");

    // Show log dialog
    m_show_log_dialog = config.get<bool>("showLogDialog", true);

    // Enable buffer rotation
    m_enable_buffer_rotation = config.get<bool>("enableBufferRotation", true);

    // Enable dicomdir
    const std::string dicom_dir_str = config.get<std::string>("dicomdirSupport", "user_selection");
    SIGHT_ASSERT(
        "<dicomdirSupport> value must be 'always' or 'never' or 'user_selection'",
        dicom_dir_str == "always" || dicom_dir_str == "never" || dicom_dir_str == "user_selection"
    );
    if(dicom_dir_str == "always")
    {
        m_dicom_dir_support = always;
    }
    else if(dicom_dir_str == "never")
    {
        m_dicom_dir_support = never;
    }
    else if(dicom_dir_str == "user_selection")
    {
        m_dicom_dir_support = user_selection;
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
            m_supported_sop_class_selection.push_back(sop_class_attr.get<std::string>("uid"));
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

data::series_set::sptr series_set_reader::create_series_set(const std::filesystem::path& _dicom_dir)
{
    auto reader                  = std::make_shared<sight::io::dicom::reader::series_set>();
    data::series_set::sptr dummy = std::make_shared<data::series_set>();
    reader->set_object(dummy);
    reader->set_folder(_dicom_dir);
    reader->set_dicom_filter_type(m_filter_type);
    reader->set_buffer_rotation_enabled(m_enable_buffer_rotation);
    reader->setsupported_sop_class_container(m_supported_sop_class_selection);
    auto job = reader->get_job();
    m_sig_job_created->emit(job);

    if(m_dicom_dir_support == user_selection && reader->is_dicom_dir_available())
    {
        sight::ui::dialog::message message_box;
        message_box.set_title("Dicomdir file");
        message_box.set_message(
            "There is a dicomdir file in the root folder. "
            "Would you like to use it for the reading process ?"
        );
        message_box.set_icon(ui::dialog::message::question);
        message_box.add_button(ui::dialog::message::yes_no);
        sight::ui::dialog::message::buttons button = message_box.show();

        reader->set_dicomdir_activated(button == sight::ui::dialog::message::yes);
    }
    else if(m_dicom_dir_support == always)
    {
        reader->set_dicomdir_activated(true);
    }
    else //m_dicomDirSupport == NEVER
    {
        reader->set_dicomdir_activated(false);
    }

    try
    {
        reader->read();

        // Retrieve logger
        core::log::logger::sptr logger = reader->get_logger();
        logger->sort();

        // Set default cursor
        sight::ui::cursor cursor;
        cursor.set_default_cursor();

        // Display logger dialog if enabled
        if(m_show_log_dialog && !logger->empty())
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
                result = sight::ui::dialog::logger::show_logger_dialog(
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
        m_read_failed = true;
        std::stringstream ss;
        ss << "Warning during loading : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
        );
    }
    catch(...)
    {
        m_read_failed = true;
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during loading",
            sight::ui::dialog::message::warning
        );
    }

    return dummy;
}

//------------------------------------------------------------------------------

void series_set_reader::updating()
{
    m_read_failed = true;

    if(has_location_defined())
    {
        auto local_series_set = create_series_set(get_folder());

        if(!local_series_set->empty())
        {
            // Retrieve dataStruct associated with this service
            const auto data       = m_data.lock();
            const auto series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());

            // Clear series_set and add new series
            const auto scoped_emitter = series_set->scoped_emit();
            series_set->clear();
            series_set->shallow_copy(local_series_set);

            m_read_failed = false;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
