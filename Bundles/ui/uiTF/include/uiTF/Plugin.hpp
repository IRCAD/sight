/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UITF_PLUGIN_HPP__
#define __UITF_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiTF/config.hpp"

namespace uiTF
{
/**
 *
 * @date    2009-2010.
 */
class UITF_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    UITF_API ~Plugin() noexcept;

    // Overrides
    UITF_API void start();

    // Overrides
    UITF_API void stop() noexcept;

};

} // namespace uiTF

#endif //__UITF_PLUGIN_HPP__
