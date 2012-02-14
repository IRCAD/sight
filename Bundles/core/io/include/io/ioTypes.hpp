/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _IO_IOTYPES_HPP_
#define _IO_IOTYPES_HPP_

#include <boost/filesystem/path.hpp>

namespace io
{
    typedef enum
    {
        FILE   = 1 << 0,
        FILES  = 1 << 1,
        FOLDER = 1 << 2
    } IOPathType;

    typedef std::vector< ::boost::filesystem::path > LocationsType;
}


#endif /* _IO_IOTYPES_HPP_ */
