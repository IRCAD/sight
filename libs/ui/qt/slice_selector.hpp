/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2021 IHU Strasbourg
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

#include <sight/ui/qt/config.hpp>

#include <boost/function.hpp>

#include <QObject>
#include <QPointer>
#include <QProxyStyle>
#include <QWidget>

#include <functional>
#include <string>
#include <variant>

// Qt
class QSlider;
class QLineEdit;
class QComboBox;
class QStringList;

namespace sight::ui::qt
{

/**
 * @brief A Qt panel used to control a VTK 2D Negatoscope view.
 *
 */
class SIGHT_UI_QT_CLASS_API_QT slice_selector : public QWidget
{
Q_OBJECT

public:

    SIGHT_UI_QT_API_QT slice_selector(
        bool _display_axis_selector,
        bool _display_step_buttons,
        std::uint8_t _index_digits = 1,
        QWidget* _parent           = nullptr
    ) noexcept;

    SIGHT_UI_QT_API_QT slice_selector(
        bool _display_axis_selector_pos,
        bool _display_step_buttons_pos,
        double _pos_digits   = 0.0,
        QWidget* _parent_pos = nullptr
    ) noexcept;

    /// @brief Destructor.
    SIGHT_UI_QT_API_QT ~slice_selector() noexcept override;

    SIGHT_UI_QT_API_QT void set_index_digits(std::uint8_t _index_digits);

    SIGHT_UI_QT_API_QT void set_slice_range(int _min, int _max);

    SIGHT_UI_QT_API_QT void set_slice_value(int _index);

    SIGHT_UI_QT_API_QT void set_type_selection(int _type);

    SIGHT_UI_QT_API_QT void set_type_selection_pos(int _type_pos);

    SIGHT_UI_QT_API_QT void set_enable(bool _enable);

    SIGHT_UI_QT_API_QT void set_position_digits(double value);

    SIGHT_UI_QT_API_QT void set_position_value(int index);

    SIGHT_UI_QT_API_QT void set_position_text(double position_index);

    SIGHT_UI_QT_API_QT void set_position_range(double min, double max);

    SIGHT_UI_QT_API_QT void set_image_info(double origin, double spacing);

    SIGHT_UI_QT_API_QT void set_orientation(std::string& orientation);

    SIGHT_UI_QT_API_QT void set_enabled(bool _enable);

    SIGHT_UI_QT_API_QT void set_prefix(const std::string& _orientation_prefix);

    using ChangeIndexCallback = std::function<void (int)>;
    SIGHT_UI_QT_API_QT void set_change_index_callback(ChangeIndexCallback _fct_index);

    using ChangeTypeCallback = std::function<void (int)>;
    SIGHT_UI_QT_API_QT void set_change_type_callback(ChangeTypeCallback _fct_type);

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param _index index of the selected type item.
     */
    SIGHT_UI_QT_API_QT void on_slice_type_change(int _index);

    /**
     * @brief Event handler for a slice index change.
     * @param _value current value of the slice index slider.
     */
    SIGHT_UI_QT_API_QT void on_slice_index_change(int _value) noexcept;

    /**
     * @brief Event handler for a slice position change.
     * @param _value_f current value of the slice position slider.
     */
    SIGHT_UI_QT_API_QT void on_slice_position_change(int _value) noexcept;

private:

    QPointer<QComboBox> m_slice_type;

    /// @brief The slice index slider widget.
    QPointer<QStyle> m_slice_index_style;
    QPointer<QLineEdit> m_slice_index_text;
    QPointer<QSlider> m_slice_index;

    /// @brief The slice position slider widget.
    QPointer<QSlider> m_slice_position_slider;
    QPointer<QLineEdit> m_slice_position_text;
    QPointer<QStyle> m_slice_position_style;

    double m_origin {0.00};
    double m_spacing {0.00};
    std::string m_orientation_prefix;

    ChangeIndexCallback m_fct_change_index_callback;
    ChangeTypeCallback m_fct_change_type_callback;
};

} // namespace sight::ui::qt
