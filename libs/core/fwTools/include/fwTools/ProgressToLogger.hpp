/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
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

#pragma once

#include <fwTools/config.hpp>

#include <boost/signals2.hpp>

#include <string>

namespace fwTools
{

/**
 * @brief   Class ProgressToLogger is an ProgressAdviser::ProgressHandler, it receive notification from its
 * ProgressAdviser
 * and send the progress to the logging system ( SpyLog )
 */
class FWTOOLS_CLASS_API ProgressToLogger : public ::boost::signals2::trackable
{
public:

    /**
     * @brief constructor : define an output to log system as :  [prefix] message percent [postfix]
     * @param[in] prefix  allow to add a prefix in the formated log message
     * @param[in] postfix  allow to add a postfix in the formated log message
     */
    FWTOOLS_API ProgressToLogger(const std::string& prefix, const std::string& postfix = " %");

    /// default destructor, do nothing
    FWTOOLS_API virtual ~ProgressToLogger();

    /**
     * @brief the handler effective code : send formated progress information to the log system SpyLog.
     * The formated ouput look like [prefix] msg percent [postfix]
     */
    FWTOOLS_API void operator()(float percent, const std::string& msg);

protected:

    std::string m_prefix;
    std::string m_postfix;
};

}
