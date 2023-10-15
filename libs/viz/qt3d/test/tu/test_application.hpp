/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

#include <array>
#include <string>

struct ArgV
{
#ifdef WIN32
    std::array<std::string, 1> m_argvs = {"test_application"};
    std::array<char*, 2> m_argv        = {m_argvs[0].data(), nullptr};
#else
    std::array<std::string, 3> m_argvs = {"test_application", "-platform", "offscreen"};
    std::array<char*, 4> m_argv        = {m_argvs[0].data(), m_argvs[1].data(), m_argvs[2].data(), nullptr};
#endif
    int m_argc {int(m_argvs.size())};
};

class test_application : private ArgV,
                         public QGuiApplication
{
public:

    test_application() :
        ArgV(),
        QGuiApplication(m_argc, m_argv.data())
    {
    }
};
