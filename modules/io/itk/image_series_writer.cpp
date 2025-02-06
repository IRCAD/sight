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

#include "image_series_writer.hpp"

#include "image_writer.hpp"

#include <core/base.hpp>
#include <core/location/single_file.hpp>
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

image_series_writer::image_series_writer() noexcept :
    writer("Choose an image file to save image")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t image_series_writer::get_path_type() const
{
    return sight::io::service::file;
}

//------------------------------------------------------------------------------

void image_series_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//------------------------------------------------------------------------------

void image_series_writer::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(*m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("NIfTI (.nii)", "*.nii *.nii.gz");
    dialog_file.add_filter("Inr (.inr.gz)", "*.inr.gz");
    dialog_file.set_option(ui::dialog::location::write);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        this->set_file(result->get_file());
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
    }
    else
    {
        this->clear_locations();
    }
}

//------------------------------------------------------------------------------

void image_series_writer::starting()
{
}

//------------------------------------------------------------------------------

void image_series_writer::stopping()
{
}

//------------------------------------------------------------------------------

void image_series_writer::info(std::ostream& _sstream)
{
    _sstream << "image_series_writer::info";
}

//------------------------------------------------------------------------------

void image_series_writer::updating()
{
    m_write_failed = true;

    if(this->has_location_defined())
    {
        const auto data         = m_data.lock();
        const auto image_series = std::dynamic_pointer_cast<const data::image_series>(data.get_shared());
        SIGHT_ASSERT(
            "The input key '" + sight::io::service::DATA_KEY + "' is not correctly set.",
            image_series
        );

        sight::ui::busy_cursor cursor;
        image_writer::save_image(this->get_file(), image_series);
        m_write_failed = false;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
