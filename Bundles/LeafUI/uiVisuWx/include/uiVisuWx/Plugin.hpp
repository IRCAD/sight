/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUWX_PLUGIN_HPP_
#define _UIVISUWX_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

namespace uiVisu
{
/**
 * @namespace uiVisu
 * @brief   Contains editors for visualization.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

struct Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};

} // namespace uiVisu

#endif //_UIVISUWX_PLUGIN_HPP_
