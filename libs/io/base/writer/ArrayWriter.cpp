/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "io/base/writer/ArrayWriter.hpp"

#include "io/base/writer/registry/macros.hpp"

#include <filesystem>
#include <fstream>
#include <iostream>

SIGHT_REGISTER_IO_WRITER(::sight::io::base::writer::ArrayWriter);

namespace sight::io::base
{

namespace writer
{

//------------------------------------------------------------------------------

ArrayWriter::ArrayWriter(io::base::writer::IObjectWriter::Key)
{
}

//------------------------------------------------------------------------------

ArrayWriter::~ArrayWriter()
{
}

//------------------------------------------------------------------------------

void ArrayWriter::write()
{
    std::filesystem::path file = getFile();

    data::Array::csptr array = this->getConcreteObject();
    size_t arraySizeInBytes  = array->getSizeInBytes();
    const auto dumpLock      = array->lock();
    const char* buff         = static_cast<const char*>(array->getBuffer());

    std::ofstream fs(file.string().c_str(), std::ios::binary | std::ios::trunc);

    SIGHT_THROW_IF("Unable to open " << file, !fs.good());

    fs.write(buff, static_cast<std::streamsize>(arraySizeInBytes));
    fs.close();
}

//------------------------------------------------------------------------------

std::string ArrayWriter::extension() const
{
    return ".raw";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace sight::io::base
