/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUIWX_PLUGIN_HPP_
#define _GUIWX_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "guiWx/config.hpp"

namespace guiWx
{
/**
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 */
class GUIWX_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    GUIWX_API ~Plugin() throw();

    // Overrides
    GUIWX_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    GUIWX_API void stop() throw();

    GUIWX_API int run() throw();
};

} // namespace guiWx

#endif //_GUIWX_PLUGIN_HPP_
