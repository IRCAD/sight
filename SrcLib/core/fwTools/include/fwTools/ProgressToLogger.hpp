/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
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
 * @brief 	Class ProgressToLogger
 * @class 	ProgressToLogger
 * @author	IRCAD (Research and Development Team).
 * @date	2007-2009.
 * @todo 	Complete doxygen
 */
class FWTOOLS_CLASS_API ProgressToLogger  : public ::boost::signals::trackable // to autoDisconnect if handler is destroyed before the notifier
{
public:

	FWTOOLS_API ProgressToLogger(std::string prefix, std::string postfix= " %");

	FWTOOLS_API virtual ~ProgressToLogger();

	FWTOOLS_API void operator()(float percent, std::string msg);

protected :

	std::string m_prefix;
	std::string m_postfix;
};

}

#endif /* PROGRESSTOLOGGER_HPP_ */
