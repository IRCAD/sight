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
    fstChange = std::mismatch(path1.begin(), path1.end(), path2.begin());

    path::iterator i1 = fstChange.first;
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
