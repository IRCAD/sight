/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "io/dicom/helper/functions.hpp"

#include <boost/date_time/posix_time/posix_time.hpp>

#include <gdcmUIDGenerator.h>

#include <sstream>
#include <string>

namespace sight::io::dicom
{

namespace helper
{

//------------------------------------------------------------------------------

std::string generatePatientId()
{
    ::gdcm::UIDGenerator uid;
    const char* id = uid.Generate();
    return std::string(id);
}

//------------------------------------------------------------------------------

std::string generateStudyInstanceUid()
{
    using namespace ::boost::posix_time;

    ptime now         = microsec_clock::local_time();
    time_facet* facet = new time_facet("%f%S%M%H%d%m");

    std::stringstream ss;
    // facet ownership is given to the std::locale
    ss.imbue(std::locale(ss.getloc(), facet));
    ss << now;

    return ss.str();
}

} // namespace helper

} // namespace sight::io::dicom
