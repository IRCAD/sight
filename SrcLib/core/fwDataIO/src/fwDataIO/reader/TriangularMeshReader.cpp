/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/statement/sequence.hpp>
#include <boost/spirit/home/phoenix/container.hpp>

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


template <typename Iterator>
bool parseTrian(Iterator first, Iterator last, ::fwData::TriangularMesh::PointContainer &points, ::fwData::TriangularMesh::CellContainer &cells)
{
    using boost::spirit::qi::ulong_long;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::float_;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::qi::repeat;
    using boost::spirit::ascii::space;
    using boost::phoenix::push_back;
    using boost::phoenix::ref;
    namespace phx = boost::phoenix;

    unsigned long long int nbPoints;
    unsigned long long int nbCells;

    ::fwData::TriangularMesh::PointContainer::value_type point(3);
    float &pa = point[0];
    float &pb = point[1];
    float &pc = point[2];
    ::fwData::TriangularMesh::CellContainer::value_type cell(3);
    int &ca = cell[0];
    int &cb = cell[1];
    int &cc = cell[2];

    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
            ulong_long[ phx::ref(nbPoints) = _1, phx::reserve(phx::ref(points), phx::ref(nbPoints)) ] >>
            repeat(phx::ref(nbPoints))[ (float_ >> float_ >> float_)[phx::ref(pa) = _1, phx::ref(pb) = _2, phx::ref(pc) = _3, phx::push_back(phx::ref(points),phx::ref(point))] ] >>
            ulong_long[ phx::ref(nbCells) = _1, phx::reserve(phx::ref(cells), phx::ref(nbCells)) ] >>
            repeat(phx::ref(nbCells))[ (int_ >> int_ >> int_ >> "-1 -1 -1")[phx::ref(ca) = _1, phx::ref(cb) = _2, phx::ref(cc) = _3, phx::push_back(phx::ref(cells),phx::ref(cell))]  ]
            // An alternative for >> "-1 -1 -1" is: >> int_  >> int_  >> int_
            // but this is a little bit less efficient, since every file should
            // currently contain -1 -1 -1 as normal's values
        ),
        //  End grammar

        space
        );

    if (first != last) // fail if we didn't get a full match
        return false;
    return r;

}


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
    SLM_ASSERT("Empty path for TriangularMesh file", !path.empty() );


    size_t length;
    //char *buffer;
    std::string buf;
    std::ifstream file;
    file.open(path.string().c_str(), std::ios::binary );

    if (!file.is_open())
    {
        OSLM_ERROR( "Trian file loading error for " << path.string());
        throw std::ios_base::failure("Unable to open " + path.string());
    }


    file.seekg (0, std::ios::end);
    length = file.tellg();
    file.seekg (0, std::ios::beg);

    //buffer = new char [length];
    buf.resize(length);
    char *buffer = &buf[0];

    file.read (buffer, length);
    file.close();


    ::fwData::TriangularMesh::sptr triMesh = getConcreteObject();

    ::fwData::TriangularMesh::PointContainer newPoints;
    ::fwData::TriangularMesh::CellContainer  newCells;
    ::fwData::TriangularMesh::PointContainer &points = triMesh->points();
    ::fwData::TriangularMesh::CellContainer  &cells  = triMesh->cells();

    //Free TriangularMesh memory
    triMesh->clearCells();
    triMesh->clearPoints();

    if (!parseTrian(buffer, buffer+length, newPoints, newCells))
    {
        OSLM_ERROR( "Bad file format : " << path.string());
        throw std::ios_base::failure("Unable to open " + path.string() + " : Bad file format.");
    }

    points.swap(newPoints);
    cells.swap(newCells);


    //delete buffer;

}

//------------------------------------------------------------------------------

std::string  TriangularMeshReader::extension()
{
    return (".trian");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
