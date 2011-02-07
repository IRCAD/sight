/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_PLUGIN_HPP_
#define _GUI_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

#include "gui/export.hpp"

namespace gui
{

/**
 * @brief   This class is called when the gui bundle is loaded.
 * @class   Plugin.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class GUI_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

     ///@brief  Destructor. Do nothing.
    GUI_API virtual ~Plugin() throw();

    /**
     * @brief This method is used by runtime to initialize the bundle.
     *
     * @exception ::fwRuntime::RuntimeException.
     *
     * Register first basic services of type IAction, IEditor and IView.
     * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
     */
    GUI_API void start() throw(::fwRuntime::RuntimeException);

    /**
    * @brief This method is used by runtime to close the bundle. Do nothing.
    */
    GUI_API void stop() throw();

};

} // namespace gui

#endif //_GUI_PLUGIN_HPP_
