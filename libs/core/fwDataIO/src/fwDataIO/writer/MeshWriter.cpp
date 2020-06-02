/************************************************************************
 *
 * Copyright (C) 2009-2020 IRCAD France
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

#include "fwDataIO/writer/MeshWriter.hpp"

#include "fwDataIO/writer/registry/macros.hpp"

#include <fwDataTools/Mesh.hpp>

#include <fstream>
#include <iostream>

fwDataIOWriterRegisterMacro( ::fwDataIO::writer::MeshWriter );

namespace fwDataIO
{

namespace writer
{

//------------------------------------------------------------------------------

MeshWriter::MeshWriter(::fwDataIO::writer::IObjectWriter::Key) :
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
                !::fwDataTools::Mesh::hasUniqueCellType(mesh, ::fwData::Mesh::CellType::TRIANGLE));

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

    const auto dumpLock = mesh->lock();

    const size_t nbPts   = mesh->getNumberOfPoints();
    const size_t nbCells = mesh->getNumberOfCells();

    auto pointsItr       = mesh->begin< ::fwData::iterator::ConstPointIterator >();
    const auto pointsEnd = mesh->end< ::fwData::iterator::ConstPointIterator >();

    file<<nbPts<<std::endl;
    for(; pointsItr != pointsEnd; ++pointsItr)
    {
        file << pointsItr->point->x << " " << pointsItr->point->y << " " << pointsItr->point->z << std::endl;
    }

    auto cellItr       = mesh->begin< ::fwData::iterator::ConstCellIterator >();
    const auto cellEnd = mesh->end< ::fwData::iterator::ConstCellIterator >();

    file << nbCells << std::endl;

    if(mesh->hasCellNormals())
    {
        while (cellItr != cellEnd)
        {
            file << (cellItr->pointIdx[0]) << " ";
            file << (cellItr->pointIdx[1]) << " ";
            file << (cellItr->pointIdx[2]) << " ";
            file << (cellItr->normal->nx) << " ";
            file << (cellItr->normal->ny) << " ";
            file << (cellItr->normal->nz) << std::endl;
            ++cellItr;
        }
    }
    else
    {
        while (cellItr != cellEnd)
        {
            file << (cellItr->pointIdx[0]) << " ";
            file << (cellItr->pointIdx[1]) << " ";
            file << (cellItr->pointIdx[2]) << " -1 -1 -1" << std::endl;
            ++cellItr;
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
