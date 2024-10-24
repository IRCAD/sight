/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

# include <core/log/spy_logger.hpp>

#include <cstdio>
#include <filesystem>
#include <ostream>

namespace sight::utest_data
{

struct log_initialization
{
    log_initialization()
    {
        std::string log_file = "fwTest.log";

        FILE* p_file = fopen(log_file.c_str(), "w");
        if(p_file == nullptr)
        {
            std::error_code err;
            std::filesystem::path sys_tmp = std::filesystem::temp_directory_path(err);
            if(err.value() != 0)
            {
                // replace log file appender by stream appender: default dir and temp dir unreachable
                core::log::spy_logger::add_global_console_log();
            }
            else
            {
                // creates SLM.log in temp directory: default dir unreachable
                sys_tmp  = sys_tmp / log_file;
                log_file = sys_tmp.string();
                core::log::spy_logger::add_global_file_log(log_file);
            }
        }
        else
        {
            // creates SLM.log in default logFile directory
            if(fclose(p_file) != 0)
            {
                perror("fclose");
            }

            core::log::spy_logger::add_global_file_log(log_file);
        }
    }
};

static log_initialization init;

} //namespace sight::utest_data
