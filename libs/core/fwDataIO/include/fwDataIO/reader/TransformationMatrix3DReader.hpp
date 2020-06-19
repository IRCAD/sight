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
#include "fwDataIO/reader/GenericObjectReader.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace fwDataIO
{

namespace reader
{

/**
 * @brief   Reader for transformation of 3D Matrix. Read file format .trf
 *
 *
 * Ircad reader to read a ::fwData::TransformationMatrix3D on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class FWDATAIO_CLASS_API TransformationMatrix3DReader :
    public GenericObjectReader< ::fwData::TransformationMatrix3D >,
    public ::fwData::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassMacro(TransformationMatrix3DReader, GenericObjectReader< ::fwData::TransformationMatrix3D >,
                     ::fwDataIO::reader::factory::New< TransformationMatrix3DReader >);

    /// Constructor. Do nothing.
    FWDATAIO_API TransformationMatrix3DReader(::fwDataIO::reader::IObjectReader::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~TransformationMatrix3DReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read() override;

    /// Defines extension supported by this reader ".trf"
    FWDATAIO_API virtual std::string extension() override;

};

}

}
