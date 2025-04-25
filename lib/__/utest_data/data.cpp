/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "data.hpp"

#include "utest/exception.hpp"

#include <cstdlib>
#include <filesystem>
#include <sstream>

namespace sight::utest_data
{

static const std::string TEST_DATA_DIR_ENV_VAR("FWTEST_DATA_DIR");

//------------------------------------------------------------------------------

std::filesystem::path dir()
{
    char* val = std::getenv(TEST_DATA_DIR_ENV_VAR.c_str());
    if(val == nullptr)
    {
        std::stringstream msg;
        msg << "The '" << TEST_DATA_DIR_ENV_VAR
        << "' environment variable is not set.";
        throw utest::exception(msg.str());
    }

    std::filesystem::path datadir(val);

    if(!std::filesystem::exists(datadir))
    {
        std::stringstream msg;
        msg << "The path'" << datadir
        << "' doesn't seem to exist.";
        throw utest::exception(msg.str());
    }

    return datadir;
}

} // namespace sight::utest_data
