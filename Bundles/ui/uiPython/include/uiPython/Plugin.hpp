/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIPYTHON_PLUGIN_HPP__
#define __UIPYTHON_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "uiPython/config.hpp"

namespace uiPython
{

/**
 * @brief   This class is called when the uiPython bundle is loaded.
 */

class UIPYTHON_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    ///@brief  Destructor. Do nothing.
    UIPYTHON_API virtual ~Plugin() noexcept;

    /**
     * @brief This method is used by runtime to initialize the bundle.
     *
     * @exception ::fwRuntime::RuntimeException.
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the IHM.
     */
    UIPYTHON_API void start();

    /**
     * @brief This method is used by runtime to close the bundle. Do nothing.
     */
    UIPYTHON_API void stop() noexcept;

};

} // namespace uiPython

#endif //__UIPYTHON_PLUGIN_HPP__
