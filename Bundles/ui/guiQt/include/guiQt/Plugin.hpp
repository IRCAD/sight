/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_PLUGIN_HPP__
#define __GUIQT_PLUGIN_HPP__

#include "guiQt/config.hpp"

#include <fwRuntime/Plugin.hpp>


namespace fwThread
{
class Worker;
} //namespace fwThread


namespace guiQt
{
/**
 * @brief   This class is called when the guiQt bundle is loaded.
 * @class   Plugin
 */
class GUIQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    GUIQT_API ~Plugin() noexcept;

    // Overrides
    GUIQT_API void start();

    // Overrides
    GUIQT_API void stop() noexcept;


    GUIQT_API int run() noexcept;

protected:

    GUIQT_API void loadStyleSheet();

private:

    SPTR(::fwThread::Worker) m_workerQt;

};

} // namespace guiQt

#endif //__GUIQT_PLUGIN_HPP__
