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

#include "module/io/matrix/matrix4_trf_reader.hpp"

#include <core/notification/observer.hpp>

#include <data/matrix4.hpp>

#include <io/__/reader/matrix4_reader.hpp>
#include <io/__/service/reader.hpp>

#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

matrix4_trf_reader::matrix4_trf_reader() noexcept :
    reader("Choose a file to load a transformation matrix")
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t matrix4_trf_reader::get_path_type() const
{
    return sight::io::service::file;
}

//-----------------------------------------------------------------------------

void matrix4_trf_reader::info(std::ostream& _sstream)
{
    this->super_class::info(_sstream);
    _sstream << std::endl << " Matrix4 object reader";
}

//-----------------------------------------------------------------------------

std::vector<std::pair<std::string, std::string> > matrix4_trf_reader::get_supported_extensions() const
{
    return {{"TRF files", "*.trf"}};
}

//-----------------------------------------------------------------------------

void matrix4_trf_reader::starting()
{
}

//-----------------------------------------------------------------------------

void matrix4_trf_reader::configuring()
{
    sight::io::service::reader::configuring();
}

//-----------------------------------------------------------------------------

void matrix4_trf_reader::stopping()
{
}

//-----------------------------------------------------------------------------

void matrix4_trf_reader::updating()
{
    m_read_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve object
        const auto locked = m_data.lock();
        const auto matrix = std::dynamic_pointer_cast<data::matrix4>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::matrix4::classname()
            + "' or '"
            + sight::io::service::READER_DATA_KEY
            + "' is not correctly set.",
            matrix
        );

        try
        {
            auto observer = this->make_notification<core::notification::observer>("Reading matrix4 TRF file");
            this->emit_notification_created(observer);

            const auto reader = std::make_shared<sight::io::reader::matrix4_reader>();
            reader->set_object(matrix);
            reader->set_file(this->get_file());
            reader->read(observer);

            m_read_failed = false;

            matrix->async_emit(data::signals::MODIFIED);
            this->async_emit(reader::signals::SUCCEEDED);
        }
        catch(const std::exception& e)
        {
            // Handle the error.
            SIGHT_ERROR(e.what());
            this->fail(e.what());
            this->async_emit(reader::signals::FAILED);
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::matrix
