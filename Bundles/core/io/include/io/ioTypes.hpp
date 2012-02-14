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
    //! IOPathType defines different type of paths used by service readers/writers
    typedef enum
    {
        FILE   = 1 << 0, /*!< io service can manage file and cannot manage a list of file*/
        FILES  = 1 << 1, /*!< io service can manage a list of file ()*/
        FOLDER = 1 << 2, /*!< io service can manage folder*/
        TYPE_NOT_DEFINED = 1 << 3, /*!< io service not used path parameter to works */
    } IOPathType;

    /// List of paths managed by io services
    typedef std::vector< ::boost::filesystem::path > LocationsType;
}


#endif /* _IO_IOTYPES_HPP_ */
