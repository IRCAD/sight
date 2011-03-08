/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */


#ifndef _BINDING_PLUGIN_HPP_
#define _BINDING_PLUGIN_HPP_


#include <fwRuntime/Plugin.hpp>

#include "binding/config.hpp"

namespace binding
{
/**
* @brief   This class is started when the bundles is loaded.
* @class   Plugin.
* @author  IRCAD (Research and Development Team).
* @date    2010.
*/

class BINDING_CLASS_API Plugin : public ::fwRuntime::Plugin
{
public:
    /**
     * @brief   destructor
     */
    BINDING_API ~Plugin() throw();

    // Overrides
    BINDING_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    BINDING_API void stop() throw();

};


} // namespace binding


#endif //_BINDING_PLUGIN_HPP_
