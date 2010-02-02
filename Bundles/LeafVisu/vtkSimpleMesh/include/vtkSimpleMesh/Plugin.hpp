/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKSIMPLEMESH_PLUGIN_HPP_
#define _VTKSIMPLEMESH_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

/**
 * @brief	Namespace vtkSimpleMesh.
 * @namespace	vtkSimpleMesh.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 *  The namespace vtkSimpleMesh contains a simple mesh renderer using vtk.
 */
namespace vtkSimpleMesh
{
class Plugin : public ::fwRuntime::Plugin
{
public:
	/**
	 * @brief	Destructor
	 */
	~Plugin() throw();
	
	/**
	* @brief Start method.
	*
	* @exception ::fwRuntime::RuntimeException.
	* This method is used by runtime to initialize the bundle.
	*/
	void start() throw(::fwRuntime::RuntimeException);

	/**
	* @brief Stop method.
	*
	* This method is used by runtime to stop the bundle.
	*/
	void stop() throw();
	
};

} // namespace vtkSimpleMesh

#endif //_VTKSIMPLEMESH_PLUGIN_HPP_
