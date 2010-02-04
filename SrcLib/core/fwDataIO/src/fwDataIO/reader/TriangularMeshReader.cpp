/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Object.hpp>
#include <fwData/TriangularMesh.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/reader/TriangularMeshReader.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::TriangularMeshReader, ::fwDataIO::reader::TriangularMeshReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

TriangularMeshReader::TriangularMeshReader()
: ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

TriangularMeshReader::~TriangularMeshReader()
{}

//------------------------------------------------------------------------------

void TriangularMeshReader::read()
{
	assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
	::boost::filesystem::path path = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();


	OSLM_INFO( "[TriangularMeshReader::read] Trian file: " << path);
	assert( path.empty() ==  false );

	::boost::shared_ptr< ::fwData::TriangularMesh > triMesh = getConcreteObject();

	std::fstream file;
	file.open(path.native_file_string().c_str(), std::fstream::in);
	if (!file.is_open())
	{
		OSLM_ERROR( "Trian file loading error for " << path);
		std::string str = "Unable to open ";
		str+= path.native_file_string();
		throw std::ios_base::failure(str);
	}

	/// Remove all points and cells
	triMesh->points().clear();
	triMesh->cells().clear();

	/// Read content and update mesh data structure
	unsigned int i, nbPts, nbCells;
	file>>nbPts;
	for( i=0 ; i<nbPts ; ++i )
	{
		std::vector< float > point(3) ;
		file>>point[0]>>point[1]>>point[2];
		triMesh->points().push_back( point ) ;
	}

	file>>nbCells;
	for( i=0 ; i<nbCells ; ++i )
	{
		int a, b, c;
		std::vector< int > cell(3) ;
		file>>cell[0]>>cell[1]>>cell[2]>>a>>b>>c;
		triMesh->cells().push_back( cell ) ;
	}
	file.close();

}



std::string  &TriangularMeshReader::extension()
{
	static std::string ext(".trian");
	return ext;
}


} // namespace reader

} // namespace fwDataIO
