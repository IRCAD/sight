/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwGuiQt/SliceSelector.hpp"

#include <fwCore/base.hpp>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>
#include <functional>
#include <QComboBox>
#include <QStringList>
#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>

namespace fwGuiQt
{

//------------------------------------------------------------------------------

SliceSelector::SliceSelector(QWidget* const parent ) noexcept : QWidget( parent )
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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->addWidget(m_sliceType, 0);
    layout->addWidget(m_sliceIndex, 1);
    layout->addWidget(m_pSliceIndexText, 0);
    layout->setContentsMargins(0,0,0,0);

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

