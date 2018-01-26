/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwIO/config.hpp"

#include <boost/filesystem/path.hpp>

#include <string>
#include <vector>

namespace fwIO
{
//! IOPathType defines different type of paths used by service readers/writers
typedef enum
{
    FILE             = (1 << 0),  /*!< io service can manage file and cannot manage a list of file*/
    FILES            = (1 << 1),  /*!< io service can manage a list of file ()*/
    FOLDER           = (1 << 2),  /*!< io service can manage folder*/
    TYPE_NOT_DEFINED = (1 << 3),  /*!< io service not used path parameter to works */
} IOPathType;

/// List of paths managed by io services
typedef std::vector< ::boost::filesystem::path > LocationsType;

FWIO_API extern const std::string s_DATA_KEY;

} // namespace fwIO
