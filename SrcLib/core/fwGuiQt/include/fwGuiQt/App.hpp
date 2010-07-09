/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef FWGUIQT_APP_HPP_
#define FWGUIQT_APP_HPP_

#include <QApplication>

#include <fwRuntime/profile/Profile.hpp>

#include "fwGuiQt/config.hpp"



namespace fwGuiQt
{

/**
 * @brief   Defines the wx application.
 * @class   App.
 * @author  IRCAD (Research and Development Team).
 * @date    2009.
 */

class FWGUIQT_CLASS_API App : public QApplication
{
    Q_OBJECT

public:
    FWGUIQT_API App(int & argc, char ** argv);

public slots :
  void onExit();
  void onLastWindowClosed();

};



} // namespace fwGuiQt


#endif /*FWGUIQT_APP_HPP_*/
