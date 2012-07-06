/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef _BEGINNERTRAINING_PLUGIN_HPP_
#define _BEGINNERTRAINING_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "beginnerTraining/config.hpp"

namespace beginnerTraining
{

/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin.
* @author  IRCAD (Research and Development Team).
* @date    2012.
*/
struct BEGINNERTRAINING_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    BEGINNERTRAINING_API ~Plugin() throw();

    /// Overrides
    BEGINNERTRAINING_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    BEGINNERTRAINING_API void stop() throw();

};


} // namespace beginnerTraining


#endif //_BEGINNERTRAINING_PLUGIN_HPP_
