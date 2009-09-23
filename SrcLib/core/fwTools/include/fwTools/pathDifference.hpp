/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef PATHDIFFERENCE_HPP_
#define PATHDIFFERENCE_HPP_

#include "fwTools/config.hpp"
#include <boost/filesystem/path.hpp>


namespace fwTools
{
/**
 * @brief 	Return the relative difference within two paths
 *
 * Return 	The relative difference within two paths in such way :
 * \li cd path1;
 * \li cd  getPathDifference(path1,path2)
 * \li assert( pwd = path2 )
 *
 * @note	Here path are supposed to be attached in a same root
 *
 * examples :
 * \li	 path1="a/b/c" 									\n
 * 		path2="a/b/e/g" 								\n
 * 		getPathDifference(path1,path2)					\n
 * 		return "../e/g"
 *
 * \li 	path1="a/b/c" 									\n
 * 		path2="e/f/g" 									\n
 * 		getPathDifference(path1,path2)					\n
 * 		return "../../../e/f/g"
 *
 * \li 	path1="a" 										\n
 * 		path2="a" 										\n
 * 		getPathDifference(path1,path2)					\n
 * 		return ""
 *
 * \li 	path1="a" 										\n
 * 		path2="a/e" 									\n
 * 		getPathDifference(path1,path2)					\n
 * 		return "e"
 *
 * \li 	etc..
 */
FWTOOLS_API boost::filesystem::path getPathDifference( boost::filesystem::path path1, boost::filesystem::path path2 );

}

#endif /*PATHDIFFERENCE_HPP_*/
