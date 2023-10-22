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

#include "modules/io/matrix/Matrix4WriterService.hpp"

#include <core/base.hpp>
#include <core/com/signal.hpp>
#include <core/com/signal.hxx>
#include <core/location/single_file.hpp>
#include <core/location/single_folder.hpp>

#include <data/matrix4.hpp>

#include <io/__/writer/matrix4_writer.hpp>

#include <service/macros.hpp>

#include <ui/__/dialog/location.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

namespace sight::module::io::matrix
{

//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------

void Matrix4WriterService::info(std::ostream& _sstream)
{
    this->SuperClass::info(_sstream);
    _sstream << std::endl << " Matrix4 object writer";
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::starting()
{
}

//------------------------------------------------------------------------------

sight::io::service::IOPathType Matrix4WriterService::getIOPathType() const
{
    return sight::io::service::FILE;
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::configuring()
{
    sight::io::service::writer::configuring();
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::openLocationDialog()
{
    static auto default_directory = std::make_shared<core::location::single_folder>();

    sight::ui::dialog::location dialog_file;
    dialog_file.setTitle(m_windowTitle.empty() ? "Choose a file to save a transformation matrix" : m_windowTitle);
    dialog_file.setDefaultLocation(default_directory);
    dialog_file.addFilter("TRF files", "*.trf");
    dialog_file.setOption(ui::dialog::location::WRITE);

    auto result = std::dynamic_pointer_cast<core::location::single_file>(dialog_file.show());
    if(result)
    {
        default_directory->set_folder(result->get_file().parent_path());
        dialog_file.saveDefaultLocation(default_directory);
        this->set_file(result->get_file());
    }
    else
    {
        this->clearLocations();
    }
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::stopping()
{
}

//-----------------------------------------------------------------------------

void Matrix4WriterService::updating()
{
    m_writeFailed = true;

    if(this->hasLocationDefined())
    {
        // Retrieve object
        const auto locked = m_data.lock();
        const auto matrix = std::dynamic_pointer_cast<const data::matrix4>(locked.get_shared());

        SIGHT_ASSERT(
            "The object is not a '"
            + data::matrix4::classname()
            + "' or '"
            + sight::io::service::s_DATA_KEY
            + "' is not correctly set.",
            matrix
        );

        const auto writer = std::make_shared<sight::io::writer::matrix4_writer>();
        writer->set_object(matrix);
        writer->set_file(this->get_file());
        writer->write();
        m_writeFailed = false;
    }
}

//-----------------------------------------------------------------------------

} // namespace sight::module::io::matrix
