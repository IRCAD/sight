/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include <data/image_series.hpp>

#include <io/__/service/writer.hpp>

#include <ui/__/cursor.hpp>

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
            "The input key '" + sight::io::service::WRITER_DATA_KEY + "' is not correctly set.",
            image_series
        );

        sight::ui::busy_cursor cursor;

        auto write_observer = this->make_notification<sight::core::notification::observer>("Saving images... ");
        image_writer::save_image(this->get_file(), image_series, write_observer);
        m_write_failed = false;
    }
}

//------------------------------------------------------------------------------

} // namespace sight::module::io::itk
