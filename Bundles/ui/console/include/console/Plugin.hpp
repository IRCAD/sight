/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CONSOLE_PLUGIN_HPP__
#define __CONSOLE_PLUGIN_HPP__

#include "console/config.hpp"

#include <fwRuntime/Plugin.hpp>

namespace fwThread
{
class Worker;
} //namespace fwThread

namespace console
{
/**
 * @brief   This class is called when the console bundle is loaded.
 */
class CONSOLE_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /// Destructor
    CONSOLE_API ~Plugin() noexcept;

    /// @override
    CONSOLE_API void start();

    /// @override
    CONSOLE_API void stop() noexcept;

    /// Run callback
    CONSOLE_API int run() noexcept;

private:

    SPTR(::fwThread::Worker) m_worker;

};

} // namespace console

#endif //__CONSOLE_PLUGIN_HPP__
