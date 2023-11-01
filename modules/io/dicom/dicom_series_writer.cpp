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

#include "dicom_series_writer.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/com/signals.hpp>
#include <core/jobs/base.hpp>
#include <core/jobs/observer.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/dicom_series.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/dicom_series_writer.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::dicom
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "job_created";

//------------------------------------------------------------------------------

dicom_series_writer::dicom_series_writer() noexcept :
    m_sig_job_created(new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL))
{
}

//------------------------------------------------------------------------------

dicom_series_writer::~dicom_series_writer() noexcept =
    default;

//------------------------------------------------------------------------------

void dicom_series_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a directory for DICOM images" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.set_option(ui::dialog::location::write);
    dialog_file.set_type(ui::dialog::location::folder);

    auto result = std::dynamic_pointer_cast<core::location::single_folder>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_folder());
        this->set_folder(result->get_folder());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void dicom_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void dicom_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void dicom_series_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void dicom_series_writer::updating()
{
    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto data   = m_data.lock();
        const auto series = std::dynamic_pointer_cast<const data::dicom_series>(data.get_shared());

        const std::filesystem::path& folder = this->get_folder();
        if(!std::filesystem::is_empty(folder))
        {
            sight::ui::dialog::message dialog;
            dialog.set_message(
                "Folder '" + folder.string() + "' isn't empty, files can be overwritten."
                                               "\nDo you want to continue ?"
            );
            dialog.set_title("Folder not empty.");
            dialog.set_icon(ui::dialog::message::question);
            dialog.add_button(sight::ui::dialog::message::yes_no);
            sight::ui::dialog::message::buttons button = dialog.show();

            if(button == sight::ui::dialog::message::no)
            {
                m_write_failed = true;
                return;
            }
        }
        else
        {
            m_write_failed = true;
        }

        if(series->get_modality() == "OT")
        {
            sight::ui::dialog::message dialog;
            dialog.set_message(
                "Series modality is '" + series->get_modality() + "' some information can be lost."
                                                                  "\nDo you want to continue ?"
            );
            dialog.set_title("Series modality.");
            dialog.set_icon(ui::dialog::message::question);
            dialog.add_button(sight::ui::dialog::message::yes_no);
            sight::ui::dialog::message::buttons button = dialog.show();

            if(button == sight::ui::dialog::message::no)
            {
                m_write_failed = true;
                return;
            }
        }

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);
        this->save_dicom_series(folder, series);
        cursor.set_default_cursor();
    }
    else
    {
        m_write_failed = true;
    }
}

//------------------------------------------------------------------------------

void dicom_series_writer::save_dicom_series(
    const std::filesystem::path _folder,
    const data::dicom_series::csptr& _series
) const
{
    auto writer = std::make_shared<sight::io::dicom::helper::dicom_series_writer>();

    writer->set_object(_series);
    writer->set_folder(_folder);
    m_sig_job_created->emit(writer->get_job());

    try
    {
        sight::ui::dialog::progress progress_meter_gui("Saving series ");
        writer->write();
    }
    catch(const std::exception& e)
    {
        std::stringstream ss;
        ss << "Warning during saving : " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
        );
    }
    catch(...)
    {
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::warning
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::path_type_t dicom_series_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::dicom
