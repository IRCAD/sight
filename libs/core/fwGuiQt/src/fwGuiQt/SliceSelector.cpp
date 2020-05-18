/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#include "fwGuiQt/SliceSelector.hpp"

#include <fwCore/base.hpp>

#include <boost/lambda/lambda.hpp>

#include <QComboBox>
#include <QHBoxLayout>
#include <QLineEdit>
#include <QSlider>
#include <QStringList>

#include <functional>

namespace fwGuiQt
{

//------------------------------------------------------------------------------

SliceSelector::SliceSelector(QWidget* const parent ) noexcept :
    QWidget( parent )
{
    m_fctChangeIndexCallback = std::bind( &::fwGuiQt::SliceSelector::printIndex, this, std::placeholders:: _1);
    m_fctChangeTypeCallback  = std::bind( &::fwGuiQt::SliceSelector::printType, this, std::placeholders:: _1);

    m_sliceType = new QComboBox( this );
    /// Slice type names as a qt string array.
    QStringList sliceTypesArray;
    sliceTypesArray << tr("Sagittal") << tr("Frontal") << tr("Axial");
    m_sliceType->addItems(sliceTypesArray);

    m_sliceIndex = new QSlider(Qt::Horizontal, this);

    m_pSliceIndexText = new QLineEdit(this);
    m_pSliceIndexText->setReadOnly(true);
    m_pSliceIndexText->setMaximumWidth(80);

    QHBoxLayout* layout = new QHBoxLayout(this);
    layout->addWidget(m_sliceType, 0);
    layout->addWidget(m_sliceIndex, 1);
    layout->addWidget(m_pSliceIndexText, 0);
    layout->setContentsMargins(0, 0, 0, 0);

    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    QObject::connect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));

    this->setLayout( layout );
}

//------------------------------------------------------------------------------

SliceSelector::~SliceSelector() noexcept
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    QObject::disconnect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceRange( int min, int max )
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setRange(min, max);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceValue( int index )
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    this->m_sliceIndex->setValue(index);
    QObject::connect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));

    std::stringstream ss;
    ss << index << " / " << this->m_sliceIndex->maximum();
    this->m_pSliceIndexText->setText( QString::fromStdString(ss.str()));
}

//------------------------------------------------------------------------------

void SliceSelector::setTypeSelection( int type )
{
    this->m_sliceType->setCurrentIndex(type);
}

//------------------------------------------------------------------------------

void SliceSelector::onSliceIndexChange( int value ) noexcept
{
    SLM_TRACE_FUNC();
    m_fctChangeIndexCallback( value );
    this->setSliceValue( value );
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

void SliceSelector::printIndex(int index)
{
    OSLM_TRACE("index= "<<index);
}

//------------------------------------------------------------------------------

void SliceSelector::printType(int type)
{
    OSLM_TRACE("type= "<<type);
}

//------------------------------------------------------------------------------

void SliceSelector::setEnable(bool enable)
{
    m_sliceType->setEnabled(enable);
    m_sliceIndex->setEnabled(enable);
    m_pSliceIndexText->setEnabled(enable);
}

//------------------------------------------------------------------------------
void SliceSelector::onSliceTypeChange( int index )
{
    m_fctChangeTypeCallback(index);
    this->setSliceValue( this->m_sliceIndex->value());
}

//------------------------------------------------------------------------------

} // fwGuiQt

