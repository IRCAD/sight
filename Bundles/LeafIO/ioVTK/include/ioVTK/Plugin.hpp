/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _ioVTK_PLUGIN_HPP_
#define _ioVTK_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

namespace ioVTK
{

class Plugin : public ::fwRuntime::Plugin
{
public:
    //! @brief Destructor.
    ~Plugin() throw();

    /**
    * @brief Start method.
    *
    * @exception ::fwRuntime::RuntimeException.
    * This method is used by runtime in order to initialize the bundle.
    */
    void start() throw(::fwRuntime::RuntimeException);

    /**
    * @brief Stop method.
    *
    * This method is used by runtime in order to close the bundle.
    */
    void stop() throw();

};

} // namespace ioVTK

#endif //_ioVTK_PLUGIN_HPP_
