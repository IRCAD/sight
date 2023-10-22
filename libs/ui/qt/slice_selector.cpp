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
    m_sliceIndex(new QSlider(Qt::Horizontal, this)),
    m_pSliceIndexText(new QLineEdit(this))
{
    m_sliceIndexStyle = new AbsoluteProxyStyle(m_sliceIndex->style());
    m_sliceIndex->setStyle(m_sliceIndexStyle);
    m_fctChangeIndexCallback = [this](int _i){printIndex(_i);};
    m_fctChangeTypeCallback  = [this](int _t){printType(_t);};

    auto* layout = new QHBoxLayout(this);
    if(_display_axis_selector)
    {
        /// Slice type names as a qt string array.
        QStringList slice_types_array;
        slice_types_array << tr("Sagittal") << tr("Frontal") << tr("Axial");

        m_sliceType = new QComboBox(this);
        m_sliceType->addItems(slice_types_array);
        layout->addWidget(m_sliceType, 0);
        QObject::connect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
    }

    m_pSliceIndexText->setReadOnly(true);
    m_pSliceIndexText->setMaximumWidth(80);

    layout->addWidget(m_sliceIndex, 1);
    layout->addWidget(m_pSliceIndexText, 0);

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
                m_sliceIndex->setValue(
                    std::max(m_sliceIndex->minimum(), m_sliceIndex->value() - m_sliceIndex->singleStep())
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
                m_sliceIndex->setValue(
                    std::min(m_sliceIndex->maximum(), m_sliceIndex->value() + m_sliceIndex->singleStep())
                );
            });

        layout->addWidget(step_forward, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    this->setLayout(layout);
}

//------------------------------------------------------------------------------

slice_selector::~slice_selector() noexcept
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    if(!m_sliceType.isNull())
    {
        QObject::disconnect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
    }
}

//------------------------------------------------------------------------------

void slice_selector::setSliceRange(int _min, int _max)
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setRange(_min, _max);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
}

//------------------------------------------------------------------------------

void slice_selector::setSliceValue(int _index)
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setValue(_index);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    std::stringstream ss;
    ss << _index << " / " << this->m_sliceIndex->maximum();
    this->m_pSliceIndexText->setText(QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void slice_selector::setTypeSelection(int _type)
{
    if(!m_sliceType.isNull())
    {
        this->m_sliceType->setCurrentIndex(_type);
    }
}

//------------------------------------------------------------------------------

void slice_selector::onSliceIndexChange(int _value) noexcept
{
    m_fctChangeIndexCallback(_value);
    this->setSliceValue(_value);
}

//------------------------------------------------------------------------------

void slice_selector::setChangeIndexCallback(ChangeIndexCallback _fct)
{
    m_fctChangeIndexCallback = _fct;
}

//------------------------------------------------------------------------------

void slice_selector::setChangeTypeCallback(ChangeTypeCallback _fct)
{
    m_fctChangeTypeCallback = _fct;
}

//------------------------------------------------------------------------------

void slice_selector::printIndex(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void slice_selector::printType(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void slice_selector::setEnable(bool _enable)
{
    if(!m_sliceType.isNull())
    {
        m_sliceType->setEnabled(_enable);
    }

    m_sliceIndex->setEnabled(_enable);
    m_pSliceIndexText->setEnabled(_enable);
}

//------------------------------------------------------------------------------
void slice_selector::onSliceTypeChange(int _index)
{
    m_fctChangeTypeCallback(_index);
    this->setSliceValue(this->m_sliceIndex->value());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
