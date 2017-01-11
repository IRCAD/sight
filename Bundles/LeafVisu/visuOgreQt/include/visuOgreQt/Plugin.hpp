/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREQT_PLUGIN_HPP__
#define __VISUOGREQT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "visuOgreQt/config.hpp"

namespace visuOgreQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin
 *
 * @date    2009.
 */
struct VISUOGREQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    VISUOGREQT_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUOGREQT_API void stop() throw();

};

} // namespace visuOgreQt

#endif //__VISUOGREQT_PLUGIN_HPP__
