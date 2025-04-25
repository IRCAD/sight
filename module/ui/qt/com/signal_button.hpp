/************************************************************************
 *
 * Copyright (C) 2015-2025 IRCAD France
 * Copyright (C) 2015-2020 IHU Strasbourg
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

#include <core/tools/failed.hpp>

#include <QPointer>
#include <QPushButton>

#include <ui/__/editor.hpp>

#include <filesystem>

namespace sight::module::ui::qt::com
{

/**
 * @brief This editor shows a button and sends a signal when it is clicked.
 *
 * If the mode is "checkable", it sends a signal with a boolean in parameter representing the "check" state.
 *
 * @section Signals Signals
 * - \b clicked(): This signal is emitted when the button is clicked.
 * - \b toggled(bool):  This signal is emitted when the button is checked.
 *
 * @section Slot Slot
 * - \b set_checked(bool): allows to check/uncheck the button.
 * - \b check(): allows to check the button.
 * - \b uncheck(): allows to uncheck the button.
 * - \b set_enabled(bool): sets the button executability.
 * - \b enable(): sets the button executable.
 * - \b disable(): sets the button inexecutable.
 * - \b set_visible(bool):s ets the button visibility.
 * - \b show(): shows the button.
 * - \b hide(): hides the button.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="sight::module::ui::qt::com::signal_button" >
       <config>
          <checkable>true|false</checkable>
          <text>...</text>
          <icon>...</icon>
          <text2>...</text2>
          <icon2>...</icon2>
          <checked>true|false</checked>
          <iconWidth>...</iconWidth>
          <iconHeight>...</iconHeight>
       </config>
   </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * - \b text (optional, string, default=""): text displayed on the button.
 * - \b icon (optional, string, default=""): icon displayed on the button.
 * - \b checkable (optional, bool, default=false): if true, the button is checkable.
 * - \b enable (optional, bool, default=true): if true, the button is executable.
 * - \b text2 (optional, string, default=""): text displayed if the button is checked.
 * - \b icon2 (optional, string, default=""): icon displayed if the button is checked.
 * - \b checked (optional, bool, default=false): if true, the button is checked at start.
 * - \b iconWidth (optional, unsigned, default=0): icon width.
 * - \b iconHeight (optional, unsigned, default=0): icon height.
 */
class signal_button : public QObject,
                      public sight::ui::editor
{
Q_OBJECT

public:

    struct signals
    {
        using bool_t = core::com::signal<void (bool)>;
        using void_t = core::com::signal<void ()>;

        static inline const core::com::signals::key_t IS_CHECKED = "is_checked";
        /// Same as "is_checked" but kept for legacy reasons.
        static inline const core::com::signals::key_t TOGGLED   = "toggled";
        static inline const core::com::signals::key_t CLICKED   = "clicked";
        static inline const core::com::signals::key_t CHECKED   = "checked";
        static inline const core::com::signals::key_t UNCHECKED = "unchecked";
    };

    /// Generates default methods as New, dynamicCast, ...
    SIGHT_DECLARE_SERVICE(signal_button, sight::ui::editor);

    /// Creates signals and slots.
    signal_button() noexcept;

    /// Destroys the service.
    ~signal_button() noexcept override;

protected:

    /// Configures the class parameters.
    void configuring() override;

    /// Launches the editor::starting method.
    void starting() override;

    ///Does nothing.
    void updating() override;

    /// Launches the editor::stopping method.
    void stopping() override;

private Q_SLOTS:

    void on_clicked();

    void on_toggled(bool /*toggled*/);

private:

    /// SLOT: checks or unchecks the button.
    void set_checked(bool _checked);

    /// SLOT: checks the button.
    void check();

    /// SLOT: unchecks the button.
    void uncheck();

    /// SLOT: sets the button executability.
    void set_enabled(bool _is_enabled) override;

    /// SLOT: sets the button executable.
    void enable() override;

    /// SLOT: sets the button inexecutable.
    void disable() override;

    /// SLOT: sets the button visibility.
    void set_visible(bool _is_visible) override;

    /// SLOT: shows the button.
    void show() override;

    /// SLOT: hides he button.
    void hide() override;

    /// Contains the button
    QPointer<QPushButton> m_button {nullptr};

    /// Defines the button's text.
    std::string m_text;

    /// Defines the button's text when it is checked.
    std::string m_text2;

    /// Defines the path of the button's icon.
    std::filesystem::path m_icon;

    /// Defines the path of the button's icon when it is checked.
    std::filesystem::path m_icon2;

    /// Defines if the button is checkable.
    bool m_checkable {false};

    /// Defines if the button is executable.
    bool m_enable {true};

    /// Defines if the button is checked at start.
    bool m_check_at_start {false};

    /// Defines the icon width.
    unsigned m_icon_width {0};

    /// Defines the icon height.
    unsigned m_icon_height {0};

    /// Defines the button tooltip.
    std::string m_tool_tip;
};

} // namespace sight::module::ui::qt::com
