/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__
#define __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__

#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"

#include "fwDataIO/writer/GenericObjectWriter.hpp"


namespace fwDataIO
{

namespace writer
{

/**
 * @brief   Writer for transformation of 3D Matrix. Write file format .trf
 * @class   TransformationMatrix3DWriter
 *
 * @date    2009
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
                                            );
    /// Constructor. Do nothing.
    FWDATAIO_API TransformationMatrix3DWriter(::fwDataIO::writer::IObjectWriter::Key key);

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~TransformationMatrix3DWriter();

    /// Write the file with standard iostream API.
    FWDATAIO_API virtual void write();

    /// Defines extension supported by this writer ".trf"
    FWDATAIO_API virtual std::string  extension();

};

}

}

#endif /* __FWDATAIO_WRITER_TRANSFORMATIONMATRIX3DWRITER_HPP__ */
