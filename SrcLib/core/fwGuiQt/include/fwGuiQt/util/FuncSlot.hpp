/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWGUIQT_UTIL_FUNCSLOT_HPP__
#define __FWGUIQT_UTIL_FUNCSLOT_HPP__

#include <fwGuiQt/config.hpp>

#include <QObject>

#include <functional>

namespace fwGuiQt
{

namespace util
{

class FWGUIQT_CLASS_API FuncSlot : public QObject
{
Q_OBJECT

public:
    FWGUIQT_API FuncSlot();

    template< typename CALLABLE >
    FuncSlot(CALLABLE c) :
        m_func(c)
    {
    }

    //------------------------------------------------------------------------------

    template< typename CALLABLE >
    void setFunction(CALLABLE c)
    {
        m_func = c;
    }

public Q_SLOTS:
    void trigger();

protected:
    std::function< void() > m_func;
};

} // namespace util

} // namespace fwGuiQt

#endif //__FWGUIQT_UTIL_FUNCSLOT_HPP__

