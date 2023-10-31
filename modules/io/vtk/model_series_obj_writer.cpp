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

#include "modules/io/vtk/model_series_obj_writer.hpp"

#include "modules/io/vtk/mesh_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_folder.hpp>
#include <core/tools/uuid.hpp>

#include <data/mesh.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <io/vtk/model_series_obj_writer.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

#include <filesystem>

namespace sight::module::io::vtk
{

static const core::com::signals::key_t JOB_CREATED_SIGNAL = "jobCreated";

//------------------------------------------------------------------------------

model_series_obj_writer::model_series_obj_writer() noexcept
{
    m_sig_job_created = new_signal<job_created_signal_t>(JOB_CREATED_SIGNAL);
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t model_series_obj_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void model_series_obj_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.set_title(m_window_title.empty() ? "Choose a directory to save meshes" : m_window_title);
    dialog.set_default_location(default_directory);
    dialog.set_option(ui::dialog::location::write);
    dialog.set_type(ui::dialog::location::folder);

    core::location::single_folder::sptr result;

    while((result = std::dynamic_pointer_cast<core::location::single_folder>(dialog.show())))
    {
        if(std::filesystem::is_empty(result->get_folder()))
        {
            break;
        }

        // message box
        sight::ui::dialog::message message_box;
        message_box.set_title("Overwrite confirmation");
        message_box.set_message("The selected directory is not empty. Write anyway ?");
        message_box.set_icon(ui::dialog::message::question);
        message_box.add_button(ui::dialog::message::yes);
        message_box.add_button(ui::dialog::message::cancel);
        if(message_box.show() == sight::ui::dialog::message::yes)
        {
            break;
        }
    }

    if(result)
    {
        this->set_folder(result->get_folder());
        default_directory->set_folder(result->get_folder().parent_path());
        dialog.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void model_series_obj_writer::starting()
{
}

//------------------------------------------------------------------------------

void model_series_obj_writer::stopping()
{
}

//------------------------------------------------------------------------------

void model_series_obj_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void model_series_obj_writer::info(std::ostream& _sstream)
{
    _sstream << "model_series_obj_writer::info";
}

//------------------------------------------------------------------------------

void model_series_obj_writer::updating()
{
    m_write_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve dataStruct associated with this service
        const auto locked       = m_data.lock();
        const auto model_series = std::dynamic_pointer_cast<const data::model_series>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::model_series::classname()
            + "' or '"
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            model_series
        );

        auto writer = std::make_shared<sight::io::vtk::model_series_obj_writer>();
        writer->set_object(model_series);
        writer->set_folder(this->get_folder());

        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);

        try
        {
            m_sig_job_created->emit(writer->get_job());
            writer->write();

            m_write_failed = false;
        }
        catch(const std::exception& e)
        {
            std::stringstream ss;
            ss << "Warning during saving : " << e.what();

            sight::ui::dialog::message message_box;
            message_box.set_title("Warning");
            message_box.set_message(ss.str());
            message_box.set_icon(ui::dialog::message::warning);
            message_box.add_button(ui::dialog::message::ok);
            message_box.show();
        }
        catch(...)
        {
            std::stringstream ss;
            ss << "Warning during saving.";

            sight::ui::dialog::message message_box;
            message_box.set_title("Warning");
            message_box.set_message(ss.str());
            message_box.set_icon(ui::dialog::message::warning);
            message_box.add_button(ui::dialog::message::ok);
            message_box.show();
        }

        cursor.set_default_cursor();
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::vtk
