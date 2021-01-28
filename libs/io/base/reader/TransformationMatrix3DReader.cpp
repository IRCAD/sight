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

#include "io/base/reader/TransformationMatrix3DReader.hpp"

#include "io/base/reader/registry/macros.hpp"

#include <data/location/SingleFile.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>

fwDataIOReaderRegisterMacro( ::sight::io::base::reader::TransformationMatrix3DReader );

namespace sight::io::base
{
namespace reader
{

TransformationMatrix3DReader::TransformationMatrix3DReader(io::base::reader::IObjectReader::Key) :
    data::location::enableSingleFile< IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

TransformationMatrix3DReader::~TransformationMatrix3DReader()
{
}

//------------------------------------------------------------------------------

void TransformationMatrix3DReader::read()
{
    assert( ::std::dynamic_pointer_cast< data::location::SingleFile >(m_location) );
    std::filesystem::path file =
        ::std::dynamic_pointer_cast< data::location::SingleFile >(m_location)->getPath();

    assert( std::filesystem::exists(file) );

    std::ifstream inFile(file.string().c_str(), std::ifstream::in );
    assert( inFile.good() );

    char readValue = 0;
    double value;

    while ( !inFile.eof() && readValue < 16 )
    {
        inFile >> value;
        this->getConcreteObject()->getCoefficients()[static_cast<size_t>(readValue)] = value;
        readValue++;
    }

    assert(this->getConcreteObject()->getCoefficients().size() == 16 );
}

//------------------------------------------------------------------------------

std::string TransformationMatrix3DReader::extension()
{
    return (".trf");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace sight::io::base
