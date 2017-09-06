/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUI_PLUGIN_HPP__
#define __GUI_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "gui/config.hpp"

namespace gui
{

/**
 * @brief   This class is called when the gui bundle is loaded.
 * @class   Plugin
 */

class GUI_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    ///@brief  Destructor. Do nothing.
    GUI_API virtual ~Plugin() noexcept;

    /**
     * @brief This method is used by runtime to initialize the bundle.
     *
     * @exception ::fwRuntime::RuntimeException
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    GUI_API void start();

    /**
     * @brief This method is used by runtime to close the bundle. Do nothing.
     */
    GUI_API void stop() noexcept;

};

} // namespace gui

#endif //__GUI_PLUGIN_HPP__
