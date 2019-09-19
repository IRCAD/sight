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

#pragma once

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/Mesh.hpp>

#include <boost/filesystem/path.hpp>

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Mesh writer. Write file format .trian
 *
 *
 * Ircad writer to write a ::fwData::Mesh (with only triangular cells) on filesystem which the file format
 * is .trian. This format is basic. This is an ascii file which contains :
 * 1) nb of points
 * 2) list of points (x,y,z). Line numbers represent index of point in vector
 * 3) nb of triangles
 * 4) list of triangles with index of points and normals ( often set a -1 )
 */
class FWDATAIO_CLASS_API MeshWriter :
    public GenericObjectWriter< ::fwData::Mesh>,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassMacro(MeshWriter, GenericObjectWriter< ::fwData::Mesh >, ::fwDataIO::writer::factory::New< MeshWriter >);
    /// Constructor. Do nothing.
    FWDATAIO_API MeshWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~MeshWriter();

    /**
     * Write the file with standard iostream API.
     * @pre Mesh must contains only triangular cells.
     */
    FWDATAIO_API virtual void write() override;

    /// Defines extension supported by this reader ".trian"
    FWDATAIO_API virtual std::string extension() override;

};

} // namespace writer

} // namespace fwDataIO
