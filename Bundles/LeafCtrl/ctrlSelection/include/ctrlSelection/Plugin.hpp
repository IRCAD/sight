/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _CTRLSELECTION_PLUGIN_HPP_
#define _CTRLSELECTION_PLUGIN_HPP_

#include <fwRuntime/Plugin.hpp>

/**
 * @brief       Namespace ctrlSelection.
 * @namespace   ctrlSelection.
 * @author      IRCAD (Research and Development Team).
 * @date        2009.
 */
namespace ctrlSelection
{

class Plugin : public ::fwRuntime::Plugin
{

public :

        /// PLugin destructor
        ~Plugin() throw();
        
        /// This method is used by runtime to initialize the bundle.
        void start() throw( ::fwRuntime::RuntimeException );

        /// This method is used by runtime to stop the bundle.
        void stop() throw();
        
};

} // namespace ctrlSelection

#endif // _CTRLSELECTION_PLUGIN_HPP_
