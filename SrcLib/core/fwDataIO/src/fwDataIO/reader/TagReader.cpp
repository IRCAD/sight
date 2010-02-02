/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <iostream>
#include <fstream>

#include <boost/cstdint.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwData/Tag.hpp>
#include <fwData/PointList.hpp>
#include <fwData/Point.hpp>
#include <fwData/Object.hpp>
#include <fwData/location/SingleFile.hpp>

#include "fwDataIO/reader/TagReader.hpp"

#include <fwMath/IntrasecTypes.hpp>

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::reader::IObjectReader , ::fwDataIO::reader::TagReader, ::fwDataIO::reader::TagReader );


namespace fwDataIO
{

namespace reader
{

//------------------------------------------------------------------------------

TagReader::TagReader()
: ::fwData::location::enableSingleFile< IObjectReader >(this)
{}

//------------------------------------------------------------------------------

TagReader::~TagReader()
{}

//------------------------------------------------------------------------------

void TagReader::read()
{
	assert( ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location) );
	::boost::filesystem::path path = ::boost::dynamic_pointer_cast< ::fwData::location::SingleFile >(m_location)->getPath();


	OSLM_INFO( "[TagReader::read] Tag file: " << path);
	assert( path.empty() ==  false );

	::boost::shared_ptr< ::fwData::Tag > tag = getConcreteObject();

	std::fstream file;
	file.open(path.native_file_string().c_str(), std::fstream::in);
	if (!file.is_open())
	{
		OSLM_ERROR( "Tag file loading error for " << path);
		std::string str = "Unable to open ";
		str+= path.native_file_string();
		throw std::ios_base::failure(str);
	}

	/// Read content and update tag data structure
	std::string name, type;
	int n, nbPts;
	double x,y,z,radius=0.0;
	file>>name;
	file>>n;
	if(n>=1)
	{
		file>>type;

		if(type=="ARTAG" || type=="CHESSBOARD" || type=="ARToolKitPlus_MARKER_ID_BCH")
		{
			file>>x>>y>>z;
			file>>nbPts;
			tag->setType(type);
			for(int i=0; i < nbPts; i++)
			{
				::fwData::Point::sptr p;
//				::fwData::Point::PointCoordArray vPoint;
				fwVec3d vPoint;
				file>>vPoint[0]>>vPoint[1]>>vPoint[2]>>radius;
				p->setCRefCoord(vPoint);
				tag->getRefPointList()->getRefPoints().push_back(p);
			}
		}
		else
		{
			OSLM_ERROR( "Tag file loading error for " << path<<" with type "<<type);
			std::string str = "Unable to open ";
			str+= path.native_file_string();
			throw std::ios_base::failure(str);
		}
	}

	file.close();

}

//------------------------------------------------------------------------------

std::string & TagReader::extension()
{
	static std::string ext(".tag");
	return ext;
}

//------------------------------------------------------------------------------

} // namespace reader

} // namespace fwDataIO
