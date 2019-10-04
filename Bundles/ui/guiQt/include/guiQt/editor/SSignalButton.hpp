/************************************************************************
 *
 * Copyright (C) 2015-2019 IRCAD France
 * Copyright (C) 2015-2019 IHU Strasbourg
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

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <filesystem>
#include <QPointer>
#include <QPushButton>

namespace guiQt
{

namespace editor
{

/**
 * @brief   This editor shows a button and send a signal when it is clicked.
 *
 * If the mode is "checkable", it sends a signal with a boolean in parameter representing the "check" state.
 *
 * @note This service doesn't modify its associated object, so it can work on any type of object.
 *
 * @section XML XML configuration
 * @code{.xml}
   <service uid="..." impl="::guiQt::editor::SSignalButton" >
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
 * - \b text (optional): text displayed on the button
 * - \b icon (optional): icon displayed on the button
 * - \b checkable (optional, default: false): if true, the button is checkable
 * - \b text2 (optional): text displayed if the button is checked
 * - \b icon2 (optional): icon displayed if the button is checked
 * - \b checked (optional, default: false): if true, the button is checked at start
 * - \b iconWidth (optional): icon width
 * - \b iconHeight (optional): icon height
 *
 * @section Signals Signals
 * - \b clicked(): This signal is emitted when the button is clicked
 * - \b toggled(bool):  This signal is emitted when the button is checked
 *
 * @section Slot Slot
 * - \b setChecked(bool): This slot allows to check/uncheck the button
 */
class GUIQT_CLASS_API SSignalButton : public QObject,
                                      public ::fwGui::editor::IEditor
{
Q_OBJECT
public:

    fwCoreServiceMacro(SSignalButton, ::fwGui::editor::IEditor)

    /// Constructor. Do nothing.
    GUIQT_API SSignalButton() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SSignalButton() noexcept;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */
    /**
     * @brief This method launches the IEditor::starting method.
     */
    GUIQT_API virtual void starting() override;

    /**
     * @brief This method launches the IEditor::stopping method.
     */
    GUIQT_API virtual void stopping() override;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUIQT_API virtual void updating() override;

    /// This method is used to configure the class parameters.
    GUIQT_API virtual void configuring() override;

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    GUIQT_API virtual void info(std::ostream& _sstream ) override;

    ///@}

private Q_SLOTS:
    void onClicked();
    void onToggled(bool);

private:

    /**
     * @name Signal
     * @{
     */
    /// Signal emitted when button is clicked
    typedef ::fwCom::Signal< void () > ClickedSignalType;
    ClickedSignalType::sptr m_sigClicked;

    /// Signal emitted when button is checked/unchecked
    typedef ::fwCom::Signal< void (bool) > ToggledSignalType;
    ToggledSignalType::sptr m_sigToggled;
    /**
     * @}
     */

    /**
     * @name Slots
     * @{
     */
    /// Slot: check/uncheck the button
    void setChecked(bool checked);
    /**
     * @}
     */

    QPointer <QPushButton> m_button; /// button

    std::string m_text; /// Button's text
    std::string m_text2; /// Button's text when it is checked
    std::filesystem::path m_icon; /// Path of the button's icon
    std::filesystem::path m_icon2; /// Path of the button's icon displayed when it is checked
    bool m_checkable; /// If true, the button is checkable
    bool m_checkAtStart; /// If true, the button is checked at start
    unsigned int m_iconWidth; /// icon width
    unsigned int m_iconHeight; /// icon height
};

}
}
