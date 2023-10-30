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

#include "modules/io/matrix/matrix4_trf_reader.hpp"

#include <core/base.hpp>
#include <core/com/signal.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/matrix4.hpp>

#include <io/__/reader/matrix4_reader.hpp>
#include <io/__/service/reader.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

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

std::vector<std::string> matrix4_trf_reader::get_supported_extensions()
{
    std::vector<std::string> extensions;
    extensions.emplace_back(".trf");
    return extensions;
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

void matrix4_trf_reader::open_location_dialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.set_title(m_window_title.empty() ? "Choose a file to load a transformation matrix" : m_window_title);
    dialog_file.set_default_location(default_directory);
    dialog_file.add_filter("TRF files", "*.trf");
    dialog_file.set_option(ui::dialog::location::read);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.save_default_location(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clear_locations();
    }
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
            + sight::io::service::DATA_KEY
            + "' is not correctly set.",
            matrix
        );

        const auto reader = std::make_shared<sight::io::reader::matrix4_reader>();
        reader->set_object(matrix);
        reader->set_file(this->get_file());
        reader->read();

        m_read_failed = false;

        // Notify reading
        const auto sig = matrix->signal<data::object::modified_signal_t>(
            data::object::MODIFIED_SIG
        );
        {
            core::com::connection::blocker block(sig->get_connection(slot(service::slots::UPDATE)));
            sig->async_emit();
        }
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::matrix
