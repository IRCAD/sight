/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _OPVTKMESH_PLUGIN_HPP_
#define _OPVTKMESH_PLUGIN_HPP_


#include <fwRuntime/Plugin.hpp>


namespace opVTKMesh
{


/**
 * @brief   Implements a plugin for the opVTKMesh bundle.
 */
struct Plugin : public ::fwRuntime::Plugin
{

    /**
     * @brief   destructor
     */
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};


} // namespace opVTKMesh


#endif //_OPVTKMESH_PLUGIN_HPP_
