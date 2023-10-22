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

#include "Select.hpp"

#include "Dialog.hpp"

#include <utility>

namespace sight::ui::test_core::helper
{

Select::Select(const char* _object_name) :
    Select(Type::FROM_MAIN, _object_name)
{
}

Select::Select(const std::string& _object_name) :
    Select(Type::FROM_MAIN, _object_name)
{
}

//------------------------------------------------------------------------------

Select Select::fromMain(const std::string& _object_name)
{
    return {Type::FROM_MAIN, _object_name};
}

//------------------------------------------------------------------------------

Select Select::fromDialog(const std::string& _object_name)
{
    return {Type::FROM_DIALOG, _object_name};
}

//------------------------------------------------------------------------------

Select Select::fromParent(const std::string& _parent_name, const std::string& _child_name)
{
    return {Type::FROM_PARENT, Data {std::in_place_type<std::pair<std::string, std::string> >, _parent_name, _child_name
            }
    };
}

//------------------------------------------------------------------------------

Select Select::fromCurrent(const std::string& _object_name)
{
    return {Type::FROM_CURRENT, _object_name};
}

//------------------------------------------------------------------------------

Select Select::current()
{
    return {Type::CURRENT, {}};
}

//------------------------------------------------------------------------------

Select Select::dialog()
{
    return {Type::DIALOG, {}};
}

//------------------------------------------------------------------------------

Select Select::withTimeout(int _timeout) const
{
    Select copy = *this;
    copy.m_timeout = _timeout;
    return copy;
}

//------------------------------------------------------------------------------

Select Select::withCondition(std::function<bool(QObject*)> _condition) const
{
    Select copy = *this;
    copy.m_conditions.push_back(_condition);
    return copy;
}

//------------------------------------------------------------------------------

void Select::select(Tester& _tester) const
{
    static const auto verifyConditions = [*this](QObject* _obj)
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
        case Type::FROM_MAIN:
            _tester.take(std::get<std::string>(m_data), std::get<std::string>(m_data), verifyConditions, m_timeout);
            break;

        case Type::FROM_DIALOG:
        {
            std::string child_name = std::get<std::string>(m_data);
            Dialog::take(_tester, "dialog", child_name);
            _tester.yields(std::get<std::string>(m_data), child_name, verifyConditions, m_timeout);
            break;
        }

        case Type::FROM_PARENT:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(m_data);
            _tester.take(parentName, parentName, always_true, m_timeout);
            _tester.yields(childName, childName, verifyConditions, m_timeout);
            break;
        }

        case Type::FROM_CURRENT:
            _tester.yields(std::get<std::string>(m_data), std::get<std::string>(m_data), verifyConditions, m_timeout);
            break;

        case Type::CURRENT:
            break;

        case Type::DIALOG:
            Dialog::take(_tester, "dialog");
    }
}

//------------------------------------------------------------------------------

std::string Select::getDescription(const Tester& _tester) const
{
    switch(m_type)
    {
        case Type::FROM_MAIN:
            return std::get<std::string>(m_data);

        case Type::FROM_DIALOG:
            return std::get<std::string>(m_data) + " (from dialog)";

        case Type::FROM_PARENT:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(m_data);
            return childName + " (child of " + parentName + ")";
        }

        case Type::FROM_CURRENT:
            return std::get<std::string>(m_data) + " (from " + _tester.getDescription() + ")";

        case Type::CURRENT:
            return _tester.getDescription();

        case Type::DIALOG:
            return "dialog";
    }

    return "";
}

Select::Select(Type _type, Data _data) :
    m_type(_type),
    m_data(std::move(_data))
{
}

} // namespace sight::ui::test_core::helper
