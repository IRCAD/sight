/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQTEXT_PLUGIN_HPP__
#define __GUIQTEXT_PLUGIN_HPP__

#include <fwRuntime/Plugin.hpp>

#include "guiQtExt/config.hpp"

namespace fwThread
{
class Worker;
} //namespace fwThread


namespace guiQtExt
{
/**
 *
 * @date    2009-2010.
 */
class GUIQTEXT_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    GUIQTEXT_API ~Plugin() throw();

    // Overrides
    GUIQTEXT_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    GUIQTEXT_API void stop() throw();


};

} // namespace guiQtExt

#endif //__GUIQTEXT_PLUGIN_HPP__
