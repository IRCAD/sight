/************************************************************************
 *
 * Copyright (C) 2024-2026 IRCAD France
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

#include <data/real.hpp>
#include <data/string.hpp>

#include <ui/__/editor.hpp>
#include <ui/qt/widget/switch_button.hpp>
#include <ui/qt/widget/tickmarks_slider.hpp>

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QLabel>
#include <QObject>
#include <QPointer>
#include <QPushButton>
#include <QSlider>

namespace sight::module::ui::qt
{

/**
 * @brief Generic editor to interact with properties.
 *
 * It allows to setup an editor with several properties.
 * Once the user validates the properties, a signal is sent containing the key and the value.
 * It supports booleans, doubles or integer at the moment.
 *
 * @section Data
 * - \b item.data [sight::data::object] (mandatory): value edited by the matching item.
 * - \b item.values [sight::data::string] (optional): tokenized values of the matching item.
 * - \b item.min [sight::data::real] (optional): minimum bound of the matching item.
 * - \b item.max [sight::data::real] (optional): maximum bound of the matching item.
 *
 * @section XML XML Configuration
 *
 * The configuration with several objects is as follows:
 *
 * @code{.xml}
       <service uid="..." type="sight::module::ui::qt::settings" >
        <config scrollable="true" />
        <item data="${...}" name="label1" key="param1" />
        <item data="${...}" name="label2" key="param2"  min="1.5" max="42.42" depends="param1" />
        <item data="${...}" name="label3" key="int" min="0" max="2" depends="param1" depends_reverse="true" />
        <item data="${...}" name="label4" key="param4" values="p1,p2,p3" />
        <item data="${...}" name="label5" key="param5" min="0" max="255" depends="param4" depends_value="p2" />
        <item data="${...}" name="button list:" key="buttonRaw" widget="buttonBar" >
                <item value="button1" label="..." icon="..." uncheckedIcon="..."/>
                <item value="button2" label="..." icon="..."/>
                <item value="button3" icon="..."/>
            </item>
        <item data="${...}" name="label6" key="..." widget="slider" orientation="vertical" .../>
       </service>
   @endcode
 *
 * Also, the type is defined by the type of the bound object.
 *
 * @subsection Configuration Configuration:
 * <config> tag (individual objects):
 * - \b scrollable: If true, add a scroll bar if the content doesn't fit on the screen. If false, flatten the content
 * <item> tag:
 *
 * Static attributes:
 * - \b name: label to display.
 * - \b key: name used in the signal to identify the parameter.
 * - \b hide_min_max (optional, boolean): allows to hide the min and max labels.
 * - \b widget (optional) : widget type, available for types 'sight::data::integer', 'sight::data::real',
 * 'sight::data::string'.
 * For 'sight::data::real', you can choose between a 'spin' or a 'slider' widget. Defaults to 'spin'.
 * For 'sight::data::bool', you can choose between a 'check_box' or a 'switch_button' . Defaults to 'check_box'
 * For 'sight::data::integer', you can choose between a 'spin', a 'slider', a 'combobox', a 'comboslider', a 'tickmarks'
 *  , or a 'buttonBar'.
 * For 'string', you can choose between 'text', 'file_[read/write]', 'dir_[read/write]',
 * buttonBar widget requires additional configuration.
 *     - \b value: the enum value sent when clicking on the button.
 *     - \b label (optional, default=""): test displayed under the button.
 *     - \b icon: path to the icon to display.
 * - \b decimals (optional, default=2): number of decimals settable using a double slider.
 * - \b reset (optional, default=true): display the reset button.
 * - \b depends (optional, string): key of the dependency.
 * - \b depends_value (optional, string): value of the dependency in case of enum.
 * - \b depends_reverse (optional, bool, default=false): reverse the dependency status checking.
 * - \b emit_on_release (optional, default = false): sliders only, if true send value when slider is released,
 * send value when value changed otherwise.
 * - \b min_width (optional, int) Minimum width, in device coordinates. @todo Support relative widget size.
 * - \b min_height (optional, int) Minimum height, in device coordinates. @todo Support relative widget size.
 * - \b use_index (optional, bool, default=true): for 'comboslider', whether to use the index or the value (if "false")
 * of the element.
 * - \b joystick (optional, string): joystick alias to use for the widget. It can be 'left' or 'right'.
 * - \b joystick_axis (optional, string): joystick axes to use for the widget. It can be a combination up to three axes.
 *                                        Allowed values: 'rx', 'ry', 'rz', 'tx', 'ty', 'tz'.
 *
 * Runtime-adjustable attributes:
 * - \b values: for 'combobox', 'comboslider', or 'tickmarks' widgets. It accepts either a literal list or a
 *   \c sight::data::string binding. Values are separated by ',', ' ', or ';'. A displayed label and its stored value
 *   can be separated with '='. For example,
 *   \c values="BLEND=imageBlend,CHECKERBOARD=imageCheckerboard" displays \c BLEND and \c CHECKERBOARD while storing
 *   \c imageBlend and \c imageCheckerboard.
 * - \b min and \b max: for integer and real widgets. Each accepts either a literal numeric value or a
 *   \c sight::data::real binding. For integer widgets, the received value is converted with \c static_cast<int>.
 *
 * Literal values are read when the service starts. Bound \c sight::data::string and \c sight::data::real objects are
 * also observed: changing them and emitting \c sight::data::signals::MODIFIED refreshes the corresponding widget.
 *
 * @code{.xml}
 *     <!-- Static values and bounds -->
 *     <item data="${enum_value}" values="low,medium,high" />
 *     <item data="${numeric_value}" min="0" max="100" />
 *
 *     <!-- Dynamically updated values and bounds -->
 *     <item data="${enum_value}" values="${list_of_values}" />
 *     <item data="${numeric_value}" min="${minimum}" max="${maximum}" />
 * @endcode
 */
class settings : public QObject,
                 public sight::ui::editor,
                 public sight::io::joystick::interactor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(settings, sight::ui::editor);

    struct enum_button_param
    {
        std::string value {};
        std::string label {};
        std::string icon_path {};
    };

    struct param_widget
    {
        std::string name {};
        std::string key {};
        std::size_t data_index {0};
        std::string default_value {};
        bool reset_button {true};
        bool hide_min_max {false};
        bool preference {false};
        boost::optional<int> min_width {};
        boost::optional<int> min_height {};
        bool use_index {true};
    };

    struct qt_property
    {
        inline static const char* s_key        = "key";
        inline static const char* s_data_index = "data_index";
        inline static const char* s_count      = "count";
        inline static const char* s_index      = "index";
        inline static const char* s_use_index  = "use_index";
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

    /**
     * @brief Called on all dependent widget to update it.
     * @param _watched Widget to update.
     * @param _event Event type, only care about ::QEvent::EnabledChange
     * @return False.
     */
    bool eventFilter(QObject* _watched, QEvent* _event) override;

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
     * @param _switch_button Dependency widget.
     * @param _widget Child widget.
     * @param _reverse Reverse the state check.
     */
    static void on_depends_changed(
        sight::ui::qt::widget::switch_button* _switch_button,
        QWidget* _widget,
        bool _reverse
    );

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

    /// Creates a reset button for one widget.
    /// @param _key Name of the parameter it resets.
    /// @param _on_click Slot to call when the button is clicked (when QPushButton::clicked is sent)
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_reset_button(const std::string& _key, std::function<void()> _on_click) const;

    /// Create a widget associated with a boolean type
    /// @returns The reset button, to put in a layout of your choice, or nullptr if not required.
    [[nodiscard]]
    QPushButton* create_bool_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        Qt::Orientation _orientation,
        std::string _widget_type
    );

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
        std::string _separators = ",;\n\t"
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

    /// Create a tickmarks widget
    void create_tickmarks_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        const std::vector<std::string>& _values
    );

    void create_enum_button_bar_widget(
        QBoxLayout* _layout,
        const param_widget& _setup,
        const std::vector<enum_button_param>& _button_list,
        int _width,
        int _height,
        int _spacing,
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

    /// SLOT: This method is used to set an enum parameter.
    void set_enum_parameter(std::string _val, std::string _key);

    /// SLOT: This method sets an enum parameter using the index of the enum
    void set_enum_index_parameter(int /*val*/, std::string _key);

    /// Updates all enum values using a tokenized string ("value1;value2").
    void update_range(const std::string& _options, const std::string& _key);

    /// @}

    /// Updates the minimum value of the widget associated with the given key.
    void update_min(double _min, const std::string& _key);

    /// Updates the maximum value of the widget associated with the given key.
    void update_max(double _max, const std::string& _key);

    /// Updates the values of tickmarks widgets
    void update_tickmarks(
        sight::ui::qt::widget::tickmarks_slider* _tickmarks,
        const std::string& _options
    );

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
    requires std::derived_from<DATATYPE,
                               sight::data::generic<SUBTYPE> > sight::csptr<DATATYPE> data(const QObject* _widget);

    template<class DATATYPE, class SUBTYPE = DATATYPE::value_t>
    requires std::derived_from<DATATYPE, sight::data::generic<SUBTYPE> >
    void connect_data(const sight::csptr<DATATYPE>& _obj, const std::string& _key);

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
    data::ptr_vector<data::object, data::access::inout> m_settings {this, "item.data"};

    /// Optional tokenized values associated with the settings items by their XML occurrence.
    data::ptr_vector<data::string, data::access::in> m_values {this, "item.values", std::nullopt};

    /// Optional minimum bounds associated with the settings items by their XML occurrence.
    data::ptr_vector<data::real, data::access::in> m_min {this, "item.min", std::nullopt};

    /// Optional maximum bounds associated with the settings items by their XML occurrence.
    data::ptr_vector<data::real, data::access::in> m_max {this, "item.max", std::nullopt};

    using object_modified_t         = core::com::slot<void ()>;
    using settings_slot_container_t = std::map<std::string, sight::sptr<object_modified_t> >;
    settings_slot_container_t m_settings_slots;
    settings_slot_container_t m_values_slots;
    settings_slot_container_t m_min_slots;
    settings_slot_container_t m_max_slots;

    struct widget_joystick
    {
        sight::io::joystick::joystick_t  alias {sight::io::joystick::joystick_t::unknown};
        sight::io::joystick::axis_t axis_1 {sight::io::joystick::axis_t::unknown};
        sight::io::joystick::axis_t axis_2 {sight::io::joystick::axis_t::unknown};
        sight::io::joystick::axis_t axis_3 {sight::io::joystick::axis_t::unknown};
    };

    std::map<std::string, widget_joystick> m_widget_joysticks;
};

//------------------------------------------------------------------------------

} //namespace sight::module::ui::qt
