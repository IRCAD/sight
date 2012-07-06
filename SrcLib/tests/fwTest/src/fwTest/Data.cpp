/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2010.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <cstdlib>
#include <sstream>

#include <boost/filesystem/operations.hpp>

#include "fwTest/Exception.hpp"
#include "fwTest/Data.hpp"

namespace fwTest
{

const std::string Data::s_TEST_DATA_DIR_ENV_VAR("FWTEST_DATA_DIR");

boost::filesystem::path Data::dir() throw(fwTest::Exception)
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


