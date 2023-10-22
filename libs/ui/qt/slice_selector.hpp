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

// This proxy style class provides a way to set slider positions in an absolute way
// which is very useful in general and especially for touchscreen input.
// See: https://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
class AbsoluteProxyStyle : public QProxyStyle
{
public:

    using QProxyStyle::QProxyStyle;

    //------------------------------------------------------------------------------

    int styleHint(
        QStyle::StyleHint _hint,
        const QStyleOption* _option    = 0,
        const QWidget* _widget         = 0,
        QStyleHintReturn* _return_data = 0
    ) const
    {
        if(_hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return Qt::LeftButton | Qt::MiddleButton | Qt::RightButton;
        }

        return QProxyStyle::styleHint(_hint, _option, _widget, _return_data);
    }
};

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
        QWidget* _parent = nullptr
    ) noexcept;

    /// @brief Destructor.
    UI_QT_API_QT ~slice_selector() noexcept override;

    UI_QT_API_QT void setSliceRange(int _min, int _max);

    UI_QT_API_QT void setSliceValue(int _index);

    UI_QT_API_QT void setTypeSelection(int _type);

    UI_QT_API_QT void setEnable(bool _enable);

    typedef std::function<void (int)> ChangeIndexCallback;
    UI_QT_API_QT void setChangeIndexCallback(ChangeIndexCallback _fct);

    typedef std::function<void (int)> ChangeTypeCallback;
    UI_QT_API_QT void setChangeTypeCallback(ChangeTypeCallback _fct);

protected Q_SLOTS:

    /**
     * @brief Event handler for a slice type change.
     * @param index index of the selected type item.
     */
    UI_QT_API_QT void onSliceTypeChange(int _index);

    /**
     * @brief Event handler for a slice index change.
     * @param value current value of the slice index slider.
     */
    UI_QT_API_QT void onSliceIndexChange(int _value) noexcept;

private:

    QPointer<QComboBox> m_sliceType;

    /// @brief The slice index slider widget.
    QPointer<QStyle> m_sliceIndexStyle;
    QPointer<QSlider> m_sliceIndex;
    QPointer<QLineEdit> m_pSliceIndexText;

    void printIndex(int _index);
    void printType(int _type);

    ChangeIndexCallback m_fctChangeIndexCallback;
    ChangeTypeCallback m_fctChangeTypeCallback;
};

} // namespace sight::ui::qt
