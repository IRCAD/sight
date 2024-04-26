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

#include "ui/qt/slice_selector.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <data/string.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSize>
#include <QSlider>
#include <QStringList>
#include <QToolButton>
#include <QWidget>

#include <functional>
#include <string>
#include <variant>
namespace sight::ui::qt
{

class slice_index_text : public QLineEdit
{
public:

    using QLineEdit::QLineEdit;

    //------------------------------------------------------------------------------

    void set_digits(std::uint8_t _digits)
    {
        m_digits = _digits;

        const auto& digits_string = std::string(_digits, '9');
        m_default_text = QString::fromStdString(digits_string + " / " + digits_string);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::uint8_t get_digits() const
    {
        return m_digits;
    }

protected:

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize minimumSizeHint() const override
    {
        // Don't forget to spare some margins
        const auto* const style = this->style() == nullptr ? QApplication::style() : this->style();
        const int right_margin  = style->pixelMetric(QStyle::PM_LayoutRightMargin, nullptr, this);
        const int left_margin   = style->pixelMetric(QStyle::PM_LayoutLeftMargin, nullptr, this);

        // Compute the optimal width for the text field.
        auto size = QLineEdit::minimumSizeHint();
        size.setWidth(this->fontMetrics().horizontalAdvance(m_default_text) + left_margin + right_margin);

        return size;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize sizeHint() const override
    {
        return minimumSizeHint();
    }

private:

    QString m_default_text;
    std::uint8_t m_digits {0};
};
//-----------------------------------------------------------------------------------

class slice_pos_text : public QLineEdit
{
public:

    using QLineEdit::QLineEdit;

    //------------------------------------------------------------------------------
    void set_digits_position(double _value)
    {
        std::ostringstream stream;
        stream << std::setw(24) << _value;
        std::string value_str = stream.str();

        const size_t dot_pos        = value_str.find('.');
        const size_t integer_digits = dot_pos != std::string::npos ? dot_pos : value_str.length();
        const size_t decimal_digits = dot_pos != std::string::npos ? value_str.length() - dot_pos - 1 : 0;

        const size_t total_digits = integer_digits + (decimal_digits > 0 ? 1 : 0) + decimal_digits;
        m_digits_p = static_cast<std::uint8_t>(total_digits > 255 ? 255 : total_digits);

        m_position_text = QString::fromStdString(value_str);
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] std::uint8_t get_position_digits() const
    {
        return m_digits_p;
    }

protected:

    //----------------------------------------------------------------- -------------

    [[nodiscard]] QSize minimumSizeHint() const override
    {
        // Don't forget to spare some margins
        const auto* const style = this->style() == nullptr ? QApplication::style() : this->style();
        const int right_margin  = style->pixelMetric(QStyle::PM_LayoutRightMargin, nullptr, this);
        const int left_margin   = style->pixelMetric(QStyle::PM_LayoutLeftMargin, nullptr, this);

        // Compute the optimal width for the text field.
        auto size = QLineEdit::minimumSizeHint();
        size.setWidth(this->fontMetrics().horizontalAdvance(m_position_text) + left_margin + right_margin);

        return size;
    }

    //------------------------------------------------------------------------------

    [[nodiscard]] QSize sizeHint() const override
    {
        return minimumSizeHint();
    }

private:

    QString m_position_text;
    std::uint8_t m_digits_p {0};
};

// This proxy style class provides a way to set slider positions in an absolute way
// which is very useful in general and especially for touchscreen input.
// See: https://stackoverflow.com/questions/11132597/qslider-mouse-direct-jump
class absolute_proxy_style : public QProxyStyle
{
public:

    using QProxyStyle::QProxyStyle;

    //------------------------------------------------------------------------------

    [[nodiscard]] int styleHint(
        QStyle::StyleHint _hint,
        const QStyleOption* _option    = nullptr,
        const QWidget* _widget         = nullptr,
        QStyleHintReturn* _return_data = nullptr
    ) const override
    {
        if(_hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return Qt::LeftButton | Qt::MiddleButton | Qt::RightButton;
        }

        return QProxyStyle::styleHint(_hint, _option, _widget, _return_data);
    }
};

class second_proxy_style : public QProxyStyle
{
public:

    using QProxyStyle::QProxyStyle;
    //------------------------------------------------------------------------------
    [[nodiscard]] int styleHint(
        QStyle::StyleHint _hint,
        const QStyleOption* _option    = nullptr,
        const QWidget* _widget         = nullptr,
        QStyleHintReturn* _return_data = nullptr
    ) const override
    {
        if(_hint == QStyle::SH_Slider_AbsoluteSetButtons)
        {
            return Qt::LeftButton | Qt::MiddleButton | Qt::RightButton;
        }

        return QProxyStyle::styleHint(_hint, _option, _widget, _return_data);
    }
};
//------------------------------------------------------------------------------
slice_selector::slice_selector(
    bool _display_axis_selector,
    bool _display_step_buttons,
    std::uint8_t _index_digits,
    QWidget* _parent
) noexcept :
    QWidget(_parent),
    m_slice_index_style(new absolute_proxy_style()),
    m_slice_index_text(new slice_index_text(this)),
    m_slice_index(new QSlider(Qt::Horizontal, this))
{
    m_slice_index->setStyle(m_slice_index_style);
    m_fct_change_index_callback = [](int){};
    m_fct_change_type_callback  = [](int){};

    auto* layout = new QHBoxLayout(this);

    if(_display_axis_selector)
    {
        /// Slice type names as a qt string array.
        QStringList slice_types_array;
        slice_types_array << tr("Sagittal") << tr("Frontal") << tr("Axial");

        m_slice_type = new QComboBox(this);
        m_slice_type->addItems(slice_types_array);
        layout->addWidget(m_slice_type, 0);
        QObject::connect(m_slice_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_slice_type_change(int)));
    }

    static_cast<slice_index_text*>(m_slice_index_text.data())->set_digits(_index_digits);

    m_slice_index_text->setReadOnly(true);
    m_slice_index_text->setAlignment(Qt::AlignVCenter | Qt::AlignRight | Qt::AlignAbsolute);

    layout->addWidget(m_slice_index, 1);
    layout->addWidget(m_slice_index_text, 0);

    if(_display_step_buttons)
    {
        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* step_backward = new QToolButton(this);
        auto path           = core::runtime::get_module_resource_path("sight::module::ui::icons");
        step_backward->setIcon(QIcon(QString::fromStdString((path / "left.svg").string())));
        step_backward->setToolTip(tr("Step backward"));

        QObject::connect(
            step_backward,
            &QToolButton::clicked,
            [this]()
            {
                m_slice_index->setValue(
                    std::max(m_slice_index->minimum(), m_slice_index->value() - m_slice_index->singleStep())
                );
            });

        layout->addWidget(step_backward, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* step_forward = new QToolButton(this);
        step_forward->setIcon(QIcon(QString::fromStdString((path / "right.svg").string())));
        step_backward->setToolTip(tr("Step forward"));

        QObject::connect(
            step_forward,
            &QToolButton::clicked,
            [this]()
            {
                m_slice_index->setValue(
                    std::min(m_slice_index->maximum(), m_slice_index->value() + m_slice_index->singleStep())
                );
            });

        layout->addWidget(step_forward, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);

    this->setLayout(layout);
}

//----------------------------------------------------------------------------------------------------------

slice_selector::slice_selector(
    bool _display_axis_selector_pos,
    bool _display_step_buttons_pos,
    double _pos_digits,
    QWidget* _parent_pos
) noexcept :
    QWidget(_parent_pos),
    m_slice_position_slider(new QSlider(Qt::Horizontal, this)),
    m_slice_position_text(new slice_pos_text(this)),
    m_slice_position_style(new second_proxy_style())
{
    auto* layout = new QHBoxLayout(this);

    m_slice_position_slider->setStyle(m_slice_position_style);

    m_fct_change_type_callback = [](int){};

    if(_display_axis_selector_pos)
    {
        /// Slice type names as a qt string array.
        QStringList slice_types_array;
        slice_types_array << tr("Sagittal") << tr("Frontal") << tr("Axial");

        m_slice_type = new QComboBox(this);
        m_slice_type->addItems(slice_types_array);
        layout->addWidget(m_slice_type, 0);
        QObject::connect(m_slice_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_slice_type_change(int)));
    }

    static_cast<slice_pos_text*>(m_slice_position_text.data())->set_digits_position(_pos_digits);
    m_slice_position_text->setReadOnly(true);
    m_slice_position_text->setAlignment(Qt::AlignVCenter | Qt::AlignRight | Qt::AlignAbsolute);

    layout->addWidget(m_slice_position_slider, 1);
    layout->addWidget(m_slice_position_text, 0);

    if(_display_step_buttons_pos)
    {
        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* step_backward_p = new QToolButton(this);
        auto path             = core::runtime::get_module_resource_path("sight::module::ui::icons");
        step_backward_p->setIcon(QIcon(QString::fromStdString((path / "left.svg").string())));
        step_backward_p->setToolTip(tr("Step backward"));

        QObject::connect(
            step_backward_p,
            &QToolButton::clicked,
            [this]()
            {
                m_slice_position_slider->setValue(
                    std::max(
                        m_slice_position_slider->minimum(),
                        m_slice_position_slider->value() - m_slice_position_slider->singleStep()
                    )
                );
            });

        layout->addWidget(step_backward_p, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* step_forward_p = new QToolButton(this);
        step_forward_p->setIcon(QIcon(QString::fromStdString((path / "right.svg").string())));
        step_backward_p->setToolTip(tr("Step forward"));

        QObject::connect(
            step_forward_p,
            &QToolButton::clicked,
            [this]()
            {
                m_slice_position_slider->setValue(
                    std::min(
                        m_slice_position_slider->maximum(),
                        m_slice_position_slider->value() + m_slice_position_slider->singleStep()
                    )
                );
            });

        layout->addWidget(step_forward_p, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    layout->setContentsMargins(0, 0, 0, 0);
    QObject::connect(m_slice_position_slider, &QSlider::valueChanged, this, &slice_selector::on_slice_position_change);
    this->setLayout(layout);
}

//------------------------------------------------------------------------------

slice_selector::~slice_selector() noexcept
{
    if(!m_slice_index_style.isNull())
    {
        m_slice_index_style->deleteLater();
    }
}

//------------------------------------------------------------------------------

void slice_selector::set_index_digits(std::uint8_t _index_digits)
{
    auto* slice_index_widget = static_cast<slice_index_text*>(m_slice_index_text.data());
    slice_index_widget->set_digits(_index_digits);
    slice_index_widget->updateGeometry();
}

//------------------------------------------------------------------------------
void slice_selector::set_position_digits(double _pos_digits)
{
    auto* slice_position_widget = static_cast<slice_pos_text*>(m_slice_position_text.data());
    slice_position_widget->set_digits_position(_pos_digits);
    slice_position_widget->updateGeometry();
}

//-------------------------------------------------------------------------------
void slice_selector::set_slice_range(int _min, int _max)
{
    m_slice_index->blockSignals(true);
    m_slice_index->setRange(_min, _max);
    m_slice_index->blockSignals(false);
}

//------------------------------------------------------------------------------

void slice_selector::set_slice_value(int _index)
{
    m_slice_index->blockSignals(true);
    m_slice_index->setValue(_index);
    m_slice_index->blockSignals(false);

    std::stringstream ss;
    ss << _index << " / " << this->m_slice_index->maximum();
    this->m_slice_index_text->setText(QString::fromStdString(ss.str()));
}

//--------------------------------------------------------------------------------
void slice_selector::set_prefix(const std::string& _orientation_prefix)
{
    this->m_orientation_prefix = _orientation_prefix;
}

//-------------------------------------------------------------------------------

void slice_selector::set_position_value(int _index)
{
    m_slice_position_slider->blockSignals(true);
    m_slice_position_slider->setValue(static_cast<int>(_index));
    m_slice_position_slider->blockSignals(false);
}

//---------------------------------------------------------------------------------------
void slice_selector::set_position_text(double _position_index)
{
    std::stringstream ss;
    ss << m_orientation_prefix << " : " << std::fixed << std::setprecision(2)
    << m_origin + (m_spacing * _position_index) << " mm";
    this->m_slice_position_text->setText(QString::fromStdString(ss.str()));
}

//---------------------------------------------------------------------------------------
void slice_selector::set_image_info(double _origin, double _spacing)
{
    m_origin = _origin;

    m_spacing = _spacing;
}

//------------------------------------------------------------------------------
void slice_selector::set_position_range(double _min, double _max)
{
    m_slice_position_slider->blockSignals(true);
    m_slice_position_slider->setRange(static_cast<int>(_min), static_cast<int>(_max));
    m_slice_position_slider->blockSignals(false);
}

//------------------------------------------------------------------------------

void slice_selector::set_type_selection(int _type)
{
    if(!m_slice_type.isNull())
    {
        this->m_slice_type->setCurrentIndex(_type);
    }
}

//------------------------------------------------------------------------------

void slice_selector::on_slice_index_change(int _value) noexcept
{
    m_fct_change_index_callback(_value);
    this->set_slice_value(_value);
}

//-----------------------------------------------------------------------------

void slice_selector::on_slice_position_change(int _value) noexcept
{
    m_fct_change_index_callback(_value);
    auto position = static_cast<double>(_value);
    this->set_position_text(position);
}

//------------------------------------------------------------------------------

void slice_selector::set_change_index_callback(ChangeIndexCallback _fct_index)
{
    m_fct_change_index_callback = _fct_index;
}

//------------------------------------------------------------------------------

void slice_selector::set_change_type_callback(ChangeTypeCallback _fct_type)
{
    m_fct_change_type_callback = _fct_type;
}

//------------------------------------------------------------------------------
void slice_selector::set_enable(bool _enable)
{
    if(!m_slice_type.isNull())
    {
        m_slice_type->setEnabled(_enable);
    }

    if(m_slice_index != nullptr)
    {
        m_slice_index->setEnabled(_enable);
    }

    if(m_slice_index_text != nullptr)
    {
        m_slice_index_text->setEnabled(_enable);
    }
}

//-----------------------------------------------------------------------------
void slice_selector::set_enabled(bool _enable)
{
    if(!m_slice_type.isNull())
    {
        m_slice_type->setEnabled(_enable);
    }

    if(m_slice_position_slider != nullptr)
    {
        m_slice_position_slider->setEnabled(_enable);
    }

    if(m_slice_position_text != nullptr)
    {
        m_slice_position_text->setEnabled(_enable);
    }
}

//------------------------------------------------------------------------------
void slice_selector::on_slice_type_change(int _index)
{
    m_fct_change_type_callback(_index);

    if(m_slice_index != nullptr)
    {
        this->set_slice_value(this->m_slice_index->value());
    }
    else if(m_slice_position_slider != nullptr)
    {
        this->set_position_value(this->m_slice_position_slider->value());
    }
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
