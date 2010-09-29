/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/filesystem/convenience.hpp>



#include <fwTools/ClassRegistrar.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/reader/TransformationMatrix3DReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::TransformationMatrix3DReader, ::fwDataIO::reader::TransformationMatrix3DReader );


namespace fwDataIO {
namespace reader{

TransformationMatrix3DReader::TransformationMatrix3DReader()
: ::fwData::location::enableSingleFile< IObjectReader >(this)
{
    // TODO Auto-generated constructor stub

}

TransformationMatrix3DReader::~TransformationMatrix3DReader() {
    // TODO Auto-generated destructor stub
}



void TransformationMatrix3DReader::read()
{
    assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    ::boost::filesystem::path file = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    assert( ::boost::filesystem::exists(file) ) ;

    std::ifstream inFile(file.string().c_str(), std::ifstream::in );
    assert( inFile.good() );

    char readedValue = 0;
    double value;
    this->getConcreteObject()->getRefCoefficients().clear();
    this->getConcreteObject()->getRefCoefficients().reserve(16);

    while ( !inFile.eof() && readedValue<16 )
    {
        inFile >> value;
        this->getConcreteObject()->getRefCoefficients().push_back( value );
        readedValue++;
    }

    assert(this->getConcreteObject()->getRefCoefficients().size()==16 );
}


std::string  TransformationMatrix3DReader::extension()
{
    return (".trf");
}


}
}
