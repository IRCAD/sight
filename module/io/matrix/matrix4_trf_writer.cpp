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

#include "module/io/matrix/matrix4_trf_writer.hpp"

#include <data/matrix4.hpp>

#include <io/__/writer/matrix4_writer.hpp>

#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

matrix4_trf_writer::matrix4_trf_writer() noexcept :
    writer("Choose a file to save a transformation matrix")
{
}

//-----------------------------------------------------------------------------

void matrix4_trf_writer::info(std::ostream& _sstream)
{
    this->super_class::info(_sstream);
    _sstream << std::endl << " Matrix4 object writer";
}

//-----------------------------------------------------------------------------

void matrix4_trf_writer::starting()
{
}

//------------------------------------------------------------------------------

sight::io::service::path_type_t matrix4_trf_writer::get_path_type() const
{
    return sight::io::service::file;
}

//-----------------------------------------------------------------------------

void matrix4_trf_writer::configuring()
{
    sight::io::service::writer::configuring();
}

//-----------------------------------------------------------------------------

void matrix4_trf_writer::stopping()
{
}

//-----------------------------------------------------------------------------

void matrix4_trf_writer::updating()
{
    m_write_failed = true;

    if(this->has_location_defined())
    {
        // Retrieve object
        const auto locked = m_data.lock();
        const auto matrix = std::dynamic_pointer_cast<const data::matrix4>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::matrix4::classname()
            + "' or '"
            + sight::io::service::WRITER_DATA_KEY
            + "' is not correctly set.",
            matrix
        );

        auto observer = this->make_notification<core::notification::observer>("Writing matrix4 TRF file");
        this->emit_notification_created(observer);

        const auto writer = std::make_shared<sight::io::writer::matrix4_writer>();
        writer->set_object(matrix);
        writer->set_file(this->get_file());
        writer->write(observer);
        m_write_failed = false;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::matrix
