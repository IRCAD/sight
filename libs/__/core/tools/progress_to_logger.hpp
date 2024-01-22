/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#pragma once

#include <core/config.hpp>

#include <boost/signals2.hpp>

#include <string>

namespace sight::core::tools
{

/**
 * @brief   Class progress_to_logger is an progress_adviser::ProgressHandler, it receive notification from its
 * progress_adviser
 * and send the progress to the logging system ( SpyLog )
 */
class CORE_CLASS_API progress_to_logger : public boost::signals2::trackable
{
public:

    /**
     * @brief constructor : define an output to log system as :  [prefix] message percent [postfix]
     * @param[in] _prefix  allow to add a prefix in the formated log message
     * @param[in] _postfix  allow to add a postfix in the formated log message
     */
    CORE_API progress_to_logger(std::string _prefix, std::string _postfix = " %");

    /// default destructor, do nothing
    CORE_API virtual ~progress_to_logger();

    /**
     * @brief the handler effective code : send formated progress information to the log system SpyLog.
     * The formated ouput look like [prefix] msg percent [postfix]
     */
    CORE_API void operator()(float _percent, const std::string& _msg);

protected:

    std::string m_prefix;
    std::string m_postfix;
};

} // namespace sight::core::tools
