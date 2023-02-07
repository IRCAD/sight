/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include <ui/base/IEditor.hpp>
#include <ui/base/parameter.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>

namespace sight::module::ui::qt
{

/**
 * @brief Generic editor to interact with parameters.
 *
 * It allows to setup an editor with several parameters.
 * Once the user validates the parameters, a signal is sent containing the key and the value.
 * It supports booleans, doubles or integer at the moment.
 *
 * @note This service doesn't need any data.
 *
 * @section Signals Signals
 * - \b parameterChanged(ui::base::parameter_t, std::string): Emitted when any parameter changes.
 * - \b boolChanged(bool, std::string): Emitted when a boolean parameter changes.
 * - \b colorChanged(std::array<std::uint8_t, 4>, std::string): Emitted when a color parameter changes.
 * - \b doubleChanged(double, std::string): Emitted when a real parameter changes.
 * - \b double2Changed(double, double, std::string): Emitted when two real parameters change.
 * - \b double3Changed(double, double, double, std::string): Emitted when three real parameters change.
 * - \b intChanged(int, std::string): Emitted when an integer parameter changes.
 * - \b int2Changed(int, int, std::string): Emitted when two integer parameters change.
 * - \b int3Changed(int, int, int, std::string): Emitted when three integer parameters change.
 * - \b enumChanged(std::string, std::string): Emitted when enum parameter changes, returns the the label of the
 * currently selected item.
 * - \b enumIndexChanged(int, std::string): Emitted when enum parameter changes, returns the index of the currently
 * selected item.
 *
 * @section Slots Slots
 * - \b setParameter(ui::base::parameter_t, std::string): set a parameter.
 * - \b setBoolParameter(bool, std::string): set a boolean parameter.
 * - \b setColorParameter(std::array<std::uint8_t, 4>, std::string): set a color parameter.
 * - \b setDoubleParameter(double, std::string): set a double parameter.
 * - \b setDouble2Parameter(double, double, std::string): set two double parameters.
 * - \b setDouble3Parameter(double, double, double, std::string): set three double parameters.
 * - \b setIntParameter(int, std::string): set an integer parameter.
 * - \b setInt2Parameter(int, int, std::string): set two int parameters.
 * - \b setInt3Parameter(int, int, int, std::string): set three int parameters.
 * - \b setEnumParameter(std::string, std::string): set an enum parameter.
 * - \b setEnumIndexParameter(int, std::string): set an enum parameter using the index of the enum.
 * - \b updateEnumRange(std::string, std::string): update range of an existing enum (value can contains a tokenized list
 * such as value1;value2;value3=test;...)
 * - \b updateIntMinParameter(int, std::string): set the minimum value of an integer parameter (int, int2, int3)
 * - \b updateIntMaxParameter(int, std::string): set the maximum value of an integer parameter (int, int2, int3)
 * - \b updateDoubleMinParameter(double, std::string): set the minimum value of a double parameter (double, double2,
 * double3)
 * - \b updateDoubleMaxParameter(double, std::string): set the maximum value of a double parameter (double, double2,
 * double3)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::qt::SParameters" >
        <parameters scrollable="true">
            <param type="bool" name="boolean parameter" key="boolParam" defaultValue="false" />
            <param type="double" name="real parameter" key="doubleParam" defaultValue="" min="1.5" max="42.42"
                depends="boolParam" />
            <param type="int" name="integer parameter" key="intParam" defaultValue="1" min="0" max="255"
                depends="boolParam" dependsReverse="true" />
            <param type="enum" name="enum parameters" key="enumParam" defaultValue="p1" values="p1,p2,p3" />
            <param type="int" name="integer parameter" key="intParam2" defaultValue="1" min="0" max="255"
                depends="enumParam" dependsValue="p2" />
        </parameters>
       </service>
   @endcode
 *
 * @subsection Configuration Configuration:
 * <parameters> tag:
 * - \b scrollable: If true, add a scroll bar if the content doesn't fit on the screen. If false, flatten the content
 * <param> tag:
 * - \b type: bool, color, enum, double, double2, double3, int, int2, int3.
 * - \b name: label to display.
 * - \b key: name used in the signal to identify the parameter.
 * - \b defaultValue: value used to initialize the parameter.
 * - \b min: minimum value, if relevant for the data type.
 * - \b max: maximum value, if relevant for the data type.
 * - \b widget (optional) : widget type, available for types 'int', 'double' and 'enum'.
 * For 'int' and 'double', you can choose between a 'spin' or a 'slider' widget. Defaults to 'spin' for 'double' and
 * 'slider' for 'int'.
 * For 'enum', you can choose between 'combobox' (the default) and 'slider' (only available with integer values).
 * - \b decimals (optional, default=2): number of decimals settable using a double slider.
 * - \b reset (optional, default=true): display the reset button.
 * - \b values: list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for enum type).
 * The actual displayed value and the returned one in the signal can be different using '=' to separate the two. For
 * example 'values="BLEND=imageBlend,CHECKERBOARD=imageCheckerboard"' means the combo will display BLEND, CHECKBOARD
 * and will send 'imageBlend' or 'imageCheckerboard'.
 * - \b depends (optional, string): key of the dependency.
 * - \b dependsValue (optional, string): value of the dependency in case of enum.
 * - \b dependsReverse (optional, bool, default=false): reverse the dependency status checking.
 * - \b emitOnRelease (optional, default = false): sliders only, if true send value when slider is released,
 * send value when value changed otherwise.
 */
class MODULE_UI_QT_CLASS_API SParameters : public QObject,
                                           public sight::ui::base::IEditor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(SParameters, sight::ui::base::IEditor);

    /// @brief  Struct to handle all signals.
    struct signals
    {
        using signal_t = core::com::Signals::SignalKeyType;
        /// Generic changed signal type
        typedef core::com::Signal<void (sight::ui::base::parameter_t, std::string)> ChangedSignalType;

        /// Boolean changed signal type
        typedef core::com::Signal<void (bool, std::string)> BooleanChangedSignalType;

        /// Color changed signal type
        typedef core::com::Signal<void (std::array<std::uint8_t, 4>, std::string)> ColorChangedSignalType;

        /// Double changed signal type
        typedef core::com::Signal<void (double, std::string)> DoubleChangedSignalType;
        typedef core::com::Signal<void (double, double, std::string)> Double2ChangedSignalType;
        typedef core::com::Signal<void (double, double, double, std::string)> Double3ChangedSignalType;

        /// Integer changed signal type
        typedef core::com::Signal<void (int, std::string)> IntegerChangedSignalType;
        typedef core::com::Signal<void (int, int, std::string)> Integer2ChangedSignalType;
        typedef core::com::Signal<void (int, int, int, std::string)> Integer3ChangedSignalType;

        /// Enum changed signal type
        typedef core::com::Signal<void (std::string, std::string)> EnumChangedSignalType;
        typedef core::com::Signal<void (int, std::string)> EnumChangedIndexSignalType;

        inline static const signal_t PARAMETER_CHANGED_SIG  = "parameterChanged";
        inline static const signal_t BOOLEAN_CHANGED_SIG    = "boolChanged";
        inline static const signal_t COLOR_CHANGED_SIG      = "colorChanged";
        inline static const signal_t DOUBLE_CHANGED_SIG     = "doubleChanged";
        inline static const signal_t DOUBLE2_CHANGED_SIG    = "double2Changed";
        inline static const signal_t DOUBLE3_CHANGED_SIG    = "double3Changed";
        inline static const signal_t INTEGER_CHANGED_SIG    = "intChanged";
        inline static const signal_t INTEGER2_CHANGED_SIG   = "int2Changed";
        inline static const signal_t INTEGER3_CHANGED_SIG   = "int3Changed";
        inline static const signal_t ENUM_CHANGED_SIG       = "enumChanged";
        inline static const signal_t ENUM_INDEX_CHANGED_SIG = "enumIndexChanged";
    };

    /// @brief  Struct to handle all slots
    struct slots
    {
        using slots_t = core::com::Slots::SlotKeyType;

        inline static const slots_t s_SET_PARAMETER_SLOT               = "setParameter";
        inline static const slots_t s_SET_BOOL_PARAMETER_SLOT          = "setBoolParameter";
        inline static const slots_t s_SET_COLOR_PARAMETER_SLOT         = "setColorParameter";
        inline static const slots_t s_SET_DOUBLE_PARAMETER_SLOT        = "setDoubleParameter";
        inline static const slots_t s_SET_DOUBLE2_PARAMETER_SLOT       = "setDouble2Parameter";
        inline static const slots_t s_SET_DOUBLE3_PARAMETER_SLOT       = "setDouble3Parameter";
        inline static const slots_t s_SET_INT_PARAMETER_SLOT           = "setIntParameter";
        inline static const slots_t s_SET_INT2_PARAMETER_SLOT          = "setInt2Parameter";
        inline static const slots_t s_SET_INT3_PARAMETER_SLOT          = "setInt3Parameter";
        inline static const slots_t s_SET_ENUM_PARAMETER_SLOT          = "setEnumParameter";
        inline static const slots_t s_SET_ENUM_INDEX_PARAMETER_SLOT    = "setEnumIndexParameter";
        inline static const slots_t s_UPDATE_ENUM_RANGE_SLOT           = "updateEnumRange";
        inline static const slots_t s_UPDATE_INT_MIN_PARAMETER_SLOT    = "updateIntMinParameter";
        inline static const slots_t s_UPDATE_INT_MAX_PARAMETER_SLOT    = "updateIntMaxParameter";
        inline static const slots_t s_UPDATE_DOUBLE_MIN_PARAMETER_SLOT = "updateDoubleMinParameter";
        inline static const slots_t s_UPDATE_DOUBLE_MAX_PARAMETER_SLOT = "updateDoubleMaxParameter";
    };
    MODULE_UI_QT_API SParameters() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_QT_API ~SParameters() noexcept override;

    /// Configure the editor.
    MODULE_UI_QT_API void configuring() override;

    /// Initializes Qt input widgets for parameters according to xml configuration
    MODULE_UI_QT_API void starting() override;

    /// This method launches the IEditor::stopping method
    MODULE_UI_QT_API void stopping() override;

    /// This method is used to update services. Does nothing
    MODULE_UI_QT_API void updating() override;

private Q_SLOTS:

    /**
     * @brief Called when a dependency widget state (enable or disable) has changed to modify the state of the child
     * widget.
     * @param _checkBox Dependency widget.
     * @param _widget Child widget.
     * @param _reverse Reverse the state check.
     */
    static void onDependsChanged(QCheckBox* _checkBox, QWidget* _widget, bool _reverse);

    /**
     * @brief Called when a dependency widget state (enable or disable) has changed to modify the state of the child
     * widget.
     * @param _comboBox Dependency widget.
     * @param _widget Child widget.
     * @param _value Value of the combo box.
     * @param _reverse Reverse the state check.
     */
    static void onDependsChanged(QComboBox* _comboBox, QWidget* _widget, const std::string& _value, bool _reverse);

    /// This method is called when a boolean value changes
    void onChangeBoolean(int value);

    /// This method is called when a color button is clicked
    void onColorButton();

    /// This method is called when an integer value changes
    void onChangeInteger(int value);

    /// This method is called when a double value changes
    void onChangeDouble(double value);

    /// This method is called when a double slider value changes
    void onChangeDoubleSlider(int value);

    /// This method is called when selection changes (QComboBox)
    void onChangeEnum(int value);

    /// This method is called to connect sliders to their labels
    static void onSliderMapped(QLabel* label, QSlider* slider);

    /// This method is called to connect double sliders to their labels
    static void onDoubleSliderMapped(QLabel* label, QSlider* slider);

    /// This method is called to connect reset buttons and checkboxes
    void onResetBooleanMapped(QWidget* widget);

    /// This method is called to connect reset buttons and color widgets
    void onResetColorMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetIntegerMapped(QWidget* widget);

    /// This method is called to connect reset buttons and sliders
    void onResetDoubleMapped(QWidget* widget);

    /// This method is called when the integer slider range is modified, it updates the min and max labels
    static void onSliderRangeMapped(QLabel* minLabel, QLabel* maxLabel, QSlider* slider);

    /// This method is called when the double slider range is modified, it updates the min and max labels
    static void onDoubleSliderRangeMapped(QLabel* minLabel, QLabel* maxLabel, QSlider* slider);

private:

    /**
     * @brief Called on all dependent widget to update it.
     * @param _watched Widget to update.
     * @param _event Event type, only care about ::QEvent::EnabledChange
     * @return False.
     */
    bool eventFilter(QObject* _watched, QEvent* _event) override;

    /// Snippet to create the reset button
    QPushButton* createResetButton(const std::string& key);

    /// Create a widget associated with a boolean type
    void createBoolWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        const std::string& defaultValue,
        bool resetButton
    );

    /// Create a widget associated with a color type
    void createColorWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        const std::string& defaultValue,
        bool resetButton
    );

    /// Create a widget associated with a double type
    void createDoubleWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        double defaultValue,
        double min,
        double max,
        int count,
        bool resetButton = true
    );

    /// Create a slider widget associated with a double type.
    void createDoubleSliderWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        double defaultValue,
        double min,
        double max,
        std::uint8_t decimals,
        bool resetButton,
        bool onRelease
    );

    /// Create a slider widget associated with an integer type
    void createIntegerSliderWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        int defaultValue,
        int min,
        int max,
        bool resetButton,
        bool onRelease
    );

    /// Create a spin widget associated with an integer type
    void createIntegerSpinWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        int defaultValue,
        int min,
        int max,
        int count,
        bool resetButton
    );

    /// Parses the string for an enum
    static void parseEnumString(
        const std::string& options,
        std::vector<std::string>& labels,
        std::vector<std::string>& keys,
        std::string separators = ", ;"
    );

    /// Create a multi choice widget
    void createEnumWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        const std::string& defaultValue,
        const std::vector<std::string>& values,
        const std::vector<std::string>& data
    );

    /// Create a multi choice widget with integer values
    void createSliderEnumWidget(
        QGridLayout& layout,
        int row,
        const std::string& key,
        const std::string& defaultValue,
        const std::vector<std::string>& values,
        bool onRelease
    );

    /// Emit the signal(s) for the integer widget
    void emitIntegerSignal(QObject* widget);

    /// Emit the signal(s) for the double widget
    void emitDoubleSignal(QObject* widget);

    /// Emit the signal for the color widget
    void emitColorSignal(const QColor color, const std::string& key);

    /**
     * @name Slots
     * @{
     */

    /// Slot: This method is used to set any parameter.
    void setParameter(sight::ui::base::parameter_t val, std::string key);

    /// Slot: This method is used to set a boolean parameter.
    void setBoolParameter(bool val, std::string key);

    /// Slot: This method is used to set a color parameter.
    void setColorParameter(std::array<std::uint8_t, 4> color, std::string key);

    /// Slot: This method is used to set a double parameter.
    void setDoubleParameter(double val, std::string key);

    /// Slot: This method is used to set two double parameters.
    void setDouble2Parameter(double val0, double val1, std::string key);

    /// Slot: This method is used to set three double parameters.
    void setDouble3Parameter(double val0, double val1, double val2, std::string key);

    /// Slot: This method is used to set an integer parameter.
    void setIntParameter(int val, std::string key);

    /// Slot: This method is used to set two int parameters.
    void setInt2Parameter(int val0, int val1, std::string key);

    /// Slot: This method is used to set three int parameters.
    void setInt3Parameter(int val0, int val1, int val2, std::string key);

    /// Slot: This method is used to set an enum parameter.
    void setEnumParameter(std::string val, std::string key);

    /// SLOT: This method sets an enum parameter using the index of the enum
    void setEnumIndexParameter(int /*val*/, std::string key);

    /// SLOT: This method updates the all enum values using a tokenized string ("value1;value2")
    void updateEnumRange(std::string options, std::string key);

    /// Slot: Updates the minimum value of an integer parameter (int, int2, int3)
    void updateIntMinParameter(int min, std::string key);

    /// Slot: Updates the maximum value of an integer parameter (int, int2, int3)
    void updateIntMaxParameter(int max, std::string key);

    /// Slot: Updates the minimum value of a double parameter (double, double2, double3)
    void updateDoubleMinParameter(double min, std::string key);

    /// Slot: Updates the maximum value of a double parameter (double, double2, double3)
    void updateDoubleMaxParameter(double max, std::string key);

    /// @}

    /// Internal function that updates enum widget value using a list of string (each element can contains value & data
    /// ex:"Value=data")
    void updateEnumList(const std::vector<std::string>& _list, const std::string _key);

    /// Return the widget of the parameter with the given key, or nullptr if it does not exist
    QWidget* getParamWidget(const std::string& key);

    /// Compute the double slider value from a slider position.
    static double getDoubleSliderValue(const QSlider* slider);

    /// Compute the double slider range according to the min and max property, update the internal slider value
    /// according to the new range
    static void setDoubleSliderRange(QSlider* slider, double currentValue);

    /// Adjust the minimum size of a label according to the range values
    template<typename T>
    static void setLabelMinimumSize(QLabel* label, T min, T max, std::uint8_t decimals = 0);

    template<typename T>
    static QString valueToStringLabel(T value, std::uint8_t decimals);

    /// Block (or not) signal emission for this service
    void blockSignals(bool block);

    /// if true, the signals are not emitted
    bool m_blockSignals {false};
};

//------------------------------------------------------------------------------

template<>
inline QString SParameters::valueToStringLabel<int>(int value, std::uint8_t /*unused*/)
{
    return QString("%1").arg(value);
}

//------------------------------------------------------------------------------

template<>
inline QString SParameters::valueToStringLabel<double>(double value, std::uint8_t decimals)
{
    return QString::number(value, 'f', decimals);
}

//------------------------------------------------------------------------------

template<typename T>
void SParameters::setLabelMinimumSize(QLabel* label, T min, T max, std::uint8_t decimals)
{
    const auto minString = valueToStringLabel(min, decimals);
    const auto maxString = valueToStringLabel(max, decimals);

    // Create a dummy label with same properties
    QLabel dummyLabel;
    dummyLabel.setFont(label->font());
    dummyLabel.setStyleSheet(label->styleSheet());

    // Fill it with the string of the max value and request the size from Qt
    dummyLabel.setText(maxString);
    const QSize sizeWithMaxValue = dummyLabel.sizeHint();

    // Fill it with the string of the min value and request the size from Qt
    dummyLabel.setText(minString);
    const QSize sizeWithMinValue = dummyLabel.sizeHint();

    // Compute the maximum size and set it to our label
    const QSize maxSize = sizeWithMaxValue.expandedTo(sizeWithMinValue);
    label->setMinimumSize(maxSize);
}

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
