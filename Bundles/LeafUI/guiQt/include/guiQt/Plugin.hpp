/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUIQT_PLUGIN_HPP_
#define _GUIQT_PLUGIN_HPP_

#include <QPointer>

#include <fwRuntime/Plugin.hpp>

#include "guiQt/config.hpp"

class QApplication;

namespace guiQt
{
/**
 * @author  IRCAD (Research and Development Team).
 * @date    2009-2010.
 */
class GUIQT_CLASS_API Plugin : public ::fwRuntime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    GUIQT_API ~Plugin() throw();

    // Overrides
    GUIQT_API void start() throw(::fwRuntime::RuntimeException);

    // Overrides
    GUIQT_API void stop() throw();


    GUIQT_API int run() throw();

private:
    QPointer< QApplication > m_app;
    int m_argc;

};

} // namespace guiQt

#endif //_GUIQT_PLUGIN_HPP_
