/************************************************************************
 *
 * Copyright (C) 2017-2025 IRCAD France
 * Copyright (C) 2017-2020 IHU Strasbourg
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

#include "surface_segmentation_writer.hpp"

#include <core/location/single_folder.hpp>
#include <core/tools/progress_to_logger.hpp>

#include <data/dicom_series.hpp>
#include <data/series.hpp>
#include <data/vector.hpp>

#include <io/__/service/writer.hpp>
#include <io/dicom/helper/fiducial.hpp>
#include <io/dicom/writer/series.hpp>
#include <io/dicom/writer/surface_segmentation.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>
#include <ui/__/dialog/selector.hpp>

#include <string>

namespace sight::module::io::dicom
{

//------------------------------------------------------------------------------

surface_segmentation_writer::surface_segmentation_writer() noexcept :
    writer("Choose a directory for DICOM images")
{
}

//------------------------------------------------------------------------------

surface_segmentation_writer::~surface_segmentation_writer() noexcept =
    default;

//------------------------------------------------------------------------------

void surface_segmentation_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
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

void surface_segmentation_writer::starting()
{
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::stopping()
{
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::updating()
{
    if(this->has_location_defined())
    {
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

        // Retrieve dataStruct associated with this service
        const auto data  = m_data.lock();
        const auto model = std::dynamic_pointer_cast<const data::model_series>(data.get_shared());

        if(!model->get_dicom_reference())
        {
            m_write_failed = true;
            sight::ui::dialog::message::show(
                "Warning",
                "DICOM image reference is missing, DICOM Surface Segmentation cannot be generated",
                sight::ui::dialog::message::warning
            );
            return;
        }

        /* Build up the filename */
        std::filesystem::path output_path = this->get_folder() / "imSeg";

        /* Write the data */
        sight::ui::cursor cursor;
        cursor.set_cursor(ui::cursor_base::busy);
        save_surface_segmentation(output_path, model);
        cursor.set_default_cursor();
    }
    else
    {
        m_write_failed = true;
    }
}

//------------------------------------------------------------------------------

void surface_segmentation_writer::save_surface_segmentation(
    const std::filesystem::path _filename,
    const data::model_series::csptr& _model
)
{
    auto writer = std::make_shared<sight::io::dicom::writer::surface_segmentation>();

    writer->set_object(_model);
    writer->set_file(_filename);

    try
    {
        writer->write();
    }
    catch(const std::exception& e)
    {
        m_write_failed = true;
        std::stringstream ss;
        ss << "Warning during saving: " << e.what();
        sight::ui::dialog::message::show(
            "Warning",
            ss.str(),
            sight::ui::dialog::message::warning
        );
    }
    catch(...)
    {
        m_write_failed = true;
        sight::ui::dialog::message::show(
            "Warning",
            "Warning during saving",
            sight::ui::dialog::message::warning
        );
    }
}

//-----------------------------------------------------------------------------

sight::io::service::path_type_t surface_segmentation_writer::get_path_type() const
{
    return sight::io::service::folder;
}

} // namespace sight::module::io::dicom
