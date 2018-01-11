/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKCOMPOSITEMESH_PLUGIN_HPP__
#define __VTKCOMPOSITEMESH_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

/**
 * @brief   Namespace vtkCompositeMesh.
 * @namespace   vtkCompositeMesh
 *
 * @date    2009.
 *
 *  The namespace vtkCompositeMesh contains a COMPOSITE mesh renderer using vtk.
 */
namespace vtkCompositeMesh
{
class Plugin : public ::fwRuntime::Plugin
{
public:
    /**
     * @brief   Destructor
     */
    ~Plugin() noexcept;

    /**
     * @brief Start method.
     *
     * @exception ::fwRuntime::RuntimeException.
     * This method is used by runtime to initialize the bundle.
     */
    void start();

    /**
     * @brief Stop method.
     *
     * This method is used by runtime to stop the bundle.
     */
    void stop() noexcept;

};

} // namespace vtkCompositeMesh

#endif //__VTKCOMPOSITEMESH_PLUGIN_HPP__
