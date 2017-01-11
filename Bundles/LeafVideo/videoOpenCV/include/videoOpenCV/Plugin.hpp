/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VIDEOOPENCV_PLUGIN_HPP__
#define __VIDEOOPENCV_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "videoOpenCV/config.hpp"

namespace videoOpenCV
{
struct Plugin : public fwRuntime::Plugin
{
    /**
     * @brief Destructor
     */
    ~Plugin() throw();

    /**
     * @brief Start method.
     *
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime in order to initialize the bundle.
     */
    void start() throw(fwRuntime::RuntimeException);

    /**
     * @brief Stop method.
     *
     * This method is used by runtime in order to close the bundle.
     */
    void stop() throw();
};

} // namespace videoOpenCV

#endif // __VIDEOOPENCV_PLUGIN_HPP__
