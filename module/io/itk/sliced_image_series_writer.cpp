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

#include "sliced_image_series_writer.hpp"

#include "module/io/itk/image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_folder.hpp>

#include <data/image.hpp>
#include <data/image_series.hpp>

#include <io/__/service/writer.hpp>

#include <service/macros.hpp>

#include <ui/__/cursor.hpp>
#include <ui/__/dialog/location.hpp>
#include <ui/__/dialog/message.hpp>
#include <ui/__/dialog/progress.hpp>

namespace sight::module::io::itk
{

sliced_image_series_writer::sliced_image_series_writer() noexcept :
    writer("Choose a directory to save image")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t sliced_image_series_writer::get_path_type() const
{
    return sight::io::service::folder;
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog;
    dialog.set_title(*m_window_title);
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

void sliced_image_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::info(std::ostream& _sstream)
{
    _sstream << "sliced_image_series_writer::info";
}

//------------------------------------------------------------------------------

void sliced_image_series_writer::updating()
{
    m_write_failed = true;
    if(this->has_location_defined())
    {
        const auto data         = m_data.lock();
        const auto image_series = std::dynamic_pointer_cast<const data::image_series>(data.get_shared());
        SIGHT_ASSERT("The input key '" + sight::io::service::DATA_KEY + "' is not correctly set.", image_series);

        sight::ui::busy_cursor cursor;
        image_writer::save_image(this->get_folder(), image_series);
        m_write_failed = false;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
