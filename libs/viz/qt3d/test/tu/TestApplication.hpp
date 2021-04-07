/************************************************************************
 *
 * Copyright (C) 2021 IRCAD France
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

#pragma once

#include <QGuiApplication>

class TestApplication : QGuiApplication
{
public:
    TestApplication() :
        QGuiApplication(s_argc, s_argv)
    {
    }

private:
#if defined(__linux)
    static inline char* s_argv[] = {"TestApplication", "-platform", "offscreen", nullptr};
    static inline int s_argc = 3;
#else
    static inline char* s_argv[] = {"TestApplication", 0};
    static inline int s_argc = 1;
#endif
};
