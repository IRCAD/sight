/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VTKSIMPLENEGATO_PLUGIN_HPP__
#define __VTKSIMPLENEGATO_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>


/**
 * @brief   Namespace vtkSimpleNegato.
 * @namespace   vtkSimpleNegato
 *
 * @date    2009.
 *
 *  The namespace vtkSimpleNegato contains a simple negato renderer using vtk.
 */
namespace vtkSimpleNegato
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

} // namespace vtkSimpleNegato

#endif //__VTKSIMPLENEGATO_PLUGIN_HPP__
