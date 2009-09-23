/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUI_PLUGIN_HPP_
#define _GUI_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

/**
 * @brief	Namespace gui.
 * @namespace	gui.
 * @author	IRCAD (Research and Development Team).
 * @author	Jean-Baptiste Fasquel.
 *
 * @date	2009.
 *
 *  The namespace gui contains the basic services to build the application IHM (ie: wxContainer, wxMenu, wxLayer...).
 */
namespace gui
{

/**
 * @brief	This class is called when the gui bundle is loaded.
 * @class	Plugin.
 * @author	IRCAD (Research and Development Team).
 * @author	Jean-Baptiste Fasquel.
 * @date	2009.
 */

class Plugin : public ::fwRuntime::Plugin
{

public:

	 ///@brief	Destructor. Do nothing.
	~Plugin() throw();

	/**
	 * @brief This method is used by runtime to initialize the bundle.
	 *
	 * @exception ::fwRuntime::RuntimeException.
	 *
	 * Register first basic services of type IAction, IEditor, IAspect and IView.
	 * Secondly, verifies if the command line and profile.xml are well formed and starts the wxWidgets IHM.
	 */
	void start() throw(::fwRuntime::RuntimeException);

	/**
	* @brief This method is used by runtime to close the bundle. Do nothing.
	*/
	void stop() throw();
	
private:
	/**
	 * @brief This method is used to verify if the command line and profile.xml are well formed.
	 * @todo Write Clauncher precondition.
	 */
	void startMethodPreconditionTest();
};

} // namespace gui

#endif //_GUI_PLUGIN_HPP_
