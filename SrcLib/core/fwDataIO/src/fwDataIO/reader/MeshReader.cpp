/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>
#include <boost/foreach.hpp>

#include <boost/spirit/include/qi.hpp>
#include <boost/spirit/include/phoenix_core.hpp>
#include <boost/spirit/include/phoenix_operator.hpp>
#include <boost/spirit/home/phoenix/statement/sequence.hpp>
#include <boost/spirit/home/phoenix/container.hpp>
#include <boost/spirit/home/phoenix/bind/bind_member_function.hpp>
#include <boost/spirit/home/phoenix/core/argument.hpp>
#include <boost/spirit/home/phoenix/operator/bitwise.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Object.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/reader/MeshReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::MeshReader, ::fwDataIO::reader::MeshReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

struct cell_data_offset_generator {
        ::fwData::Mesh::CellDataOffsetType current;
        cell_data_offset_generator() {current=0;}
        int operator()() {
            ::fwData::Mesh::CellDataOffsetType res = current;
            current += 3;
            return res;
        }
} ;

//------------------------------------------------------------------------------


template <typename Iterator>
bool parseTrian2(Iterator first, Iterator last, ::fwData::Mesh::sptr mesh)
{
    using boost::spirit::qi::ulong_long;
    using boost::spirit::qi::int_;
    using boost::spirit::qi::float_;
    using boost::spirit::qi::phrase_parse;
    using boost::spirit::qi::_1;
    using boost::spirit::qi::_2;
    using boost::spirit::qi::_3;
    using boost::spirit::qi::_4;
    using boost::spirit::qi::_5;
    using boost::spirit::qi::_6;
    using boost::spirit::qi::repeat;
    using boost::spirit::ascii::space;
    using boost::phoenix::push_back;
    using boost::phoenix::ref;
    namespace phx = boost::phoenix;

    unsigned long long int nbPoints;
    unsigned long long int nbCells;


    mesh->allocateCellNormals();

    ::fwData::Array::sptr pointArray           = mesh->getPointsArray();
    ::fwData::Array::sptr cellDataArray        = mesh->getCellDataArray();
    ::fwData::Array::sptr cellTypesArray       = mesh->getCellTypesArray();
    ::fwData::Array::sptr cellDataOffsetsArray = mesh->getCellDataOffsetsArray();
    ::fwData::Array::sptr cellNormalsArray     = mesh->getCellNormalsArray();


    ::fwData::Array::SizeType pointArraySize;
    ::fwData::Array::SizeType cellArraySize;

    ::fwData::Mesh::PointValueType  *pointArrayBuffer       = 0;
    ::fwData::Mesh::CellValueType   *cellDataArrayBuffer    = 0;
    ::fwData::Mesh::NormalValueType *cellNormalsArrayBuffer = 0;

    bool r = phrase_parse(first, last,

        //  Begin grammar
        (
            ulong_long
            [
                ref(nbPoints) = _1,
                phx::bind(&::fwData::Mesh::setNumberOfPoints, *mesh, _1),
                phx::push_back(ref(pointArraySize),ref(nbPoints)),
                phx::bind(&::fwData::Array::resize, *pointArray, ref(pointArraySize), true) ,
                ref(pointArrayBuffer) = phx::bind(&::fwData::Array::begin< ::fwData::Mesh::PointValueType >, *pointArray )
            ]

            >> repeat(ref(nbPoints))
            [
                (float_ >> float_ >> float_)
                [
                    *ref(pointArrayBuffer)++ = _1,
                    *ref(pointArrayBuffer)++ = _2,
                    *ref(pointArrayBuffer)++ = _3
                ]
            ]

            >> ulong_long
            [
                ref(nbCells) = _1,
                phx::bind(&::fwData::Mesh::setNumberOfCells, *mesh, _1),
                phx::bind(&::fwData::Mesh::setCellDataSize, *mesh, _1*3),
                phx::bind(&::fwData::Mesh::adjustAllocatedMemory, *mesh),
                ref(cellDataArrayBuffer) = phx::bind(&::fwData::Array::begin< ::fwData::Mesh::CellValueType >, *cellDataArray ),
                ref(cellNormalsArrayBuffer) = phx::bind(&::fwData::Array::begin< ::fwData::Mesh::NormalValueType >, *cellNormalsArray )
            ]

            >> repeat(ref(nbCells))
            [
                (int_ >> int_ >> int_ >> float_ >> float_ >> float_)
                [
                    *ref(cellDataArrayBuffer)++ = _1,
                    *ref(cellDataArrayBuffer)++ = _2,
                    *ref(cellDataArrayBuffer)++ = _3,
                    *ref(cellNormalsArrayBuffer)++ = _4,
                    *ref(cellNormalsArrayBuffer)++ = _5,
                    *ref(cellNormalsArrayBuffer)++ = _6
                ]
            ]
        ),
        //  End grammar

        space
        );

    std::fill( 
            cellTypesArray->begin< ::fwData::Mesh::CellTypes >(),
            cellTypesArray->end< ::fwData::Mesh::CellTypes >(),
            static_cast< ::fwData::Mesh::CellTypes >(::fwData::Mesh::TRIANGLE)
            );


    cell_data_offset_generator cellDataOffsetGenerator;

    std::generate(
            cellDataOffsetsArray->begin< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsetsArray->end< ::fwData::Mesh::CellDataOffsetType >(),
            cellDataOffsetGenerator
            );



    // Check if normals array is filled of -1. values
    const float normalBadValue = -1.f;
    float normal = normalBadValue;
    int &n = *reinterpret_cast<int*>(&normal);

    std::for_each(
            cellNormalsArray->begin< int >(),
            cellNormalsArray->end< int >(),
            ref(n) &= boost::phoenix::arg_names::arg1
            );

    if (normal == -1)
    {
        mesh->clearCellNormals();
        SLM_WARN("normals equals to (-1,-1,-1) : normals removed.");
    }

    if (first != last) // fail if we didn't get a full match
        return false;
    return r;

}



//------------------------------------------------------------------------------

MeshReader::MeshReader()
: ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

MeshReader::~MeshReader()
{}

//------------------------------------------------------------------------------

void MeshReader::read()
{
    assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
    ::boost::filesystem::path path = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();

    OSLM_TRACE( "Trian file: " << path.string());
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


    ::fwData::Mesh::sptr mesh = getConcreteObject();

    mesh->clear();

    if (!parseTrian2(buffer, buffer+length, mesh))
    {
        OSLM_ERROR( "Bad file format : " << path.string());
        throw std::ios_base::failure("Unable to open " + path.string() + " : Bad file format.");
    }

}

//------------------------------------------------------------------------------

std::string  MeshReader::extension()
{
    return (".trian");
}

//------------------------------------------------------------------------------

} // namespace reader
} // namespace fwDataIO
