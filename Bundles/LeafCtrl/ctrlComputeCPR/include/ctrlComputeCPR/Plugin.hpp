/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef __CTRLCOMPUTECPR_PLUGIN_HPP__
#define __CTRLCOMPUTECPR_PLUGIN_HPP__

#include "ctrlComputeCPR/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace ctrlComputeCPR
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
struct CTRLCOMPUTECPR_CLASS_API Plugin : public ::fwRuntime::Plugin
{

    /// Destructor
    CTRLCOMPUTECPR_API ~Plugin() noexcept;

    /// Overrides
    CTRLCOMPUTECPR_API void start();

    /// Overrides
    CTRLCOMPUTECPR_API void stop() noexcept;

};


} // namespace ctrlComputeCPR

#endif // __CTRLCOMPUTECPR_PLUGIN_HPP__

