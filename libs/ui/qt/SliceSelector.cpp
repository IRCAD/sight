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

#include "ui/qt/SliceSelector.hpp"

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

SliceSelector::SliceSelector(bool displayAxisSelector, bool displayStepButtons, QWidget* const parent) noexcept :
    QWidget(parent),
    m_sliceIndex(new QSlider(Qt::Horizontal, this)),
    m_pSliceIndexText(new QLineEdit(this))
{
    m_sliceIndexStyle = new AbsoluteProxyStyle(m_sliceIndex->style());
    m_sliceIndex->setStyle(m_sliceIndexStyle);
    m_fctChangeIndexCallback = [this](int i){printIndex(i);};
    m_fctChangeTypeCallback  = [this](int t){printType(t);};

    auto* layout = new QHBoxLayout(this);
    if(displayAxisSelector)
    {
        /// Slice type names as a qt string array.
        QStringList sliceTypesArray;
        sliceTypesArray << tr("Sagittal") << tr("Frontal") << tr("Axial");

        m_sliceType = new QComboBox(this);
        m_sliceType->addItems(sliceTypesArray);
        layout->addWidget(m_sliceType, 0);
        QObject::connect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
    }

    m_pSliceIndexText->setReadOnly(true);
    m_pSliceIndexText->setMaximumWidth(80);

    layout->addWidget(m_sliceIndex, 1);
    layout->addWidget(m_pSliceIndexText, 0);

    if(displayStepButtons)
    {
        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* stepBackward = new QToolButton(this);
        auto path          = core::runtime::get_module_resource_path("sight::module::ui::flaticons");
        stepBackward->setIcon(QIcon(QString::fromStdString((path / "YellowBackwardStep.svg").string())));
        stepBackward->setToolTip(tr("Step backward"));

        QObject::connect(
            stepBackward,
            &QToolButton::clicked,
            [this]()
            {
                m_sliceIndex->setValue(
                    std::max(m_sliceIndex->minimum(), m_sliceIndex->value() - m_sliceIndex->singleStep())
                );
            });

        layout->addWidget(stepBackward, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));

        auto* stepForward = new QToolButton(this);
        stepForward->setIcon(QIcon(QString::fromStdString((path / "YellowForwardStep.svg").string())));
        stepBackward->setToolTip(tr("Step forward"));

        QObject::connect(
            stepForward,
            &QToolButton::clicked,
            [this]()
            {
                m_sliceIndex->setValue(
                    std::min(m_sliceIndex->maximum(), m_sliceIndex->value() + m_sliceIndex->singleStep())
                );
            });

        layout->addWidget(stepForward, 0);

        layout->addSpacerItem(new QSpacerItem(16, 0, QSizePolicy::Expanding, QSizePolicy::Minimum));
    }

    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    this->setLayout(layout);
}

//------------------------------------------------------------------------------

SliceSelector::~SliceSelector() noexcept
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    if(!m_sliceType.isNull())
    {
        QObject::disconnect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
    }
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceRange(int min, int max)
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setRange(min, max);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceValue(int index)
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setValue(index);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    std::stringstream ss;
    ss << index << " / " << this->m_sliceIndex->maximum();
    this->m_pSliceIndexText->setText(QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void SliceSelector::setTypeSelection(int type)
{
    if(!m_sliceType.isNull())
    {
        this->m_sliceType->setCurrentIndex(type);
    }
}

//------------------------------------------------------------------------------

void SliceSelector::onSliceIndexChange(int value) noexcept
{
    m_fctChangeIndexCallback(value);
    this->setSliceValue(value);
}

//------------------------------------------------------------------------------

void SliceSelector::setChangeIndexCallback(ChangeIndexCallback fct)
{
    m_fctChangeIndexCallback = fct;
}

//------------------------------------------------------------------------------

void SliceSelector::setChangeTypeCallback(ChangeTypeCallback fct)
{
    m_fctChangeTypeCallback = fct;
}

//------------------------------------------------------------------------------

void SliceSelector::printIndex(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void SliceSelector::printType(int /*unused*/)
{
}

//------------------------------------------------------------------------------

void SliceSelector::setEnable(bool enable)
{
    if(!m_sliceType.isNull())
    {
        m_sliceType->setEnabled(enable);
    }

    m_sliceIndex->setEnabled(enable);
    m_pSliceIndexText->setEnabled(enable);
}

//------------------------------------------------------------------------------
void SliceSelector::onSliceTypeChange(int index)
{
    m_fctChangeTypeCallback(index);
    this->setSliceValue(this->m_sliceIndex->value());
}

//------------------------------------------------------------------------------

} // namespace sight::ui::qt
