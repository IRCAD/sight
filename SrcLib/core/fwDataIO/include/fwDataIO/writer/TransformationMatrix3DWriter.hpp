/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _DATAIO_READER_TRANSFORMATIONMATRIX3DWRITER_HPP_
#define _DATAIO_READER_TRANSFORMATIONMATRIX3DWRITER_HPP_

#include <boost/shared_ptr.hpp>
#include <fwData/TransformationMatrix3D.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"

#include "fwDataIO/writer/GenericObjectWriter.hpp"


namespace fwDataIO {

namespace writer {

/**
 * @brief	Writer for transformation of 3D Matrix. Write file format .trf
 * @class	TransformationMatrix3DWriter.
 * @author	IRCAD (Research and Development Team).
 * @date	2009
 *
 * Ircad writer to write a ::fwData::TransformationMatrix3D on filesystem which the file format
 * is .trf. This format is basic. This is an ascii file which contains matrix 4x4 values
 */
class FWDATAIO_CLASS_API TransformationMatrix3DWriter :
	public GenericObjectWriter< ::fwData::TransformationMatrix3D>,
	public ::fwData::location::enableSingleFile< ::fwDataIO::writer::IObjectWriter >
{

public:

	/// Constructor. Do nothing.
	FWDATAIO_API TransformationMatrix3DWriter();

	/// Destructor. Do nothing.
	FWDATAIO_API virtual ~TransformationMatrix3DWriter();

	/// Write the file with standard iostream API.
	FWDATAIO_API virtual void write();

	/// Defines extension supported by this writer ".trf"
	FWDATAIO_API virtual std::string  extension();

};

}

}

#endif /* _DATAIO_READER_TRANSFORMATIONMATRIX3DWRITER_HPP_ */
