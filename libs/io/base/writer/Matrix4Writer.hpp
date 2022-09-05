/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "io/base/config.hpp"
#include "io/base/writer/GenericObjectWriter.hpp"

#include <core/location/SingleFile.hpp>

#include <data/Matrix4.hpp>

namespace sight::io::base::writer
{

/**
 * @brief   Writer for transformation of 3D Matrix. Writes .trf files.
 *
 *
 * Writes 'sight::data::Matrix4' into a '.trf' file.
 * This is an ascii file which contains matrix 4x4 values
 */
class IO_BASE_CLASS_API Matrix4Writer :
    public GenericObjectWriter<data::Matrix4>,
    public core::location::SingleFile
{
public:

    SIGHT_DECLARE_CLASS(
        Matrix4Writer,
        GenericObjectWriter<data::Matrix4>,
        io::base::writer::factory::New<Matrix4Writer>
    )

    /// Constructor. Does nothing.
    IO_BASE_API Matrix4Writer(io::base::writer::IObjectWriter::Key key);

    /// Destructor. Does nothing.
    IO_BASE_API ~Matrix4Writer() override;

    /// Writes the file using the standard iostream API.
    IO_BASE_API void write() override;

    /// Defines the extensions supported by this writer. Here: ".trf"
    IO_BASE_API std::string extension() const override;
};

} // namespace sight::io::base::writer
