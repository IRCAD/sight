/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWTOOLS_PROGRESSTOLOGGER_HPP__
#define __FWTOOLS_PROGRESSTOLOGGER_HPP__

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
class FWTOOLS_CLASS_API ProgressToLogger  : public ::boost::signals2::trackable
{
public:

    /**
     * @brief constructor : define an output to log system as :  [prefix] message percent [postfix]
     * @param[in] prefix  allow to add a prefix in the formated log message
     * @param[in] postfix  allow to add a postfix in the formated log message
     */
    FWTOOLS_API ProgressToLogger(std::string prefix, std::string postfix = " %");

    /// default destructor, do nothing
    FWTOOLS_API virtual ~ProgressToLogger();

    /**
     * @brief the handler effective code : send formated progress information to the log system SpyLog.
     * The formated ouput look like [prefix] msg percent [postfix]
     */
    FWTOOLS_API void operator()(float percent, std::string msg);

protected:

    std::string m_prefix;
    std::string m_postfix;
};

}

#endif /* __FWTOOLS_PROGRESSTOLOGGER_HPP__ */
