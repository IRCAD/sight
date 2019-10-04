/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#include "fwDataIO/reader/TransformationMatrix3DReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <fwData/location/SingleFile.hpp>

#include <filesystem>

#include <fstream>
#include <iostream>

fwDataIOReaderRegisterMacro( ::fwDataIO::reader::TransformationMatrix3DReader );

namespace fwDataIO
{
namespace reader
{

TransformationMatrix3DReader::TransformationMatrix3DReader(::fwDataIO::reader::IObjectReader::Key key) :
    ::fwData::location::enableSingleFile< IObjectReader >(this)
{
}

//------------------------------------------------------------------------------

TransformationMatrix3DReader::~TransformationMatrix3DReader()
{
}

//------------------------------------------------------------------------------

void TransformationMatrix3DReader::read()
{
    assert( ::std::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    std::filesystem::path file =
        ::std::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    assert( std::filesystem::exists(file) );

    std::ifstream inFile(file.string().c_str(), std::ifstream::in );
    assert( inFile.good() );

    char readedValue = 0;
    double value;

    while ( !inFile.eof() && readedValue < 16 )
    {
        inFile >> value;
        this->getConcreteObject()->getCoefficients()[readedValue] = value;
        readedValue++;
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
} // namespace fwDataIO
