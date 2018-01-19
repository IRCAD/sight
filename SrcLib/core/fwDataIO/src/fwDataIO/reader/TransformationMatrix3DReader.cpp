/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDataIO/reader/TransformationMatrix3DReader.hpp"

#include "fwDataIO/reader/registry/macros.hpp"

#include <fwData/location/SingleFile.hpp>

#include <boost/filesystem/convenience.hpp>

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
    ::boost::filesystem::path file =
        ::std::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    assert( ::boost::filesystem::exists(file) );

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
