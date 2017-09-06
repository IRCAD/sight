/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2015-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GUIQT_EDITOR_SSLIDER_HPP__
#define __GUIQT_EDITOR_SSLIDER_HPP__

#include "guiQt/config.hpp"

#include <QPushButton>
#include <QObject>
#include <QSlider>
#include <QLabel>
#include <QLineEdit>
#include <QPointer>

#include <fwTools/Failed.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <gui/editor/IEditor.hpp>

namespace guiQt
{

namespace editor
{


/**
 * @brief   This editor allows to draw a slider with an integer data.
 *
 * Default slider params : value 0, range 0-100, no text, no reset button and edit box.
 * @note value, defaultValue, min and max must be integers.
 *
 * @section XML XML Configuration
 * @code{.xml}
    <service uid="..." type="::gui::editor::IEditor" impl="::guiQt::editor::SSlider" autoConnect="no"/>
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
class GUIQT_CLASS_API SSlider : public QObject,
                                public ::gui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro ( (SSlider)(::gui::editor::IEditor) );

    /// Constructor. Do nothing.
    GUIQT_API SSlider() noexcept;

    /// Destructor. Do nothing.
    GUIQT_API virtual ~SSlider() noexcept;

    /**@name Signals API
     * @{
     */

    GUIQT_API static const ::fwCom::Signals::SignalKeyType s_VALUE_CHANGED_SIG;
    typedef ::fwCom::Signal<void (int)> ValueChangedSignalType;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_SET_VALUE_SLIDER_SLOT;
    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_SET_MIN_VALUE_SLIDER_SLOT;
    GUIQT_API static const ::fwCom::Slots::SlotKeyType s_SET_MAX_VALUE_SLIDER_SLOT;

    ///@}

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /// Installs the layout
    virtual void starting();

    /// Destroys the layout
    virtual void stopping();

    /// Does nothing
    virtual void updating();

    /// Does nothing
    virtual void swapping();

    /// Configure the service
    virtual void configuring();

    /// Signal when the position os the slider changed
    ValueChangedSignalType::sptr m_sigValueChanged;


protected Q_SLOTS:

    /// SLOT : Called to set the value.
    void setValue( int value, bool _bForced = false );

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

    QPointer<QSlider>       m_valueSlider;          ///< User draggable slider.
    QPointer<QLabel>        m_valueLabel;           ///< The current value. Visible only when there is no edit box.
    QPointer<QLabel>        m_minValueLabel;        ///< Minimum value of the slider displayed on the left side.
    QPointer<QLabel>        m_maxValueLabel;        ///< Maximum value of the slider displayed on the right side.
    QPointer<QLabel>        m_textLabel;            ///< Text displayed on the left.
    QPointer<QPushButton>   m_resetButton;          ///< Button to reset the slider to default value.
    QPointer<QLineEdit>     m_valueEdit;            ///< Edit box, allowing the user to change the slider value precisely.

    int m_value;                                    ///< Current value.
    int m_minValue;                                 ///< Minimum value.
    int m_maxValue;                                 ///< Maximum value.
    int m_defaultValue;                             ///< Default value, used when button reset is pressed.
    QString m_text;                                 ///< Description, displayed via m_textLabel.
    bool m_isUpdatedOnRelease;                      ///< If true, the value actually updated only on slider release.
    bool m_hasResetButton;                          ///< If true, the reset button is available.
    bool m_hasEditBox;                              ///< If true, the edit box is available, and the value label is off.
    bool m_sliderPressed;                           ///< Set to true when the slider is pressed.
};


}
}

#endif /*__GUIQT_EDITOR_SSLIDER_HPP__*/
