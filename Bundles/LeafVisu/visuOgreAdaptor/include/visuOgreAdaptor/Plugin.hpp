/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_PLUGIN_HPP__
#define __VISUOGREADAPTOR_PLUGIN_HPP__

#include "visuOgreAdaptor/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class VISUOGREADAPTOR_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /**
     * @brief   destructor
     */
    ~Plugin() noexcept;

    // Overrides
    VISUOGREADAPTOR_API void start();

    // Overrides
    VISUOGREADAPTOR_API void stop() noexcept;
};

} // namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_PLUGIN_HPP__
