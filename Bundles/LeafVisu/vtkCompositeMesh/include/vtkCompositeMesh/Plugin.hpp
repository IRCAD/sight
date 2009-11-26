/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#ifndef _VTKCOMPOSITEMESH_PLUGIN_HPP_
#define _VTKCOMPOSITEMESH_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

/**
 * @brief	Namespace vtkCompositeMesh.
 * @namespace	vtkCompositeMesh.
 * @author	IRCAD (Research and Development Team).
 * @date	2009.
 *
 *  The namespace vtkCompositeMesh contains a COMPOSITE mesh renderer using vtk.
 */
namespace vtkCompositeMesh
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

} // namespace vtkCompositeMesh

#endif //_VTKCOMPOSITEMESH_PLUGIN_HPP_
