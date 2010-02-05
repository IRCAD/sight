/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef PROGRESSTOLOGGER_HPP_
#define PROGRESSTOLOGGER_HPP_

#include <string>
#include <fwTools/config.hpp>
#include <boost/signals/trackable.hpp>

namespace fwTools {

/**
 * @brief       Class ProgressToLogger is an ProgressAdviser::ProgressHandler, it receive notification from its ProgressAdviser
 * and send the progress to the logging system ( SpyLog )
 * @class       ProgressToLogger
 * @author      IRCAD (Research and Development Team).
 * @date        2007-2009.
 */
class FWTOOLS_CLASS_API ProgressToLogger  : public ::boost::signals::trackable // to autoDisconnect if handler is destroyed before the notifier
{
public:

        /**
         * @brief constructor : define an output to log system as :  [prefix] message percent [postfix]
         * @param[in] prefix  allow to add a prefix in the formated log message
         * @param[in] postfix  allow to add a postfix in the formated log message
         */
        FWTOOLS_API ProgressToLogger(std::string prefix, std::string postfix= " %");

        /// default destructor, do nothing
        FWTOOLS_API virtual ~ProgressToLogger();

        /**
         * @brief the handler effective code : send formated progress information to the log system SpyLog.
         * The formated ouput look like [prefix] msg percent [postfix]
         */
        FWTOOLS_API void operator()(float percent, std::string msg);

protected :

        std::string m_prefix;
        std::string m_postfix;
};

}

#endif /* PROGRESSTOLOGGER_HPP_ */
