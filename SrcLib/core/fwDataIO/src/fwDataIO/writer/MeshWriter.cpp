/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwDataTools/MeshGenerator.hpp>

#include "fwDataIO/writer/MeshWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::fwDataIO::writer::MeshWriter, ::fwDataIO::writer::MeshWriter );


namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

MeshWriter::MeshWriter()
: ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{}

//------------------------------------------------------------------------------

MeshWriter::~MeshWriter()
{}

//------------------------------------------------------------------------------

void MeshWriter::write()
{
    OSLM_INFO( "[MeshReader::read] Trian file: " << getFile());
    assert( getFile().empty() ==  false );

    ::fwData::Mesh::sptr triMesh = this->getConcreteObject();
    FW_RAISE_IF("Can't convert this Mesh to TriangularMesh", !::fwDataTools::MeshGenerator::hasUniqueCellType(triMesh, ::fwData::Mesh::TRIANGLE));

    std::fstream file;
    file.open(getFile().string().c_str(), std::fstream::out);
    if (!file.is_open())
    {
        OSLM_ERROR( "Trian file writing error for " << getFile().string());
        std::string str = "Unable to write ";
        str+= getFile().string();
        throw std::ios_base::failure(str);
    }

    size_t i, nbPts, nbCells;
    nbPts = triMesh->getNumberOfPoints();
    ::fwData::Mesh::PointMultiArrayType points = triMesh->getPoints();
    file<<nbPts<<std::endl;
    for( i=0 ; i<nbPts ; ++i )
    {
        file<<points[i][0]<<" "<<points[i][1]<<" "<<points[i][2]<<std::endl;
    }

    nbCells = triMesh->getNumberOfCells();
    ::fwData::Mesh::CellDataMultiArrayType cells = triMesh->getCellData();
    size_t cellsSize = nbCells*3;
    SLM_ASSERT("Wrong CellDataMultiArray size", cells.size() == cellsSize);
    file<<nbCells<<std::endl;
    typedef ::fwData::Mesh::CellDataMultiArrayType::index CellTypesIndex;
    ::fwData::Array::sptr normals;// = triMesh->getCellNormalsArray();
    if(normals)
    {
//        ::fwData::Array::sptr cellTypes = triMesh->getCellTypesArray();
//        SLM_ASSERT("", normals->getSize().size() == 1);
//        SLM_ASSERT("", cellTypes->getSize().size() == 1);
//        SLM_ASSERT("", cellTypes->getSize()[0] == normals->getSize()[0]);
//
//        ::fwData::Mesh::CellNormalsMultiArrayType cellNormals = triMesh->getCellNormals();
//        for (CellTypesIndex i = 0; i < cellsSize; i+=3)
//        {
//            file<<cells[i]<<" "<<cells[i+1]<<" "<<cells[i+2]<<" "
//                    <<cellNormals[i/3][0]<<" "<<cellNormals[i/3][1]<<" "<<cellNormals[i/3][2]<<std::endl;
//        }
    }
    else
    {
        for (CellTypesIndex i = 0; i < cellsSize; i+=3)
        {
            file<<cells[i]<<" "<<cells[i+1]<<" "<<cells[i+2]<<" "
                    <<-1<<" "<<-1<<" "<<-1<<std::endl;
        }
    }
    file.close();
}

//------------------------------------------------------------------------------

std::string  MeshWriter::extension()
{
    return ".trian";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
