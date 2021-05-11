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

#include <string>

struct ArgV
{
#ifdef WIN32
    std::string m_argvs[1] = {"TestApplication"};
    char* m_argv[2]        = {m_argvs[0].data(), nullptr};
    int m_argc {1};
#else
    std::string m_argvs[3] = {"TestApplication", "-platform", "offscreen"};
    char* m_argv[4]        = {m_argvs[0].data(), m_argvs[1].data(), m_argvs[2].data(), nullptr};
    int m_argc {3};
#endif
};

class TestApplication : private ArgV,
                        public QGuiApplication
{
public:
    TestApplication() :
        ArgV(),
        QGuiApplication(m_argc, m_argv)
    {
    }
};
