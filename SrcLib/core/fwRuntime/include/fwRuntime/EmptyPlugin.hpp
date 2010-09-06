/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWRUNTIME_EMPTYPLUGIN_HPP_
#define _FWRUNTIME_EMPTYPLUGIN_HPP_


#include "fwRuntime/config.hpp"
#include "fwRuntime/Plugin.hpp"


namespace fwRuntime
{


/**
 * @struct  EmptyPlugin
 * @brief   Implements a default plugin for bundles that don't provide a
 *          ::fwRuntime::IPlugin interface implementation. This plugin does nothing
 *          by default. It has not been design to be subclassed, but subclassing
 *          is neither forbidden.
 *
 * @remark  The factory for this executable structure is registered by the runtime
 *          it-self.
 *
 * @date    2004-2005
 * @author  IRCAD (Research and Development Team).
 */
struct EmptyPlugin : public Plugin
{

    /**
     * @brief   Destructor : does nothing.
     */
    FWRUNTIME_API virtual ~EmptyPlugin() throw() {}


    // Overrides
    virtual void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    virtual void stop() throw();

};


} // namespace fwRuntime


#endif //_FWRUNTIME_EMPTYPLUGIN_HPP_
