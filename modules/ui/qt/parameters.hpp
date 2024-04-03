/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#include <ui/__/editor.hpp>
#include <ui/__/parameter.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>

class QGroupBox;

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
 * - \b parameter_changed(ui::parameter_t, std::string): Emitted when any parameter changes.
 * - \b bool_changed(bool, std::string): Emitted when a boolean parameter changes.
 * - \b color_changed(std::array<std::uint8_t, 4>, std::string): Emitted when a color parameter changes.
 * - \b double_changed(double, std::string): Emitted when a real parameter changes.
 * - \b double2_changed(double, double, std::string): Emitted when two real parameters change.
 * - \b double3_changed(double, double, double, std::string): Emitted when three real parameters change.
 * - \b int_changed(int, std::string): Emitted when an integer parameter changes.
 * - \b int2_changed(int, int, std::string): Emitted when two integer parameters change.
 * - \b int3_changed(int, int, int, std::string): Emitted when three integer parameters change.
 * - \b enum_changed(std::string, std::string): Emitted when enum parameter changes, returns the the label of the
 * currently selected item.
 * - \b enum_index_changed(int, std::string): Emitted when enum parameter changes, returns the index of the currently
 * selected item.
 *
 * @section Slots Slots
 * - \b set_parameter(ui::parameter_t, std::string): set a parameter.
 * - \b set_bool_parameter(bool, std::string): set a boolean parameter.
 * - \b set_color_parameter(std::array<std::uint8_t, 4>, std::string): set a color parameter.
 * - \b set_double_parameter(double, std::string): set a double parameter.
 * - \b set_double2_parameter(double, double, std::string): set two double parameters.
 * - \b set_double3_parameter(double, double, double, std::string): set three double parameters.
 * - \b set_int_parameter(int, std::string): set an integer parameter.
 * - \b set_int2_parameter(int, int, std::string): set two int parameters.
 * - \b set_int3_parameter(int, int, int, std::string): set three int parameters.
 * - \b set_enum_parameter(std::string, std::string): set an enum parameter.
 * - \b set_enum_index_parameter(int, std::string): set an enum parameter using the index of the enum.
 * - \b updateEnumRange(std::string, std::string): update range of an existing enum (value can contains a tokenized list
 * such as value1;value2;value3=test;...)
 * - \b update_int_min_parameter(int, std::string): set the minimum value of an integer parameter (int, int2, int3)
 * - \b update_int_max_parameter(int, std::string): set the maximum value of an integer parameter (int, int2, int3)
 * - \b update_double_min_parameter(double, std::string): set the minimum value of a double parameter (double, double2,
 * double3)
 * - \b update_double_max_parameter(double, std::string): set the maximum value of a double parameter (double, double2,
 * double3)
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::qt::parameters" >
        <parameters scrollable="true">
            <param type="bool" name="boolean parameter" key="boolParam" defaultValue="false" />
            <param type="double" name="real parameter" key="doubleParam" defaultValue="" min="1.5" max="42.42"
                depends="boolParam" />
            <param type="int" name="integer parameter" key="intParam" defaultValue="1" min="0" max="255"
                depends="boolParam" dependsReverse="true" />
            <param type="enum" name="enum parameters" key="enumParam" defaultValue="p1" values="p1,p2,p3" />
            <param type="int" name="integer parameter" key="intParam2" defaultValue="1" min="0" max="255"
                depends="enumParam" dependsValue="p2" />
            <param type="enum"    name="button list:" key="buttonRaw" widget="buttonBar" defaultValue="button2">
                <item value="button1" label="..." icon="..." uncheckedIcon="..."/>
                <item value="button2" label="..." icon="..."/>
                <item value="button3" icon="..."/>
            </param>
            <param type="int" name="Something" key="..." widget="slider" orientation="vertical" .../>
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
 * For 'enum', you can choose between 'combobox' (the default), 'slider' (only available with integer values), or
 *'buttonBar'.
 * buttonBar widget requires additional configuration.
 *     - \b value: the enum value sent when clicking on the button.
 *     - \b label (optional, default=""): test displayed under the button.
 *     - \b icon: path to the icon to display.
 * - \b decimals (optional, default=2): number of decimals settable using a double slider.
 * - \b reset (optional, default=true): display the reset button.
 * - \b values: list of possible values separated by a comma ',' a space ' ' or a semicolon ';' (only for enum type).
 * The actual displayed value and the returned one in the signal can be different using '=' to separate the two. For
 * example 'values="BLEND=imageBlend,CHECKERBOARD=imageCheckerboard"' means the combo will display BLEND, CHECKERBOARD
 * and will send 'imageBlend' or 'imageCheckerboard'.
 * - \b depends (optional, string): key of the dependency.
 * - \b dependsValue (optional, string): value of the dependency in case of enum.
 * - \b dependsReverse (optional, bool, default=false): reverse the dependency status checking.
 * - \b emitOnRelease (optional, default = false): sliders only, if true send value when slider is released,
 * send value when value changed otherwise.
 * - \b min_width (optional, int) Minimum width, in device coordinates. @todo Support relative widget size.
 * - \b min_height (optional, int) Minimum height, in device coordinates. @todo Support relative widget size.
 */
class parameters : public QObject,
                   public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(parameters, sight::ui::editor);

    /// @brief  Struct to handle all signals.
    struct signals
    {
        using signal_t = core::com::signals::key_t;
        /// Generic changed signal type
        using changed_signal_t = core::com::signal<void (sight::ui::parameter_t, std::string)>;

        /// Boolean changed signal type
        using boolean_changed_signal_t = core::com::signal<void (bool, std::string)>;

        /// Color changed signal type
        using color_changed_signal_t = core::com::signal<void (std::array<std::uint8_t, 4>, std::string)>;

        /// Double changed signal type
        using double_changed_signal_t  = core::com::signal<void (double, std::string)>;
        using double2_changed_signal_t = core::com::signal<void (double, double, std::string)>;
        using double3_changed_signal_t = core::com::signal<void (double, double, double, std::string)>;

        /// Integer changed signal type
        using integer_changed_signal_t  = core::com::signal<void (int, std::string)>;
        using integer2_changed_signal_t = core::com::signal<void (int, int, std::string)>;
        using integer3_changed_signal_t = core::com::signal<void (int, int, int, std::string)>;

        /// Enum changed signal type
        using enum_changed_signal_t       = core::com::signal<void (std::string, std::string)>;
        using enum_changed_index_signal_t = core::com::signal<void (int, std::string)>;

        inline static const signal_t PARAMETER_CHANGED_SIG  = "parameter_changed";
        inline static const signal_t BOOLEAN_CHANGED_SIG    = "bool_changed";
        inline static const signal_t COLOR_CHANGED_SIG      = "color_changed";
        inline static const signal_t DOUBLE_CHANGED_SIG     = "double_changed";
        inline static const signal_t DOUBLE2_CHANGED_SIG    = "double2_changed";
        inline static const signal_t DOUBLE3_CHANGED_SIG    = "double3_changed";
        inline static const signal_t INTEGER_CHANGED_SIG    = "int_changed";
        inline static const signal_t INTEGER2_CHANGED_SIG   = "int2_changed";
        inline static const signal_t INTEGER3_CHANGED_SIG   = "int3_changed";
        inline static const signal_t ENUM_CHANGED_SIG       = "enum_changed";
        inline static const signal_t ENUM_INDEX_CHANGED_SIG = "enum_index_changed";
    };

    /// @brief  Struct to handle all slots
    struct slots
    {
        using slots_t = core::com::slots::key_t;

        inline static const slots_t SET_PARAMETER_SLOT               = "set_parameter";
        inline static const slots_t SET_BOOL_PARAMETER_SLOT          = "set_bool_parameter";
        inline static const slots_t SET_COLOR_PARAMETER_SLOT         = "set_color_parameter";
        inline static const slots_t SET_DOUBLE_PARAMETER_SLOT        = "set_double_parameter";
        inline static const slots_t SET_DOUBLE2_PARAMETER_SLOT       = "set_double2_parameter";
        inline static const slots_t SET_DOUBLE3_PARAMETER_SLOT       = "set_double3_parameter";
        inline static const slots_t SET_INT_PARAMETER_SLOT           = "set_int_parameter";
        inline static const slots_t SET_INT2_PARAMETER_SLOT          = "set_int2_parameter";
        inline static const slots_t SET_INT3_PARAMETER_SLOT          = "set_int3_parameter";
        inline static const slots_t SET_ENUM_PARAMETER_SLOT          = "set_enum_parameter";
        inline static const slots_t SET_ENUM_INDEX_PARAMETER_SLOT    = "set_enum_index_parameter";
        inline static const slots_t UPDATE_ENUM_RANGE_SLOT           = "updateEnumRange";
        inline static const slots_t UPDATE_INT_MIN_PARAMETER_SLOT    = "update_int_min_parameter";
        inline static const slots_t UPDATE_INT_MAX_PARAMETER_SLOT    = "update_int_max_parameter";
        inline static const slots_t UPDATE_DOUBLE_MIN_PARAMETER_SLOT = "update_double_min_parameter";
        inline static const slots_t UPDATE_DOUBLE_MAX_PARAMETER_SLOT = "update_double_max_parameter";
    };

    struct enum_button_param
    {
        std::string value {""};
        std::string label {""};
        std::string icon_path {""};
    };

    struct param_widget
    {
        std::string name          = {};
        std::string key           = {};
        std::string default_value = {};
        bool reset_button         = true;
        bool hide_min_max         = false;
        QSize min_size            = {0, 0};
    };

    template<typename T>
    struct scalar_widget : param_widget
    {
        T min = T {0};
        T max = T {1};
    };

    using int_widget    = scalar_widget<int>;
    using double_widget = scalar_widget<double>;

    parameters() noexcept;

    /// Destructor. Does nothing
    ~parameters() noexcept override = default;

    /// Configure the editor.
    void configuring() override;

    /// Initializes Qt input widgets for parameters according to xml configuration
    void starting() override;

    /// This method launches the editor::stopping method
    void stopping() override;

    /// This method is used to update services. Does nothing
    void updating() override;

private Q_SLOTS:

    /**
     * @brief Called when a dependency widget state (enable or disable) has changed to modify the state of the child
     * widget.
     * @param _check_box Dependency widget.
     * @param _widget Child widget.
     * @param _reverse Reverse the state check.
     */
    static void on_depends_changed(QCheckBox* _check_box, QWidget* _widget, bool _reverse);

    /**
     * @brief Called when a dependency widget state (enable or disable) has changed to modify the state of the child
     * widget.
     * @param _combo_box Dependency widget.
     * @param _widget Child widget.
     * @param _value Value of the combo box.
     * @param _reverse Reverse the state check.
     */
    static void on_depends_changed(QComboBox* _combo_box, QWidget* _widget, const std::string& _value, bool _reverse);

    /// This method is called when a boolean value changes
    void on_change_boolean(int _value) const;

    /// This method is called when a color button is clicked
    void on_color_button();

    /// This method is called when an integer value changes
    void on_change_integer(int _value) const;

    /// This method is called when a double value changes
    void on_change_double(double _value) const;

    /// This method is called when a double slider value changes
    void on_change_double_slider(int _value) const;

    /// This method is called when selection changes (QComboBox)
    void on_change_enum(int _value) const;

    /// This method is called to connect sliders to their labels
    static void on_slider_mapped(QLabel* _label, QSlider* _slider);

    /// This method is called to connect double sliders to their labels
    static void on_double_slider_mapped(QLabel* _label, QSlider* _slider);

    /// This method is called to connect reset buttons and checkboxes
    void on_reset_boolean_mapped(QWidget* _widget) const;

    /// This method is called to connect reset buttons and color widgets
    void on_reset_color_mapped(QWidget* _widget) const;

    /// This method is called to connect reset buttons and sliders
    void on_reset_integer_mapped(QWidget* _widget);

    /// This method is called to connect reset buttons and sliders
    void on_reset_double_mapped(QWidget* _widget);

    /// This method is called when the integer slider range is modified, it updates the min and max labels
    static void on_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider);

    /// This method is called when the double slider range is modified, it updates the min and max labels
    static void on_double_slider_range_mapped(QLabel* _min_label, QLabel* _max_label, QSlider* _slider);

private:

    /**
     * @brief Called on all dependent widget to update it.
     * @param _watched Widget to update.
     * @param _event Event type, only care about ::QEvent::EnabledChange
     * @return False.
     */
    bool eventFilter(QObject* _watched, QEvent* _event) override;

    /// Creates a reset button for one widget.
    /// @param _key Name of the parameter it resets.
    /// @param _on_click Slot to call when the button is clicked (when QPushButton::clicked is sent)
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_reset_button(const std::string& _key, std::function<void()> _on_click) const;

    /// Create a widget associated with a boolean type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_bool_widget(QBoxLayout& _layout, const param_widget& _setup) const;

    /// Create a widget associated with a color type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_color_widget(QBoxLayout& _layout, const param_widget& _setup) const;

    /// Create a widget associated with a double type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_double_widget(
        QBoxLayout& _layout,
        const double_widget& _setup,
        int _count,
        Qt::Orientation _orientation
    );

    /// Create a slider widget associated with a double type.
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_double_slider_widget(
        QBoxLayout& _layout,
        const double_widget& _setup,
        std::uint8_t _decimals,
        Qt::Orientation _orientation,
        bool _on_release
    );

    /// Create a slider widget associated with an integer type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_integer_slider_widget(
        QBoxLayout& _layout,
        const int_widget& _setup,
        Qt::Orientation _orientation,
        bool _on_release
    );

    /// Create a spin widget associated with an integer type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_integer_spin_widget(
        QBoxLayout& _layout,
        const int_widget& _setup,
        int _count,
        Qt::Orientation _orientation
    );

    /// Parses the string for an enum
    static void parse_enum_string(
        const std::string& _options,
        std::vector<std::string>& _labels,
        std::vector<std::string>& _keys,
        std::string _separators = ", ;"
    );

    /// Create a multi choice widget
    void create_enum_widget(
        QBoxLayout& _layout,
        const param_widget& _setup,
        const std::vector<std::string>& _values,
        const std::vector<std::string>& _data
    ) const;

    /// Create a multi choice widget with integer values
    void create_slider_enum_widget(
        QBoxLayout& _layout,
        const param_widget& _setup,
        const std::vector<std::string>& _values,
        Qt::Orientation _orientation,
        bool _on_release
    ) const;

    void create_button_bar_enum_widget(
        QBoxLayout& _layout,
        const param_widget& _setup,
        const std::vector<enum_button_param>& _button_list,
        const int _width,
        const int _height,
        const int _spacing,
        const std::string& _style,
        Qt::Orientation _orientation
    ) const;

    /// Emit the signal(s) for the integer widget
    void emit_integer_signal(QObject* _widget) const;

    /// Emit the signal(s) for the double widget
    void emit_double_signal(QObject* _widget) const;

    /// Emit the signal for the color widget
    void emit_color_signal(const QColor _color, const std::string& _key) const;

    /**
     * @name Slots
     * @{
     */

    /// Slot: This method is used to set any parameter.
    void set_parameter(sight::ui::parameter_t _val, std::string _key);

    /// Slot: This method is used to set a boolean parameter.
    void set_bool_parameter(bool _val, std::string _key);

    /// Slot: This method is used to set a color parameter.
    void set_color_parameter(std::array<std::uint8_t, 4> _color, std::string _key);

    /// Slot: This method is used to set a double parameter.
    void set_double_parameter(double _val, std::string _key);

    /// Slot: This method is used to set two double parameters.
    void set_double2_parameter(double _val0, double _val1, std::string _key);

    /// Slot: This method is used to set three double parameters.
    void set_double3_parameter(double _val0, double _val1, double _val2, std::string _key);

    /// Slot: This method is used to set an integer parameter.
    void set_int_parameter(int _val, std::string _key);

    /// Slot: This method is used to set two int parameters.
    void set_int2_parameter(int _val0, int _val1, std::string _key);

    /// Slot: This method is used to set three int parameters.
    void set_int3_parameter(int _val0, int _val1, int _val2, std::string _key);

    /// Slot: This method is used to set an enum parameter.
    void set_enum_parameter(std::string _val, std::string _key);

    /// SLOT: This method sets an enum parameter using the index of the enum
    void set_enum_index_parameter(int /*val*/, std::string _key);

    /// SLOT: This method updates the all enum values using a tokenized string ("value1;value2")
    void update_enum_range(std::string _options, std::string _key);

    /// Slot: Updates the minimum value of an integer parameter (int, int2, int3)
    void update_int_min_parameter(int _min, std::string _key);

    /// Slot: Updates the maximum value of an integer parameter (int, int2, int3)
    void update_int_max_parameter(int _max, std::string _key);

    /// Slot: Updates the minimum value of a double parameter (double, double2, double3)
    void update_double_min_parameter(double _min, std::string _key);

    /// Slot: Updates the maximum value of a double parameter (double, double2, double3)
    void update_double_max_parameter(double _max, std::string _key);

    /// @}

    /// Internal function that updates enum widget value using a list of string (each element can contains value & data
    /// ex:"Value=data")
    void update_enum_list(const std::vector<std::string>& _list, const std::string _key);

    /// Return the widget of the parameter with the given key, or nullptr if it does not exist
    QObject* get_param_widget(const std::string& _key);

    /// Compute the double slider value from a slider position.
    static double get_double_slider_value(const QSlider* _slider);

    /// Compute the double slider range according to the min and max property, update the internal slider value
    /// according to the new range
    static void set_double_slider_range(QSlider* _slider, double _current_value);

    /// Adjust the minimum size of a label according to the range values
    template<typename T>
    static void set_label_minimum_size(QLabel* _label, T _min, T _max, std::uint8_t _decimals = 0);

    template<typename T>
    static QString value_to_string_label(T _value, std::uint8_t _decimals);

    /// Block (or not) signal emission for this service
    void blockSignals(bool _block);

    /// if true, the signals are not emitted
    bool m_block_signals {false};

    /// The list of intermediate boxes containing each widgets. This array is processed each time we need to find
    /// a parameter with a given key (when enabling/disabling, etc.).
    /// This vector is cleared upon stopping().
    std::vector<QPointer<QGroupBox> > m_param_boxes;
};

//------------------------------------------------------------------------------

template<>
inline QString parameters::value_to_string_label<int>(int _value, std::uint8_t /*unused*/)
{
    return QString("%1").arg(_value);
}

//------------------------------------------------------------------------------

template<>
inline QString parameters::value_to_string_label<double>(double _value, std::uint8_t _decimals)
{
    return QString::number(_value, 'f', _decimals);
}

//------------------------------------------------------------------------------

template<typename T>
void parameters::set_label_minimum_size(QLabel* _label, T _min, T _max, std::uint8_t _decimals)
{
    const auto min_string = value_to_string_label(_min, _decimals);
    const auto max_string = value_to_string_label(_max, _decimals);

    // Create a dummy label with same properties
    QLabel dummy_label;
    dummy_label.setFont(_label->font());
    dummy_label.setStyleSheet(_label->styleSheet());

    // Fill it with the string of the max value and request the size from Qt
    dummy_label.setText(max_string);
    const QSize size_with_max_value = dummy_label.sizeHint();

    // Fill it with the string of the min value and request the size from Qt
    dummy_label.setText(min_string);
    const QSize size_with_min_value = dummy_label.sizeHint();

    // Compute the maximum size and set it to our label
    const QSize max_size = size_with_max_value.expandedTo(size_with_min_value);
    _label->setMinimumSize(max_size);
}

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
