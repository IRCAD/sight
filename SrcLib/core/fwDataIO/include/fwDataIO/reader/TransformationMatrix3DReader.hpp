/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  __FWDATAIO_READER_TRANSFORMATIONMATRIX3DREADER_HPP__
#define  __FWDATAIO_READER_TRANSFORMATIONMATRIX3DREADER_HPP__

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

    fwCoreClassDefinitionsWithFactoryMacro((TransformationMatrix3DReader)(GenericObjectReader< ::fwData::
                                                                                               TransformationMatrix3D >),
                                           (()),
                                           ::fwDataIO::reader::factory::New< TransformationMatrix3DReader >
                                           );

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

#endif /*  __FWDATAIO_READER_TRANSFORMATIONMATRIX3DREADER_HPP__ */
