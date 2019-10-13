/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#ifndef __FWGUIQT_APP_HPP__
#define __FWGUIQT_APP_HPP__

#include "fwGuiQt/config.hpp"

#include <fwRuntime/profile/Profile.hpp>

#include <QApplication>

namespace fwGuiQt
{

/**
 * @brief   Defines the Qt application.
 * @class   App
 */
class FWGUIQT_CLASS_API App : public QApplication
{
Q_OBJECT

public:
    FWGUIQT_API App(int& argc, char** argv, bool guiEnabled);

public Q_SLOTS:
    void aboutToQuit();
    void onExit();

};

} // namespace fwGuiQt


#endif /*__FWGUIQT_APP_HPP__*/
