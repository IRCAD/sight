/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef  _DATAIO_READER_TRANSFORMATIONMATRIXREADER3D_HPP_
#define  _DATAIO_READER_TRANSFORMATIONMATRIXREADER3D_HPP_

#include <boost/shared_ptr.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

namespace fwDataIO {

namespace reader {

/**
 * @brief   Reader for transformation of 3D Matrix. Read file format .trf
 * @class   TransformationMatrix3DReader.
 * @author  IRCAD (Research and Development Team).
 * @date    2009
 *
 * Ircad reader to read a ::fwData::TransformationMatrix3D on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class FWDATAIO_CLASS_API TransformationMatrix3DReader :
    public GenericObjectReader< ::fwData::TransformationMatrix3D >,
    public ::fwData::location::enableSingleFile< IObjectReader >
{

public:

    fwCoreClassDefinitionsWithFactoryMacro((TransformationMatrix3DReader)(GenericObjectReader< ::fwData::TransformationMatrix3D >),
                                           (()),
                                           new  TransformationMatrix3DReader
                                           );

    /// Constructor. Do nothing.
    FWDATAIO_API TransformationMatrix3DReader();

    /// Destructor. Do nothing.
    FWDATAIO_API virtual ~TransformationMatrix3DReader();

    /// Read the file with standard iostream API.
    FWDATAIO_API virtual void read();

    /// Defines extension supported by this reader ".trf"
    FWDATAIO_API virtual std::string extension();

};

}

}

#endif /*  _DATAIO_READER_TRANSFORMATIONMATRIXREADER3D_HPP_ */
