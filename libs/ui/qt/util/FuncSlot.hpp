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

#include "ui/qt/config.hpp"

#include <QObject>

#include <functional>

namespace sight::ui::qt
{

namespace util
{

class UI_QT_CLASS_API FuncSlot : public QObject
{
Q_OBJECT

public:

    UI_QT_API FuncSlot();

    template<typename CALLABLE>
    FuncSlot(CALLABLE c) :
        m_func(c)
    {
    }

    //------------------------------------------------------------------------------

    template<typename CALLABLE>
    void setFunction(CALLABLE c)
    {
        m_func = c;
    }

public Q_SLOTS:

    void trigger();

protected:

    std::function<void()> m_func;
};

} // namespace util

} // namespace sight::ui::qt
