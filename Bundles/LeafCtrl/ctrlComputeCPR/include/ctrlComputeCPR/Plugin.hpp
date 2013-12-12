/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

 
#ifndef __CTRLCOMPUTECPR_PLUGIN_HPP__
#define __CTRLCOMPUTECPR_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "ctrlComputeCPR/config.hpp"

namespace ctrlComputeCPR
{

/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 */
struct CTRLCOMPUTECPR_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    CTRLCOMPUTECPR_API ~Plugin() throw();

    /// Overrides
    CTRLCOMPUTECPR_API void start() throw(::fwRuntime::RuntimeException);

    /// Overrides
    CTRLCOMPUTECPR_API void stop() throw();

};


} // namespace ctrlComputeCPR

#endif // __CTRLCOMPUTECPR_PLUGIN_HPP__

