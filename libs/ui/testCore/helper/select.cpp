/************************************************************************
 *
 * Copyright (C) 2023 IRCAD France
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

#include "select.hpp"

#include "dialog.hpp"

#include <utility>

namespace sight::ui::test_core::helper
{

selector::selector(const char* _object_name) :
    selector(type::from_main, _object_name)
{
}

selector::selector(const std::string& _object_name) :
    selector(type::from_main, _object_name)
{
}

//------------------------------------------------------------------------------

selector selector::from_main(const std::string& _object_name)
{
    return {type::from_main, _object_name};
}

//------------------------------------------------------------------------------

selector selector::from_dialog(const std::string& _object_name)
{
    return {type::from_dialog, _object_name};
}

//------------------------------------------------------------------------------

selector selector::from_parent(const std::string& _parent_name, const std::string& _child_name)
{
    return {type::from_parent, Data {std::in_place_type<std::pair<std::string, std::string> >, _parent_name, _child_name
            }
    };
}

//------------------------------------------------------------------------------

selector selector::from_current(const std::string& _object_name)
{
    return {type::from_current, _object_name};
}

//------------------------------------------------------------------------------

selector selector::current()
{
    return {type::current, {}};
}

//------------------------------------------------------------------------------

selector selector::dialog()
{
    return {type::dialog, {}};
}

//------------------------------------------------------------------------------

selector selector::with_timeout(int _timeout) const
{
    selector copy = *this;
    copy.m_timeout = _timeout;
    return copy;
}

//------------------------------------------------------------------------------

selector selector::with_condition(std::function<bool(QObject*)> _condition) const
{
    selector copy = *this;
    copy.m_conditions.push_back(_condition);
    return copy;
}

//------------------------------------------------------------------------------

void selector::select(tester& _tester) const
{
    static const auto s_VERIFY_CONDITIONS = [*this](QObject* _obj)
                                            {
                                                return std::ranges::all_of(
                                                    m_conditions,
                                                    [_obj](std::function<bool(QObject*)> _condition)
            {
                return _condition(_obj);
            });
                                            };
    switch(m_type)
    {
        case type::from_main:
            _tester.take(std::get<std::string>(m_data), std::get<std::string>(m_data), s_VERIFY_CONDITIONS, m_timeout);
            break;

        case type::from_dialog:
        {
            std::string child_name = std::get<std::string>(m_data);
            dialog::take(_tester, "dialog", child_name);
            _tester.yields(std::get<std::string>(m_data), child_name, s_VERIFY_CONDITIONS, m_timeout);
            break;
        }

        case type::from_parent:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(m_data);
            _tester.take(parentName, parentName, always_true, m_timeout);
            _tester.yields(childName, childName, s_VERIFY_CONDITIONS, m_timeout);
            break;
        }

        case type::from_current:
            _tester.yields(
                std::get<std::string>(m_data),
                std::get<std::string>(m_data),
                s_VERIFY_CONDITIONS,
                m_timeout
            );
            break;

        case type::current:
            break;

        case type::dialog:
            dialog::take(_tester, "dialog");
    }
}

//------------------------------------------------------------------------------

std::string selector::get_description(const tester& _tester) const
{
    switch(m_type)
    {
        case type::from_main:
            return std::get<std::string>(m_data);

        case type::from_dialog:
            return std::get<std::string>(m_data) + " (from dialog)";

        case type::from_parent:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(m_data);
            return childName + " (child of " + parentName + ")";
        }

        case type::from_current:
            return std::get<std::string>(m_data) + " (from " + _tester.get_description() + ")";

        case type::current:
            return _tester.get_description();

        case type::dialog:
            return "dialog";
    }

    return "";
}

selector::selector(enum type _type, Data _data) :
    m_type(_type),
    m_data(std::move(_data))
{
}

} // namespace sight::ui::test_core::helper
