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

#include "ui/qt/slice_selector.hpp"

#include <core/base.hpp>
#include <core/runtime/path.hpp>

#include <QApplication>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSize>
#include <QSlider>
#include <QStringList>
#include <QToolButton>

#include <functional>

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

//------------------------------------------------------------------------------

slice_selector::slice_selector(
    bool _display_axis_selector,
    bool _display_step_buttons,
    std::uint8_t _index_digits,
    QWidget* const _parent
) noexcept :
    QWidget(_parent),
    m_slice_index_style(new absolute_proxy_style()),
    m_slice_index(new QSlider(Qt::Horizontal, this)),
    m_slice_index_text(new slice_index_text(this))
{
    m_slice_index->setStyle(m_slice_index_style);

    m_fct_change_index_callback = [this](int _i){print_index(_i);};
    m_fct_change_type_callback  = [this](int _t){print_type(_t);};

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
        auto path           = core::runtime::get_module_resource_path("sight::module::ui::flaticons");
        step_backward->setIcon(QIcon(QString::fromStdString((path / "YellowBackwardStep.svg").string())));
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
        step_forward->setIcon(QIcon(QString::fromStdString((path / "YellowForwardStep.svg").string())));
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

//------------------------------------------------------------------------------

void slice_selector::set_change_index_callback(ChangeIndexCallback _fct)
{
    m_fct_change_index_callback = _fct;
}

//------------------------------------------------------------------------------

void slice_selector::set_change_type_callback(ChangeTypeCallback _fct)
{
    m_fct_change_type_callback = _fct;
}

//------------------------------------------------------------------------------

void slice_selector::print_index(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void slice_selector::print_type(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void slice_selector::set_enable(bool _enable)
{
    if(!m_slice_type.isNull())
    {
        m_slice_type->setEnabled(_enable);
    }

    m_slice_index->setEnabled(_enable);
    m_slice_index_text->setEnabled(_enable);
}

//------------------------------------------------------------------------------
void slice_selector::on_slice_type_change(int _index)
{
    m_fct_change_type_callback(_index);
    this->set_slice_value(this->m_slice_index->value());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
