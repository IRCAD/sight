/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <data/image.hpp>
#include <data/integer.hpp>
#include <data/transfer_function.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

class QAction;
class QComboBox;
class QLabel;
class QLineEdit;
class QMenu;
class QSlider;
class QToolButton;

namespace sight::ui::qt::widget
{

class range_slider;

} // namespace sight::ui::qt::widget

namespace sight::module::ui::qt::image
{

/**
 * @brief window_level service allows to change the min/max value of windowing.
 *
 * This is represented by two sliders to modify the min and max values of windowing.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="..." type="sight::module::ui::qt::image::window_level" auto_connect="true">
        <in key="image" uid="..."/>
        <inout key="tf" uid="..." />
        <config autoWindowing="true" enableSquareTF="false" />
        <properties piece="..." />
    </service>
   @endcode
 *
 * @subsection Input Input
 * - \b image [sight::data::image]: image on which the windowing will be changed.
 *
 * @subsection In-Out In-Out
 * - \b tf [sight::data::transfer_function] (optional): the current TransferFunction. If it is not defined, we use the
 *      image's default transferFunction (CT-GreyLevel).
 *
 * @subsection Configuration Configuration
 * - \b minimal(optional, default="false"): if 'true', only the windowing range slider is shown
 * - \b autoWindowing(optional, default="false"): if 'true', image windowing will be automatically compute from image
 * pixel
 * min/max intensity when this service receive BUFFER event.
 * - \b enableSquareTF(optional, default="true"): if 'true', enables the button to switch between current TF and square
 * TF.
 *
 * @subsection Properties Properties
 * - \b piece(optional, default="-1"): if >=0, restrict all settings made to the configured piece in the TF
 */
class window_level final : public QObject,
                           public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(window_level, sight::ui::editor);

    struct slots
    {
        static inline const core::com::slots::key_t UPDATE_IMAGE = "update_image";
    };

    /// Initialize signals and slots.
    window_level() noexcept;

    /// Destroys the service.
    ~window_level() noexcept final = default;

protected:

    /// Configures the service.
    void configuring() final;

    /// Installs the layout.
    void starting() final;

    /// Updates slider position
    void updating() final;

    /// Destroys the layout.
    void stopping() final;

    /**
     * @brief Proposals to connect service slots to associated object signals.
     * @return A map of each proposed connection.
     *
     * Connect data::image::MODIFIED_SIG to module::ui::qt::image::window_level::service::slots::UPDATE
     * Connect image::BUFFER_MODIFIED_SIG to module::ui::qt::image::window_level::service::slots::UPDATE
     */
    connections_t auto_connections() const final;

    /**
     * @brief Adds informations about this service into the stream.
     * @param _sstream Stream where stores information.
     */
    void info(std::ostream& _sstream) final;

    /// Slot: Updates the slider range
    void update_image();

protected Q_SLOTS:

    void on_text_editing_finished();

    void on_toggle_tf(bool _square_tf);

    void on_toggle_auto_wl(bool _auto_wl);

    void on_window_level_widget_changed(double _min, double _max);

    void on_dynamic_range_selection_changed(QAction* _action);

protected:

    double to_window_level(double _val) const;

    double from_window_level(double _val);

    void on_image_window_level_changed(double _image_min, double _image_max);

    void update_widget_min_max(double _image_min, double _image_max);

    void update_image_window_level(double _image_min, double _image_max);

    void update_text_window_level(double _image_min, double _image_max);

    static bool get_widget_double_value(QLineEdit* _widget, double& _val);

    void set_widget_dynamic_range(double _min, double _max);

private:

    QPointer<QLineEdit> m_value_text_min;
    QPointer<QLineEdit> m_value_text_max;
    QPointer<QToolButton> m_toggle_tf_button;
    QPointer<QToolButton> m_toggle_auto_button;
    QPointer<QToolButton> m_dynamic_range_selection;
    QPointer<QMenu> m_dynamic_range_menu;

    QPointer<sight::ui::qt::widget::range_slider> m_range_slider;

    double m_widget_dynamic_range_min {-1024};
    double m_widget_dynamic_range_width {4000};
    bool m_minimal {false};
    bool m_auto_windowing {false};
    bool m_enable_square_tf {true};

    /// Store previous TF, used in onToggleTF() to restore this TF when switching to the square TF
    data::transfer_function::sptr m_previous_tf;

    static constexpr std::string_view IMAGE = "image";
    static constexpr std::string_view TF    = "tf";

    data::ptr<data::image, data::access::in> m_image {this, IMAGE};
    data::ptr<data::transfer_function, data::access::inout> m_tf {this, TF};

    sight::data::property<sight::data::integer> m_piece {this, "piece", -1};
};

} // namespace sight::module::ui::qt::image
