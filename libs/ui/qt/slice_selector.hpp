/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/qt/config.hpp"

#include <boost/function.hpp>

#include <QObject>
#include <QPointer>
#include <QProxyStyle>
#include <QWidget>

#include <functional>

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
class UI_QT_CLASS_API_QT slice_selector : public QWidget
{
Q_OBJECT

public:

    /**
     * @brief Constructor.
     * @param parent The parent widget.
     */
    UI_QT_API_QT slice_selector(
        bool _display_axis_selector,
        bool _display_step_buttons,
        std::uint8_t _index_digits = 1,
        QWidget* _parent           = nullptr
    ) noexcept;

    /// @brief Destructor.
    UI_QT_API_QT ~slice_selector() noexcept override;

    UI_QT_API_QT void set_index_digits(std::uint8_t _index_digits);

    UI_QT_API_QT void set_slice_range(int _min, int _max);

    UI_QT_API_QT void set_slice_value(int _index);

    UI_QT_API_QT void set_type_selection(int _type);

    UI_QT_API_QT void set_enable(bool _enable);

    using ChangeIndexCallback = std::function<void (int)>;
    UI_QT_API_QT void set_change_index_callback(ChangeIndexCallback _fct);

    using ChangeTypeCallback = std::function<void (int)>;
    UI_QT_API_QT void set_change_type_callback(ChangeTypeCallback _fct);

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param index index of the selected type item.
     */
    UI_QT_API_QT void on_slice_type_change(int _index);

    /**
     * @brief Event handler for a slice index change.
     * @param value current value of the slice index slider.
     */
    UI_QT_API_QT void on_slice_index_change(int _value) noexcept;

private:

    QPointer<QComboBox> m_slice_type;

    /// @brief The slice index slider widget.
    QPointer<QStyle> m_slice_index_style;
    QPointer<QSlider> m_slice_index;
    QPointer<QLineEdit> m_slice_index_text;

    void print_index(int _index);
    void print_type(int _type);

    ChangeIndexCallback m_fct_change_index_callback;
    ChangeTypeCallback m_fct_change_type_callback;
};

} // namespace sight::ui::qt
