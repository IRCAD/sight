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

namespace sight::ui::testCore::helper
{

Select::Select(const char* objectName) :
    Select(Type::FROM_MAIN, objectName)
{
}

Select::Select(const std::string& objectName) :
    Select(Type::FROM_MAIN, objectName)
{
}

//------------------------------------------------------------------------------

Select Select::fromMain(const std::string& objectName)
{
    return {Type::FROM_MAIN, objectName};
}

//------------------------------------------------------------------------------

Select Select::fromDialog(const std::string& objectName)
{
    return {Type::FROM_DIALOG, objectName};
}

//------------------------------------------------------------------------------

Select Select::fromParent(const std::string& parentName, const std::string& childName)
{
    return {Type::FROM_PARENT, Data {std::in_place_type<std::pair<std::string, std::string> >, parentName, childName}};
}

//------------------------------------------------------------------------------

Select Select::fromCurrent(const std::string& objectName)
{
    return {Type::FROM_CURRENT, objectName};
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

Select Select::withTimeout(int timeout) const
{
    Select copy = *this;
    copy.m_timeout = timeout;
    return copy;
}

//------------------------------------------------------------------------------

Select Select::withCondition(std::function<bool(QObject*)> condition) const
{
    Select copy = *this;
    copy.m_conditions.push_back(condition);
    return copy;
}

//------------------------------------------------------------------------------

void Select::select(Tester& tester) const
{
    static const auto verifyConditions = [*this](QObject* obj)
                                         {
                                             return std::ranges::all_of(
                                                 m_conditions,
                                                 [obj](std::function<bool(QObject*)> condition)
            {
                return condition(obj);
            });
                                         };
    switch(m_type)
    {
        case Type::FROM_MAIN:
            tester.take(std::get<std::string>(m_data), std::get<std::string>(m_data), verifyConditions, m_timeout);
            break;

        case Type::FROM_DIALOG:
        {
            std::string childName = std::get<std::string>(m_data);
            Dialog::take(tester, "dialog", childName);
            tester.yields(std::get<std::string>(m_data), childName, verifyConditions, m_timeout);
            break;
        }

        case Type::FROM_PARENT:
        {
            auto [parentName, childName] = std::get<std::pair<std::string, std::string> >(m_data);
            tester.take(parentName, parentName, alwaysTrue, m_timeout);
            tester.yields(childName, childName, verifyConditions, m_timeout);
            break;
        }

        case Type::FROM_CURRENT:
            tester.yields(std::get<std::string>(m_data), std::get<std::string>(m_data), verifyConditions, m_timeout);
            break;

        case Type::CURRENT:
            break;

        case Type::DIALOG:
            Dialog::take(tester, "dialog");
    }
}

//------------------------------------------------------------------------------

std::string Select::getDescription(const Tester& tester) const
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
            return std::get<std::string>(m_data) + " (from " + tester.getDescription() + ")";

        case Type::CURRENT:
            return tester.getDescription();

        case Type::DIALOG:
            return "dialog";
    }

    return "";
}

Select::Select(Type type, Data data) :
    m_type(type),
    m_data(std::move(data))
{
}

} // namespace sight::ui::testCore::helper
