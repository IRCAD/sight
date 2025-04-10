/************************************************************************
 *
 * Copyright (C) 2024-2025 IRCAD France
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

#include <io/joystick/interactor.hpp>

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
 * @brief Generic editor to interact with properties.
 *
 * It allows to setup an editor with several properties.
 * Once the user validates the properties, a signal is sent containing the key and the value.
 * It supports booleans, doubles or integer at the moment.
 *
 * @section Slots Slots
 * - \b update_enum_range(std::string, std::string): update range of an existing enum (value can contains a tokenized
 * list
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
 * The configuration with several objects is as follows:
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::qt::settings" >
        <inout group="keys">
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
            <key uid="..." />
        </inout>
        <ui scrollable="true">
            <item name="label1" key="param1" />
            <item name="label2" key="param2"  min="1.5" max="42.42" depends="param1" />
            <item name="label3" key="int" min="0" max="2" depends="param1" depends_reverse="true" />
            <item name="label4" key="param4"values="p1,p2,p3" />
            <item name="label5" key="param5" min="0" max="255" depends="param4" depends_value="p2" />
            <item name="button list:" key="buttonRaw" widget="buttonBar" >
                <item value="button1" label="..." icon="..." uncheckedIcon="..."/>
                <item value="button2" label="..." icon="..."/>
                <item value="button3" icon="..."/>
            </item>
            <item name="label6" key="..." widget="slider" orientation="vertical" .../>
        </ui>
       </service>
   @endcode
 *
 * The configuration with a map of objects is as follows:
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::qt::settings" >
        <inout key="map" uid="..." />
        <ui scrollable="true">
            <item name="label1" key="param1" />
            <item name="label2" key="param2"  min="1.5" max="42.42" depends="param1" />
            <item name="label3" key="int" min="0" max="2" depends="param1" depends_reverse="true" />
            <item name="label4" key="param4"values="p1,p2,p3" />
            <item name="label5" key="param5" min="0" max="255" depends="param4" depends_value="p2" />
            <item name="button list:" key="buttonRaw" widget="buttonBar" >
                <item value="button1" label="..." icon="..." uncheckedIcon="..."/>
                <item value="button2" label="..." icon="..."/>
                <item value="button3" icon="..."/>
            </item>
            <item name="label6" key="..." widget="slider" orientation="vertical" .../>
        </ui>
       </service>
   @endcode
 * No default value can be given because in this configuration, it is the responsibility if the data to do it. Doing
 * otherwise could be confusing and would lead to timing issues with start order.
 * Also, the type is defined by the type of the object key in the properties map.
 *
 * @subsection Configuration Configuration:
 * <properties> tag:
 * - \b scrollable: If true, add a scroll bar if the content doesn't fit on the screen. If false, flatten the content
 * <param> tag:
 * - \b name: label to display.
 * - \b key: name used in the signal to identify the parameter.
 * - \b min: minimum value, if relevant for the data type.
 * - \b max: maximum value, if relevant for the data type.
 * - \b hide_min_max (optional, boolean): allows to hide the min and max labels.
 * - \b widget (optional) : widget type, available for types 'sight::data::integer', 'sight::data::real',
 * 'sight::data::string'.
 * For 'sight::data::real', you can choose between a 'spin' or a 'slider' widget. Defaults to 'spin'.
 * For 'sight::data::integer', you can choose between a 'spin', a 'slider', a 'combobox', a 'comboslider', or
 * a 'buttonBar'.
 * For 'string', you can choose between 'text', 'file_[read/write]', 'dir_[read/write]',
 * buttonBar widget requires additional configuration.
 *     - \b value: the enum value sent when clicking on the button.
 *     - \b label (optional, default=""): test displayed under the button.
 *     - \b icon: path to the icon to display.
 * - \b decimals (optional, default=2): number of decimals settable using a double slider.
 * - \b reset (optional, default=true): display the reset button.
 * - \b values: for 'combobox', 'comboslider', or 'buttonBar' widgets, the list of possible values separated by a
 * comma ',' a space ' ' or a semicolon ';'.
 * The actual displayed value and the returned one in the signal can be different using '=' to separate the two. For
 * example 'values="BLEND=imageBlend,CHECKERBOARD=imageCheckerboard"' means the combo will display BLEND, CHECKERBOARD
 * and will send 'imageBlend' or 'imageCheckerboard'.
 * - \b depends (optional, string): key of the dependency.
 * - \b depends_value (optional, string): value of the dependency in case of enum.
 * - \b depends_reverse (optional, bool, default=false): reverse the dependency status checking.
 * - \b emit_on_release (optional, default = false): sliders only, if true send value when slider is released,
 * send value when value changed otherwise.
 * - \b min_width (optional, int) Minimum width, in device coordinates. @todo Support relative widget size.
 * - \b min_height (optional, int) Minimum height, in device coordinates. @todo Support relative widget size.
 */
class settings : public QObject,
                 public sight::ui::editor,
                 public sight::io::joystick::interactor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(settings, sight::ui::editor);

    /// @brief  Struct to handle all slots
    struct slots
    {
        using slot_key_t = core::com::slots::key_t;
        inline static const slot_key_t UPDATE_ENUM_RANGE_SLOT           = "update_enum_range";
        inline static const slot_key_t UPDATE_INT_MIN_PARAMETER_SLOT    = "update_int_min_parameter";
        inline static const slot_key_t UPDATE_INT_MAX_PARAMETER_SLOT    = "update_int_max_parameter";
        inline static const slot_key_t UPDATE_DOUBLE_MIN_PARAMETER_SLOT = "update_double_min_parameter";
        inline static const slot_key_t UPDATE_DOUBLE_MAX_PARAMETER_SLOT = "update_double_max_parameter";
    };

    struct enum_button_param
    {
        std::string value {""};
        std::string label {""};
        std::string icon_path {""};
    };

    struct param_widget
    {
        std::string name                = {};
        std::string key                 = {};
        std::size_t data_index          = {0};
        std::string default_value       = {};
        bool reset_button               = true;
        bool hide_min_max               = false;
        bool preference                 = false;
        boost::optional<int> min_width  = {};
        boost::optional<int> min_height = {};
    };

    struct qt_property
    {
        inline static const char* key        = "key";
        inline static const char* data_index = "data_index";
        inline static const char* count      = "count";
        inline static const char* index      = "index";
    };

    template<typename T>
    struct scalar_widget : param_widget
    {
        T min = T {0};
        T max = T {1};
    };

    using int_widget    = scalar_widget<int>;
    using double_widget = scalar_widget<double>;

    settings() noexcept;

    /// Destructor. Does nothing
    ~settings() noexcept override = default;

protected:

    /// Configure the editor.
    void configuring() override;

    /// Initializes Qt input widgets for settings according to xml configuration
    void starting() override;

    /// This method launches the editor::stopping method
    void stopping() override;

    /// This method is used to update services. Does nothing
    void updating() override;

    /**
     * @brief Manage joystick events
     *
     * @param _event
     */
    void joystick_axis_direction_event(const sight::io::joystick::axis_direction_event& _event) final;

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

    /// This method is called when a color button is clicked
    void on_color_button();

    /// This method is called when an integer value changes
    void on_change_integer(int _value);

    /// This method is called when a double value changes
    void on_change_double(double _value);

    /// This method is called when a double slider value changes
    void on_change_double_slider(int _value);

    /// This method is called to connect sliders to their labels
    static void on_slider_mapped(QLabel* _label, QSlider* _slider);

    /// This method is called to connect double sliders to their labels
    static void on_double_slider_mapped(QLabel* _label, QSlider* _slider);

    /// This method is called to connect reset buttons and checkboxes
    void on_reset_boolean(QWidget* _widget);

    /// This method is called to connect reset buttons and color widgets
    void on_reset_color(QWidget* _widget);

    /// This method is called to connect reset buttons and sliders
    void on_reset_integer(QWidget* _widget);

    /// This method is called to connect reset buttons and sliders
    void on_reset_double(QWidget* _widget);

    /// This method is called to connect reset buttons and text widgets
    void on_reset_string(QWidget* _widget);

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
    QPushButton* create_bool_widget(QBoxLayout* _layout, const param_widget& _setup, Qt::Orientation _orientation);

    /// Create a widget associated with a color type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_color_widget(QBoxLayout* _layout, const param_widget& _setup);

    /// Create a widget associated with a double type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_double_spin_widget(
        QBoxLayout* _layout,
        const double_widget& _setup,
        int _count,
        Qt::Orientation _orientation
    );

    /// Create a slider widget associated with a double type.
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_double_slider_widget(
        QBoxLayout* _layout,
        const double_widget& _setup,
        std::uint8_t _decimals,
        Qt::Orientation _orientation,
        bool _on_release
    );

    /// Create a slider widget associated with an integer type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_integer_slider_widget(
        QBoxLayout* _layout,
        const int_widget& _setup,
        Qt::Orientation _orientation,
        bool _on_release
    );

    /// Create a spin widget associated with an integer type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_integer_spin_widget(
        QBoxLayout* _layout,
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
    void create_enum_combobox_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        const std::vector<std::string>& _values,
        const std::vector<std::string>& _data
    );

    /// Create a multi choice widget with integer values
    void create_enum_slider_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        const std::vector<std::string>& _values,
        Qt::Orientation _orientation,
        bool _on_release
    );

    void create_enum_button_bar_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        const std::vector<enum_button_param>& _button_list,
        const int _width,
        const int _height,
        const int _spacing,
        const std::string& _style,
        Qt::Orientation _orientation
    );

    /// Create a text widget associated with a string type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_text_widget(QBoxLayout* _layout, const param_widget& _setup, const std::string& _type);

    /**
     * @name Slots
     * @{
     */

    template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
    requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
    void set_parameter(const SUBTYPE& _val, std::string _key);

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
    void block_signals(bool _block);

    /// Returns the data pointer at the given index and key
    template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
    requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> > CSPTR(DATATYPE) data(const QObject * _widget);

    template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
    requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
    void connect_data(const CSPTR(DATATYPE)& _obj, const std::string& _key);

    template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
    requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
    void update_data(const QObject* _widget, const SUBTYPE& _val);

    /// if true, the signals are not emitted
    bool m_block_signals {false};

    /// The list of intermediate boxes containing each widgets. This array is processed each time we need to find
    /// a parameter with a given key (when enabling/disabling, etc.).
    /// This vector is cleared upon stopping().
    std::vector<QPointer<QWidget> > m_param_boxes;

    /// Used when we bind widgets to data
    data::ptr_vector<data::object, data::access::inout> m_settings {this, "keys"};

    /// Used when we bind widgets to a map of data
    data::ptr<data::map, data::access::inout> m_settings_map {this, "map"};

    using object_modified_t         = core::com::slot<void ()>;
    using settings_slot_container_t = std::map<std::string, SPTR(object_modified_t)>;
    settings_slot_container_t m_settings_slots;

    std::vector<std::string> m_joystickable_widgets_key;
};

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
