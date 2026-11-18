/************************************************************************
 *
 * Copyright (C) 2026 IRCAD France
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

#include <QApplication>

#include <array>
#include <cstddef>
#include <cstdint>
#include <string>
#include <utility>

namespace sight::ui::test
{

/// The platform of the QApplication of a test.
enum class qt_platform : std::uint8_t
{
    /// Qt's offscreen platform: the process has a screen of its own, which no other window can cover.
    offscreen,
    /// The platform Qt chooses: QT_QPA_PLATFORM if it is set, else the one of the system.
    system
};

/**
 * @brief The command line of the QApplication of a test.
 *
 * QApplication keeps references to its argc and argv: they must outlive it, as a member declared before it does.
 *
 * @code{.cpp}
 * sight::ui::test::qt_arguments arguments {"worker_qt_test"};
 * auto worker = sight::ui::qt::get_qt_worker(arguments.argc(), arguments.argv(), make_application, "", "");
 * @endcode
 */
class qt_arguments final
{
public:

    /**
     * @brief Builds the command line of an application.
     *
     * @param _name the name of the application.
     * @param _platform its platform.
     */
    explicit qt_arguments(std::string _name, qt_platform _platform = qt_platform::offscreen) :
        m_name(std::move(_name)),
        m_argc(_platform == qt_platform::offscreen ? 3 : 1)
    {
        m_argv.at(static_cast<std::size_t>(m_argc)) = nullptr;
    }

    // argv points into the strings of this object: a copy would point into the original.
    qt_arguments(const qt_arguments&)            = delete;
    qt_arguments& operator=(const qt_arguments&) = delete;

    /// The argument count, which QApplication may change.
    [[nodiscard]] int& argc()
    {
        return m_argc;
    }

    /// The arguments, ending with a null pointer, which QApplication may change.
    [[nodiscard]] char** argv()
    {
        return m_argv.data();
    }

private:

    std::string m_name;
    std::string m_platform_option {"-platform"};
    std::string m_platform {"offscreen"};
    int m_argc;
    std::array<char*, 4> m_argv {m_name.data(), m_platform_option.data(), m_platform.data(), nullptr};
};

/**
 * @brief The QApplication of a test, with its command line.
 *
 * @code{.cpp}
 * TEST_CASE("shows_the_value")
 * {
 *     sight::ui::test::qt_application application {"ui_qt_test"};
 *     QLabel label("1");
 *     label.show();
 * }
 * @endcode
 */
class qt_application final
{
public:

    /**
     * @brief Creates the application.
     *
     * @param _name the name of the application.
     * @param _platform its platform.
     */
    explicit qt_application(std::string _name, qt_platform _platform = qt_platform::offscreen) :
        m_arguments(std::move(_name), _platform),
        m_application(m_arguments.argc(), m_arguments.argv())
    {
    }

    /// The application itself.
    [[nodiscard]] QApplication& get()
    {
        return m_application;
    }

private:

    qt_arguments m_arguments;
    QApplication m_application;
};

} // namespace sight::ui::test
