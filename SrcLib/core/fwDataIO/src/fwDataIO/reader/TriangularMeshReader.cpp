/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Object.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/reader/TriangularMeshReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::TriangularMeshReader, ::fwDataIO::reader::TriangularMeshReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

TriangularMeshReader::TriangularMeshReader()
: ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

TriangularMeshReader::~TriangularMeshReader()
{}

//------------------------------------------------------------------------------

void TriangularMeshReader::read()
{
    assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    ::boost::filesystem::path path = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();


    OSLM_INFO( "[TriangularMeshReader::read] Trian file: " << path.string());
    assert( path.empty() ==  false );

    ::boost::shared_ptr< ::fwData::TriangularMesh > triMesh = getConcreteObject();

    std::fstream file;
    file.open(path.string().c_str(), std::fstream::in);
    if (!file.is_open())
    {
        OSLM_ERROR( "Trian file loading error for " << path.string());
        throw std::ios_base::failure("Unable to open " + path.string());
    }


    // Clear the container cells and set its capacity to 0
    triMesh->clearCells();
    // Clear the container points and set its capacity to 0
    triMesh->clearPoints();

    ::fwData::TriangularMesh::PointContainer &points = triMesh->points();
    ::fwData::TriangularMesh::CellContainer  &cells  = triMesh->cells();

    std::vector< float > point(3);
    float &pa = point[0];
    float &pb = point[1];
    float &pc = point[2];
    std::vector< int > cell(3);
    int &ca = cell[0];
    int &cb = cell[1];
    int &cc = cell[2];

    /// Read content and update mesh data structure
    unsigned int nbPts, nbCells, i;

    file>>nbPts;
    points.reserve(nbPts);
    i = nbPts + 1;
    while (--i)
    {
        file >> pa >> pb >> pc;
        points.push_back( point ) ;
    }
    points.resize(nbPts);

    file>>nbCells;
    cells.reserve(nbCells);
    i = nbCells + 1;
    while (--i)
    {
        file>>ca >> cb >> cc;
        file.ignore(20, '\n');
        cells.push_back( cell ) ;
    }
    cells.resize(nbCells);
    file.close();

}



std::string  TriangularMeshReader::extension()
{
    return (".trian");
}


} // namespace reader

} // namespace fwDataIO
