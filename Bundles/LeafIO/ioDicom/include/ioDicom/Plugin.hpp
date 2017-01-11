/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOM_PLUGIN_HPP__
#define __IODICOM_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

namespace ioDicom
{

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

} // namespace ioDicom

#endif //__IODICOM_PLUGIN_HPP__
