/************************************************************************
 *
 * Copyright (C) 2023-2024 IRCAD France
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

#include <sight/ui/test/config.hpp>

#include <ui/test/tester.hpp>

#include <string>
#include <variant>

namespace sight::ui::test::helper
{

class selector
{
public:

    enum class type
    {
        from_main,
        from_dialog,
        from_parent,
        from_current,
        current,
        dialog
    };

    using Data = std::variant<std::string, std::pair<std::string, std::string> >;

    /**
     * Implicit constructor from a string. Equivalent to selector::fromMain. @see fromMain
     *
     * @param _object_name The objectName of the object to be selected
     *
     * @{
     */
    SIGHT_UI_TEST_API selector(const char* _object_name);
    SIGHT_UI_TEST_API selector(const std::string& _object_name);
    /// @}

    /**
     * Create a selector to select an object from the main window.
     *
     * @param _object_name The objectName of the object to be selected
     */
    SIGHT_UI_TEST_API static selector from_main(const std::string& _object_name);

    /**
     * Create a selector to select an object from the current dialog window.
     *
     * @param _object_name The objectName of the object to be selected
     */
    SIGHT_UI_TEST_API static selector from_dialog(const std::string& _object_name);

    /**
     * Create a selector to select an object via its parent.
     *
     * @param _parent_name The objectName of the parent of the object to be selected
     * @param _child_name The objectName of the object to be selected
     */
    SIGHT_UI_TEST_API static selector from_parent(const std::string& _parent_name, const std::string& _child_name);

    /**
     * Create a selector to select an object via the current graphic component.
     *
     * @param _object_name The objectName of the object to be selected
     */
    SIGHT_UI_TEST_API static selector from_current(const std::string& _object_name);

    /**
     * Create a selector to select the current component graphic component.
     */
    SIGHT_UI_TEST_API static selector current();

    /**
     * Create a selector to select the current dialog window.
     */
    SIGHT_UI_TEST_API static selector dialog();

    /**
     * Sets the timeout associated with the selection.
     *
     * @param _timeout Timeout in milliseconds
     *
     * @returns A copy of this
     */
    SIGHT_UI_TEST_API selector with_timeout(int _timeout) const;

    /**
     * Adds a new condition for the object to meet
     *
     * @param _condition A condition for the object to meet
     *
     * @returns A copy of this
     */
    SIGHT_UI_TEST_API selector with_condition(std::function<bool(QObject*)> _condition) const;

    /**
     * selects the desired object.
     *
     * @param _tester The current tester
     *
     * @post The current tester's current graphic component is the desired object if it was found.
     */
    SIGHT_UI_TEST_API void select(tester& _tester) const;

    /**
     * Returns the description of the object
     *
     * @param _tester The current tester
     *
     * @returns The description of the object
     */
    SIGHT_UI_TEST_API std::string get_description(const tester& _tester) const;

    //------------------------------------------------------------------------------

    SIGHT_UI_TEST_API enum type type() const
    {
        return m_type;
    }

    //------------------------------------------------------------------------------

    SIGHT_UI_TEST_API Data data() const
    {
        return m_data;
    }

private:

    selector(enum type _type, Data _data);

    enum type m_type;
    Data m_data;
    int m_timeout = tester::DEFAULT_TIMEOUT;
    std::vector<std::function<bool(QObject*)> > m_conditions;
};

} // namespace sight::ui
