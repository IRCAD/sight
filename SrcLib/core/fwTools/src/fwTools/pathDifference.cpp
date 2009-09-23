/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <algorithm>

#include "fwTools/pathDifference.hpp"


namespace fwTools
{
boost::filesystem::path getPathDifference( boost::filesystem::path path1, boost::filesystem::path path2 )
{
	using boost::filesystem::path;
	path pathDiff;
	
	// search iterator on path for the first element difference
	std::pair<path::iterator, path::iterator> fstChange;
	fstChange = std::mismatch(path1.begin(), path1.end(), path2.begin()) ;

	path::iterator i1 = fstChange.first ;
	path::iterator i2 = fstChange.second;

	// move updir from path1 position : climb to first difference in folder hierarchy
	while (i1 != path1.end() )
	{
		pathDiff /= "..";
		++i1;
	}
	
	// mode down_dir to path2
	while (i2 != path2.end() )
	{
		pathDiff /= *i2;
		++i2;
	}
	
	return pathDiff;
}

}
