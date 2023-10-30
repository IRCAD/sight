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

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QStringList>
#include <QToolButton>

#include <functional>

namespace sight::ui::qt
{

//------------------------------------------------------------------------------

slice_selector::slice_selector(
    bool _display_axis_selector,
    bool _display_step_buttons,
    QWidget* const _parent
) noexcept :
    QWidget(_parent),
    m_slice_index(new QSlider(Qt::Horizontal, this)),
    m_p_slice_index_text(new QLineEdit(this))
{
    m_slice_index_style = new absolute_proxy_style(m_slice_index->style());
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

    m_p_slice_index_text->setReadOnly(true);
    m_p_slice_index_text->setMaximumWidth(80);

    layout->addWidget(m_slice_index, 1);
    layout->addWidget(m_p_slice_index_text, 0);

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
    QObject::disconnect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);

    if(!m_slice_type.isNull())
    {
        QObject::disconnect(m_slice_type, SIGNAL(currentIndexChanged(int)), this, SLOT(on_slice_type_change(int)));
    }
}

//------------------------------------------------------------------------------

void slice_selector::set_slice_range(int _min, int _max)
{
    QObject::disconnect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);
    this->m_slice_index->setRange(_min, _max);
    QObject::connect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);
}

//------------------------------------------------------------------------------

void slice_selector::set_slice_value(int _index)
{
    QObject::disconnect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);
    this->m_slice_index->setValue(_index);
    QObject::connect(m_slice_index, &QSlider::valueChanged, this, &slice_selector::on_slice_index_change);

    std::stringstream ss;
    ss << _index << " / " << this->m_slice_index->maximum();
    this->m_p_slice_index_text->setText(QString::fromStdString(ss.str()));
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
    m_p_slice_index_text->setEnabled(_enable);
}

//------------------------------------------------------------------------------
void slice_selector::on_slice_type_change(int _index)
{
    m_fct_change_type_callback(_index);
    this->set_slice_value(this->m_slice_index->value());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
