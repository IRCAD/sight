/************************************************************************
 *
 * Copyright (C) 2015-2021 IRCAD France
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

#include "modules/ui/qt/config.hpp"

#include <core/com/Slot.hpp>
#include <core/com/Slots.hpp>
#include <core/tools/Failed.hpp>

#include <ui/base/IEditor.hpp>

#include <QLabel>
#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>

namespace sight::module::ui::qt
{

/**
 * @brief   This editor allows to draw a slider with an integer data.
 *
 * Default slider params : value 0, range 0-100, no text, no reset button and edit box.
 * @note value, defaultValue, min and max must be integers.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::SSlider" autoConnect="false"/>
        <editBox>false</editBox>
        <resetButton>false</resetButton>
        <defaultValue>200</defaultValue>
        <updateOnRelease>false</updateOnRelease>
        <value>200</value>
        <text>opacity value</text>
        <range>
            <min>1</min>
            <max>600</max>
        </range>
    </service>
   @endcode
 *
 * @section Signals Signals
 * - \b valueChanged(int): This editor emits the signal "valueChanged" with the changed slider value.
 *
 * @section Slots Slots
 * - \b setValue(int, bool): This slot allows to update the slider value.
 * - \b setMinValue(int): This slot allows to set minimum value.
 * - \b setMaxValue(int): This slot allows to set maximum value.
 */
class MODULE_UI_QT_CLASS_API SSlider : public QObject,
                                       public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SSlider, sight::ui::base::IEditor);

    /// Constructor. Do nothing.
    MODULE_UI_QT_API SSlider() noexcept;

    /// Destructor. Do nothing.
    MODULE_UI_QT_API virtual ~SSlider() noexcept;

    /**@name Signals API
     * @{
     */

    MODULE_UI_QT_API static const core::com::Signals::SignalKeyType s_VALUE_CHANGED_SIG;
    typedef core::com::Signal<void (int)> ValueChangedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_SET_VALUE_SLIDER_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_SET_MIN_VALUE_SLIDER_SLOT;
    MODULE_UI_QT_API static const core::com::Slots::SlotKeyType s_SET_MAX_VALUE_SLIDER_SLOT;

    ///@}

protected:

    typedef core::runtime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    void starting() override;

    /// Destroys the layout
    void stopping() override;

    /// Does nothing
    void updating() override;

    /// Configure the service
    void configuring() override;

    /// Signal when the position os the slider changed
    ValueChangedSignalType::sptr m_sigValueChanged;

protected Q_SLOTS:

    /// SLOT : Called to set the value.
    void setValue(int value, bool _bForced = false);

    /// SLOT : Called to set the min range
    void setMinValue(int value);

    /// SLOT : Called to set the max range
    void setMaxValue(int value);

    /// Internal slot. Called when the cursor starts to move.
    void sliderPressed();

    /// Internal slot. Reset the value - and the slider position - to default
    void resetValue();

    /// Internal slot. Called when the cursor is moved.
    void changeValue();

    /// Internal slot. Called when Return is pressed on the edit box
    void editValue();

private:

    QPointer<QSlider> m_valueSlider;     ///< User draggable slider.
    QPointer<QLabel> m_valueLabel;       ///< The current value. Visible only when there is no edit box.
    QPointer<QLabel> m_minValueLabel;    ///< Minimum value of the slider displayed on the left side.
    QPointer<QLabel> m_maxValueLabel;    ///< Maximum value of the slider displayed on the right side.
    QPointer<QLabel> m_textLabel;        ///< Text displayed on the left.
    QPointer<QPushButton> m_resetButton; ///< Button to reset the slider to default value.
    QPointer<QLineEdit> m_valueEdit;     ///< Edit box, allowing the user to change the slider value
    // precisely.

    int m_value;               ///< Current value.
    int m_minValue;            ///< Minimum value.
    int m_maxValue;            ///< Maximum value.
    int m_defaultValue;        ///< Default value, used when button reset is pressed.
    QString m_text;            ///< Description, displayed via m_textLabel.
    bool m_isUpdatedOnRelease; ///< If true, the value actually updated only on slider release.
    bool m_hasResetButton;     ///< If true, the reset button is available.
    bool m_hasEditBox;         ///< If true, the edit box is available, and the value label is off.
    bool m_sliderPressed;      ///< Set to true when the slider is pressed.
};

} // sight::module::ui::qt
