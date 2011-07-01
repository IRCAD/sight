/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <fwData/Object.hpp>
#include <fwData/TriangularMesh.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwDataIO/writer/TriangularMeshWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwDataIO::writer::TriangularMeshWriter, ::fwDataIO::writer::TriangularMeshWriter );


namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

TriangularMeshWriter::TriangularMeshWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

TriangularMeshWriter::~TriangularMeshWriter()
{}

//------------------------------------------------------------------------------

void TriangularMeshWriter::write()
{
    OSLM_INFO( "[TriangularMeshReader::read] Trian file: " << getFile());
    assert( getFile().empty() ==  false );

    ::boost::shared_ptr< ::fwData::TriangularMesh > triMesh = getConcreteObject();

    std::fstream file;
    file.open(getFile().string().c_str(), std::fstream::out);
    if (!file.is_open())
    {
        OSLM_ERROR( "Trian file writing error for " << getFile().string());
        std::string str = "Unable to write ";
        str+= getFile().string();
        throw std::ios_base::failure(str);
    }

    unsigned int i, nbPts, nbCells;
    nbPts = triMesh->points().size();
    file<<nbPts<<std::endl;
    for( i=0 ; i<nbPts ; ++i )
    {
        std::vector< float > point = triMesh->points().at(i);
        file<<point[0]<<" "<<point[1]<<" "<<point[2]<<std::endl;
    }

    nbCells = triMesh->cells().size();
    file<<nbCells<<std::endl;
    for( i=0 ; i<nbCells ; ++i )
    {
        std::vector< int > cell = triMesh->cells().at(i);
        file<<cell[0]<<" "<<cell[1]<<" "<<cell[2]<<" "<<-1<<" "<<-1<<" "<<-1<<std::endl;
    }
    file.close();
}


std::string  TriangularMeshWriter::extension()
{
    return ".trian";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
