/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <QMainWindow>
#include <QPointer>

#include <functional>

namespace sight::guiQt
{

/**
 * @brief A qt panel used to control a VTK 2D Negatoscope view.
 *
 */
class QtMainFrame : public QMainWindow
{

Q_OBJECT

public:
    /**
     * @brief Constructor.
     */
    GUIQT_API QtMainFrame() noexcept;

    /// @brief Destructor.
    GUIQT_API virtual ~QtMainFrame() noexcept;

    typedef std::function<void ()> CloseCallback;
    GUIQT_API void setCloseCallback(CloseCallback fct);

protected:
    void closeEvent(QCloseEvent* event);

private:
    CloseCallback m_fctCloseCallback;
};

} // fwGuiQt
