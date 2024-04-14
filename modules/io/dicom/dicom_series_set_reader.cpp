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

#include "dicom_series_set_reader.hpp"

#include <core/com/signal.hxx>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/tools/system.hpp>

#include <data/series_set.hpp>

#include <io/__/service/reader.hpp>
#include <io/dicom/reader/series_set.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/logger.hpp>
#include <ui/__/dialog/message.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "job_created";

//------------------------------------------------------------------------------

dicom_series_set_reader::dicom_series_set_reader() noexcept :
    m_cancelled(false),
    m_show_log_dialog(true),
    m_dicom_dir_support(user_selection)
{
    m_sig_job_created = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::configuring()
{
    sight::io::service::reader::configuring();

    const auto& config = this->get_config();

    m_show_log_dialog = config.get<bool>("showLogDialog", m_show_log_dialog);

    if(const auto dicom_dir = config.get_optional<std::string>("dicomdirSupport"); dicom_dir.has_value())
    {
        const std::string& dicom_dir_str = dicom_dir.value();
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
    }
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? this->get_selector_dialog_title() : m_window_title);
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
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::starting()
{
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::stopping()
{
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::info(std::ostream& _sstream)
{
    _sstream << "dicom_series_set_reader::info";
}

//------------------------------------------------------------------------------

std::string dicom_series_set_reader::get_selector_dialog_title()
{
    return "Choose a directory with DICOM images";
}

//------------------------------------------------------------------------------

data::series_set::sptr dicom_series_set_reader::create_series_set(const std::filesystem::path& _dicom_dir)
{
    auto reader     = std::make_shared<sight::io::dicom::reader::series_set>();
    auto series_set = std::make_shared<data::series_set>();
    reader->set_object(series_set);
    reader->set_folder(_dicom_dir);

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
        reader->read_dicom_series();

        // Retrieve logger
        core::log::logger::sptr logger = reader->get_logger();
        logger->sort();

        // Display logger dialog if enabled
        if(m_show_log_dialog && !logger->empty())
        {
            std::stringstream ss;
            if(series_set->size() > 1)
            {
                ss << "The reading process is over : <b>" << series_set->size() << " series</b> have been found. "
                                                                                   "<br>Please verify the log report to be informed of the potential errors.";
            }
            else
            {
                ss << "The reading process is over : <b>" << series_set->size() << " series</b> has been found. "
                                                                                   "<br>Please verify the log report to be informed of the potential errors.";
            }

            bool result = false;
            if(!reader->get_job()->cancel_requested())
            {
                result = sight::ui::dialog::logger::show_logger_dialog(
                    "Reading process over",
                    ss.str(),
                    logger
                );
            }

            // If the user cancel the reading process we delete the loaded series
            if(!result || reader->get_job()->cancel_requested())
            {
                const auto scoped_emitter = series_set->scoped_emit();
                series_set->clear();
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

    m_cancelled = job->cancel_requested();

    return series_set;
}

//------------------------------------------------------------------------------

void dicom_series_set_reader::updating()
{
    if(this->has_location_defined())
    {
        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        auto series_set = this->create_series_set(this->get_folder());

        if(!series_set->empty() && !m_cancelled)
        {
            // Retrieve dataStruct associated with this service
            const auto data                  = m_data.lock();
            const auto associated_series_set = std::dynamic_pointer_cast<data::series_set>(data.get_shared());
            SIGHT_ASSERT("associated series_set not instanced", associated_series_set);

            // Clear series_set and add new series
            const auto scoped_emitter = associated_series_set->scoped_emit();

            associated_series_set->clear();
            associated_series_set->shallow_copy(series_set);
        }

        cursor.set_default_cursor();
    }
    else
    {
        m_read_failed = true;
    }
}

//-----------------------------------------------------------------------------

sight::io::service::path_type_t dicom_series_set_reader::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
