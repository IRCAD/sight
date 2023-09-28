/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include "ui/qt/config.hpp"

#include <core/base.hpp>

#include <QCoreApplication>
#include <QSharedPointer>

#include <functional>

namespace sight::core::thread
{

class worker;

} // namespace sight::core::thread

namespace sight::ui::qt
{

//-----------------------------------------------------------------------------

UI_QT_API SPTR(core::thread::worker) getQtWorker(
    int& argc,
    char** argv,
    std::function<QSharedPointer<QCoreApplication>(int&, char**)> callback,
    const std::string& name,
    const std::string& version
);

//-----------------------------------------------------------------------------

} //namespace sight::ui::qt
