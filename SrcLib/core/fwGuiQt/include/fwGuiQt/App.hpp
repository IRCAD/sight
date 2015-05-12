/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGUIQT_APP_HPP__
#define __FWGUIQT_APP_HPP__

#include <QApplication>

#include <fwRuntime/profile/Profile.hpp>

#include "fwGuiQt/config.hpp"



namespace fwGuiQt
{

/**
 * @brief   Defines the Qt application.
 * @class   App
 *
 * @date    2009.
 */

class FWGUIQT_CLASS_API App : public QApplication
{
Q_OBJECT

public:
    FWGUIQT_API App(int & argc, char ** argv, bool GUIenabled);

public Q_SLOTS:
    void aboutToQuit();
    void onExit();

};



} // namespace fwGuiQt


#endif /*__FWGUIQT_APP_HPP__*/
