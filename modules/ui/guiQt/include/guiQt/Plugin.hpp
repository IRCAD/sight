/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "guiQt/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace thread
{
class Worker;
} //namespace thread

namespace guiQt
{
/**
 * @brief   This class is called when the guiQt module is loaded.
 */
class GUIQT_CLASS_API Plugin : public core::runtime::Plugin
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

    SPTR(core::thread::Worker) m_workerQt;

};

} // namespace guiQt
