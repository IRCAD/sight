/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATAIO_READER_TRIANGULARMESH_HPP_
#define _FWDATAIO_READER_TRIANGULARMESH_HPP_

#include <boost/filesystem/path.hpp>
#include <boost/shared_ptr.hpp>

#include <fwData/TriangularMesh.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/config.hpp"
#include "fwDataIO/reader/GenericObjectReader.hpp"

namespace fwData {
	class TriangularMesh;
}

namespace fwDataIO
{

namespace reader
{

/**
 * @brief	Mesh reader. Read file format .trian
 * @class	TriangularMeshReader.
 * @author	IRCAD (Research and Development Team).
 * @date	2009
 *
 * Ircad reader to read a ::fwData::TriangularMesh on filesystem which the file format
 * is .trian. This format is basic. This is an ascii file which contains : 1) nb of points
 * 2) list of points (x,y,z). Line numbers represent index of point in vector 3) nb of triangles
 * 4) list of triangles with index of points and normals ( often set a -1 )
 */
class FWDATAIO_CLASS_API TriangularMeshReader :
	public GenericObjectReader< ::fwData::TriangularMesh>,
	public ::fwData::location::enableSingleFile< IObjectReader >
{

public :

	fwCoreClassDefinitionsWithFactoryMacro( 	(TriangularMeshReader)(GenericObjectReader< ::fwData::TriangularMesh >::Baseclass),
												 (()),
												 new  TriangularMeshReader
											);

	/// Constructor. Do nothing.
	FWDATAIO_API TriangularMeshReader();

	/// Destructor. Do nothing.
	FWDATAIO_API virtual ~TriangularMeshReader();

	/// Read the file with standard iostream API.
	FWDATAIO_API virtual void read();

	/// Defines extension supported by this reader ".trian"
	FWDATAIO_API std::string  &extension();

};

} // namespace reader

} // namespace fwDataIO


#endif // _FWDATAIO_READER_TRIANGULARMESH_HPP_
