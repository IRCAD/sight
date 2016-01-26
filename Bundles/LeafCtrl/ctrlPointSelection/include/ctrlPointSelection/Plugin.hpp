/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __CTRLPOINTSELECTION_PLUGIN_HPP__
#define __CTRLPOINTSELECTION_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlPointSelection/config.hpp"

namespace ctrlPointSelection
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLPOINTSELECTION_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    CTRLPOINTSELECTION_API ~Plugin() throw();

    /// Overrides
    CTRLPOINTSELECTION_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    CTRLPOINTSELECTION_API void stop() throw();

};

} // namespace ctrlPointSelection

#endif // __CTRLPOINTSELECTION_PLUGIN_HPP__

