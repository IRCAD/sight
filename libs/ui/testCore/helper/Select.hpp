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

#pragma once

#include <ui/testCore/config.hpp>
#include <ui/testCore/Tester.hpp>

#include <string>
#include <variant>

namespace sight::ui::testCore::helper
{

class Select
{
public:

    /**
     * Implicit constructor from a string. Equivalent to Select::fromMain. @see fromMain
     *
     * @param objectName The objectName of the object to be selected
     *
     * @{
     */
    GUITEST_API Select(const char* objectName);
    GUITEST_API Select(const std::string& objectName);
    /// @}

    /**
     * Create a selector to select an object from the main window.
     *
     * @param objectName The objectName of the object to be selected
     */
    GUITEST_API static Select fromMain(const std::string& objectName);

    /**
     * Create a selector to select an object from the current dialog window.
     *
     * @param objectName The objectName of the object to be selected
     */
    GUITEST_API static Select fromDialog(const std::string& objectName);

    /**
     * Create a selector to select an object via its parent.
     *
     * @param parentName The objectName of the parent of the object to be selected
     * @param childName The objectName of the object to be selected
     */
    GUITEST_API static Select fromParent(const std::string& parentName, const std::string& childName);

    /**
     * Create a selector to select an object via the current graphic component.
     *
     * @param objectName The objectName of the object to be selected
     */
    GUITEST_API static Select fromCurrent(const std::string& objectName);

    /**
     * Create a selector to select the current component graphic component.
     */
    GUITEST_API static Select current();

    /**
     * Create a selector to select the current dialog window.
     */
    GUITEST_API static Select dialog();

    /**
     * Sets the timeout associated with the selection.
     *
     * @param timeout Timeout in milliseconds
     *
     * @returns A copy of this
     */
    GUITEST_API Select withTimeout(int timeout) const;

    /**
     * Adds a new condition for the object to meet
     *
     * @param condition A condition for the object to meet
     *
     * @returns A copy of this
     */
    GUITEST_API Select withCondition(std::function<bool(QObject*)> condition) const;

    /**
     * Selects the desired object.
     *
     * @param tester The current tester
     *
     * @post The current tester's current graphic component is the desired object if it was found.
     */
    GUITEST_API void select(Tester& tester) const;

    /**
     * Returns the description of the object
     *
     * @param tester The current tester
     *
     * @returns The description of the object
     */
    GUITEST_API std::string getDescription(const Tester& tester) const;

private:

    enum class Type
    {
        FROM_MAIN,
        FROM_DIALOG,
        FROM_PARENT,
        FROM_CURRENT,
        CURRENT,
        DIALOG
    };

    using Data = std::variant<std::string, std::pair<std::string, std::string> >;

    Select(Type type, Data data);

    Type m_type;
    Data m_data;
    int m_timeout = Tester::DEFAULT_TIMEOUT;
    std::vector<std::function<bool(QObject*)> > m_conditions;
};

} // namespace sight::ui
