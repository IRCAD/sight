/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
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

#include "guiQt/config.hpp"

#include <core/tools/Failed.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <QPointer>
#include <QPushButton>

#include <filesystem>

namespace guiQt
{

namespace editor
{

/**
 * @brief This editor shows a button and send a signal when it is clicked.
 *
 * If the mode is "checkable", it sends a signal with a boolean in parameter representing the "check" state.
 *
 * @section Signals Signals
 * - \b clicked(): This signal is emitted when the button is clicked.
 * - \b toggled(bool):  This signal is emitted when the button is checked.
 *
 * @section Slot Slot
 * - \b setChecked(bool): allows to check/uncheck the button.
 * - \b check(): allows to check the button.
 * - \b uncheck(): allows to uncheck the button.
 * - \b setIsExecutable(bool): sets the button executability.
 * - \b setExecutable(): sets the button executable.
 * - \b setInexecutable(): sets the button inexecutable.
 * - \b setVisible(bool):s ets the button visibility.
 * - \b show(): shows the button.
 * - \b hide(): hides the button.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." type="::guiQt::editor::SSignalButton" >
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
 * - \b executable (optional, bool, default=true): if true, the button is executable.
 * - \b text2 (optional, string, default=""): text displayed if the button is checked.
 * - \b icon2 (optional, string, default=""): icon displayed if the button is checked.
 * - \b checked (optional, bool, default=false): if true, the button is checked at start.
 * - \b iconWidth (optional, unsigned, default=0): icon width.
 * - \b iconHeight (optional, unsigned, default=0): icon height.
 */
class GUIQT_CLASS_API SSignalButton : public QObject,
                                      public ::fwGui::editor::IEditor
{

Q_OBJECT

public:

    /// Generates default methods as New, dynamicCast, ...
    fwCoreServiceMacro(SSignalButton, ::fwGui::editor::IEditor)

    /// Creates signals and slots.
    GUIQT_API SSignalButton() noexcept;

    /// Destroys the service.
    GUIQT_API virtual ~SSignalButton() noexcept;

protected:

    /// Configures the class parameters.
    GUIQT_API void configuring() override;

    /// Launches the IEditor::starting method.
    GUIQT_API void starting() override;

    ///Does nothing.
    GUIQT_API void updating() override;

    /// Launches the IEditor::stopping method.
    GUIQT_API void stopping() override;

private Q_SLOTS:

    void onClicked();

    void onToggled(bool);

private:

    /// SLOT: checks or unchecks the button.
    void setChecked(bool checked);

    /// SLOT: checks the button.
    void check();

    /// SLOT: unchecks the button.
    void uncheck();

    /// SLOT: sets the button executability.
    void setIsExecutable(bool _isExecutable);

    /// SLOT: sets the button executable.
    void setExecutable();

    /// SLOT: sets the button inexecutable.
    void setInexecutable();

    /// SLOT: sets the button visibility.
    void setVisible(bool _isVisible);

    /// SLOT: shows the button.
    void show();

    /// SLOT: hides he button.
    void hide();

    typedef core::com::Signal< void () > ClickedSignalType;
    /// Contains the signal emitted when button is clicked.
    ClickedSignalType::sptr m_sigClicked { nullptr };

    typedef core::com::Signal< void (bool) > ToggledSignalType;
    /// Contains the signal emitted when button is checked/unchecked
    ToggledSignalType::sptr m_sigToggled { nullptr };

    /// Contains the button
    QPointer <QPushButton> m_button { nullptr };

    /// Defines the button's text.
    std::string m_text { "" };

    /// Defines the button's text when it is checked.
    std::string m_text2 { "" };

    /// Defines the path of the button's icon.
    std::filesystem::path m_icon;

    /// Defines the path of the button's icon when it is checked.
    std::filesystem::path m_icon2;

    /// Defines if the button is checkable.
    bool m_checkable { false };

    /// Defines if the button is executable.
    bool m_executable { true };

    /// Defines if the button is checked at start.
    bool m_checkAtStart { false };

    /// Defines the icon width.
    unsigned m_iconWidth { 0 };

    /// Defines the icon height.
    unsigned m_iconHeight { 0 };

    /// Defines the button tooltip.
    std::string m_toolTip { "" };

};

} // namespace editor.
} // namespace gui.
