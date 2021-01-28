/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include <data/location/SingleFile.hpp>
#include <data/TransformationMatrix3D.hpp>

namespace sight::io::base
{

namespace writer
{

/**
 * @brief   Writer for transformation of 3D Matrix. Write file format .trf
 *
 *
 * Ircad writer to write a data::TransformationMatrix3D on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class IO_BASE_CLASS_API TransformationMatrix3DWriter :
    public GenericObjectWriter< data::TransformationMatrix3D>,
    public data::location::enableSingleFile< io::base::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(
        TransformationMatrix3DWriter,
        GenericObjectWriter< data::TransformationMatrix3D >,
        io::base::writer::factory::New< TransformationMatrix3DWriter >
        )
    /// Constructor. Do nothing.
    IO_BASE_API TransformationMatrix3DWriter(io::base::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    IO_BASE_API virtual ~TransformationMatrix3DWriter();

    /// Write the file with standard iostream API.
    IO_BASE_API virtual void write() override;

    /// Defines extension supported by this writer ".trf"
    IO_BASE_API virtual std::string  extension() override;

};

}

}
