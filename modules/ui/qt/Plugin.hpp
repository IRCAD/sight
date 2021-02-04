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

#include "modules/ui/qt/config.hpp"

#include <core/runtime/Plugin.hpp>

namespace sight::core::thread
{
class Worker;
} //namespace thread

namespace sight::modules::ui::qt
{
/**
 * @brief   This class is called when the guiQt module is loaded.
 */
class MODULE_UI_QT_CLASS_API Plugin : public core::runtime::Plugin
{

public:

    /**
     * @brief   destructor
     */
    MODULE_UI_QT_API ~Plugin() noexcept;

    // Overrides
    MODULE_UI_QT_API void start();

    // Overrides
    MODULE_UI_QT_API void stop() noexcept;

    MODULE_UI_QT_API int run() noexcept;

protected:

    MODULE_UI_QT_API void loadStyleSheet();

private:

    SPTR(core::thread::Worker) m_workerQt;

};

} // namespace sight::modules::ui::qt
