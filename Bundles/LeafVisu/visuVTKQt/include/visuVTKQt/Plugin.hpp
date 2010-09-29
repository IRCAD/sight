/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKQT_PLUGIN_HPP_
#define _VISUVTKQT_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "visuVTKQt/config.hpp"

namespace visuVTKQt
{
/**
 * @brief   This class is started when the bundles is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */
struct VISUVTKQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{
    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    VISUVTKQT_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    VISUVTKQT_API void stop() throw();

};

} // namespace visuVTKQt

#endif //_VISUVTKQT_PLUGIN_HPP_
