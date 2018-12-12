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

#include "fwTest/Exception.hpp"
#include "fwTest/Data.hpp"

#include <boost/filesystem/operations.hpp>

#include <cstdlib>
#include <sstream>


namespace fwTest
{

const std::string Data::s_TEST_DATA_DIR_ENV_VAR("FWTEST_DATA_DIR");

::boost::filesystem::path Data::dir()
{
    char * val = std::getenv(s_TEST_DATA_DIR_ENV_VAR.c_str());
    if (val == 0)
    {
        std::stringstream msg;
        msg << "The '" << s_TEST_DATA_DIR_ENV_VAR
            << "' environment variable is not set.";
        throw fwTest::Exception(msg.str());
    }

    ::boost::filesystem::path datadir(val);

    if (!::boost::filesystem::exists(datadir))
    {
        std::stringstream msg;
        msg << "The path'" << datadir
            << "' doesn't seem to exist.";
        throw fwTest::Exception(msg.str());
    }

    return datadir;
}


} // namespace fwTest


