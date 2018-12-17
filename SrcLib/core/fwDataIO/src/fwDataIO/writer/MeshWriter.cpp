/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDataIO/writer/MeshWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwDataTools/helper/Array.hpp>
#include <fwDataTools/helper/MeshGetter.hpp>
#include <fwDataTools/Mesh.hpp>

#include <fstream>
#include <iostream>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::MeshWriter );

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

MeshWriter::MeshWriter(::fwDataIO::writer::IObjectWriter::Key key) :
    ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >(this)
{
}

//------------------------------------------------------------------------------

MeshWriter::~MeshWriter()
{
}

//------------------------------------------------------------------------------

void MeshWriter::write()
{
    OSLM_INFO( "[MeshReader::read] Trian file: " << getFile());
    assert( getFile().empty() == false );

    ::fwData::Mesh::csptr mesh = this->getConcreteObject();
    FW_RAISE_IF("Can't convert this Mesh to Trian file",
                !::fwDataTools::Mesh::hasUniqueCellType(mesh, ::fwData::Mesh::TRIANGLE));

    std::fstream file;
    file.open(getFile().string().c_str(), std::fstream::out);
    file.precision(std::numeric_limits< float >::digits10);

    if (!file.is_open())
    {
        OSLM_ERROR( "Trian file writing error for " << getFile().string());
        std::string str = "Unable to write ";
        str += getFile().string();
        throw std::ios_base::failure(str);
    }

    ::fwDataTools::helper::MeshGetter meshHelper(mesh);

    size_t i, nbPts, nbCells;
    nbPts                                            = mesh->getNumberOfPoints();
    ::fwData::Mesh::ConstPointsMultiArrayType points = meshHelper.getPoints();
    file<<nbPts<<std::endl;
    for( i = 0; i < nbPts; ++i )
    {
        file << points[i][0] << " " << points[i][1] << " " << points[i][2] << std::endl;
    }

    nbCells                     = mesh->getNumberOfCells();
    ::fwData::Array::sptr cells = mesh->getCellDataArray();

    ::fwDataTools::helper::Array cellsArrayHelper(cells);

    FW_RAISE_IF("Not able to write " << cells->getType().string() << " cell type in trian file.",
                cells->getType() != ::fwTools::Type::create< std::uint64_t >());

    std::uint64_t* cellBuf    = cellsArrayHelper.begin< std::uint64_t >();
    std::uint64_t* cellBufEnd = cellBuf + 3*nbCells;

    SLM_ASSERT("Wrong CellDataMultiArray size", cells->getNumberOfElements() >= nbCells*3);
    file << nbCells << std::endl;

    ::fwData::Array::sptr normals = mesh->getCellNormalsArray();

    if(normals
       && !normals->empty()
       && normals->getType() == ::fwTools::Type::create<float>()
       && normals->getNumberOfComponents() == 3
       && normals->getNumberOfDimensions() == 1
       && nbCells == normals->getSize().at(0)
       )
    {
        ::fwDataTools::helper::Array normalsArrayHelper(normals);
        float* normalBuf = normalsArrayHelper.begin< float >();

        while (cellBuf != cellBufEnd)
        {
            file << (*cellBuf++) << " ";
            file << (*cellBuf++) << " ";
            file << (*cellBuf++) << " ";
            file << (*normalBuf++) << " ";
            file << (*normalBuf++) << " ";
            file << (*normalBuf++) << std::endl;
        }
    }
    else
    {
        while (cellBuf != cellBufEnd)
        {
            file << (*cellBuf++) << " ";
            file << (*cellBuf++) << " ";
            file << (*cellBuf++) << " -1 -1 -1" << std::endl;
        }
    }
    file.close();
}

//------------------------------------------------------------------------------

std::string MeshWriter::extension()
{
    return ".trian";
}

//------------------------------------------------------------------------------

} // namespace writer

} // namespace fwDataIO
