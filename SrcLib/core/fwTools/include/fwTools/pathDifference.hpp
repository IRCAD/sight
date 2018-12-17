/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#ifndef __FWTOOLS_PATHDIFFERENCE_HPP__
#define __FWTOOLS_PATHDIFFERENCE_HPP__

#include "fwTools/config.hpp"
#include <boost/filesystem/path.hpp>


namespace fwTools
{
/**
 * @brief   Return the relative difference within two paths
 *
 * Return   The relative difference within two paths in such way :
 * \li cd path1;
 * \li cd  getPathDifference(path1,path2)
 * \li assert( pwd = path2 )
 *
 * @note    Here path are supposed to be attached in a same root
 *
 * examples :
 * \li   path1="a/b/c"                                  \n
 *      path2="a/b/e/g"                                 \n
 *      getPathDifference(path1,path2)                  \n
 *      return "../e/g"
 *
 * \li  path1="a/b/c"                                   \n
 *      path2="e/f/g"                                   \n
 *      getPathDifference(path1,path2)                  \n
 *      return "../../../e/f/g"
 *
 * \li  path1="a"                                       \n
 *      path2="a"                                       \n
 *      getPathDifference(path1,path2)                  \n
 *      return ""
 *
 * \li  path1="a"                                       \n
 *      path2="a/e"                                     \n
 *      getPathDifference(path1,path2)                  \n
 *      return "e"
 *
 * \li  etc..
 */
FWTOOLS_API boost::filesystem::path getPathDifference( boost::filesystem::path path1, boost::filesystem::path path2 );

}

#endif /*__FWTOOLS_PATHDIFFERENCE_HPP__*/
