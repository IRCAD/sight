/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __OPPOCMESHER_PLUGIN_HPP__
#define __OPPOCMESHER_PLUGIN_HPP__


#include <fwRuntime/Plugin.hpp>


namespace opPOCMesher
{


/// Implements a plugin for the opPOCMesher bundle.
struct Plugin : public ::fwRuntime::Plugin
{

    /// destructor
    ~Plugin() throw();

    // Overrides
    void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    void stop() throw();

};


} // namespace opPOCMesher


#endif //__OPPOCMESHER_PLUGIN_HPP__
