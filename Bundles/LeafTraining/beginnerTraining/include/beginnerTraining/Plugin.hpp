/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __BEGINNERTRAINING_PLUGIN_HPP__
#define __BEGINNERTRAINING_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "beginnerTraining/config.hpp"

namespace beginnerTraining
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2012.
 */
struct BEGINNERTRAINING_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    BEGINNERTRAINING_API ~Plugin() noexcept;

    /// Overrides
    BEGINNERTRAINING_API void start();

    /// Overrides
    BEGINNERTRAINING_API void stop() noexcept;

};


} // namespace beginnerTraining


#endif //__BEGINNERTRAINING_PLUGIN_HPP__
