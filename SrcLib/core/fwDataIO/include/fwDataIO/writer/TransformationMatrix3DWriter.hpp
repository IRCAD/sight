/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__
#define __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__

#include "fwDataIO/config.hpp"
#include "fwDataIO/writer/GenericObjectWriter.hpp"

#include <fwData/location/SingleFile.hpp>
#include <fwData/TransformationMatrix3D.hpp>

namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Writer for transformation of 3D Matrix. Write file format .trf
 *
 *
 * Ircad writer to write a ::fwData::TransformationMatrix3D on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class FWDATAIO_CLASS_API TransformationMatrix3DWriter :
    public GenericObjectWriter< ::fwData::TransformationMatrix3D>,
    public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro( (TransformationMatrix3DWriter)(GenericObjectWriter< ::fwData::
                                                                                                TransformationMatrix3D >),
                                            (()),
                                            ::fwDataIO::writer::factory::New< TransformationMatrix3DWriter >
                                            )
    /// Constructor. Do nothing.
    FWDATAIO_API TransformationMatrix3DWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~TransformationMatrix3DWriter();

    /// Write the file with standard iostream API.
    FWDATAIO_API virtual void write() override;

    /// Defines extension supported by this writer ".trf"
    FWDATAIO_API virtual std::string  extension() override;

};

}

}

#endif /* __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__ */
