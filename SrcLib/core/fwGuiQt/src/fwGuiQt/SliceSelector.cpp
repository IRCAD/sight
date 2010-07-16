/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <QComboBox>
#include <QStringList>
#include <QSlider>
#include <QLineEdit>
#include <QHBoxLayout>

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwCore/base.hpp>

#include "fwGuiQt/SliceSelector.hpp"

namespace fwGuiQt
{

//------------------------------------------------------------------------------

const QStringList SliceSelector::m_sliceTypesArray = (QStringList() << tr("Sagittal") << tr("Frontal") << tr("Axial"));

//------------------------------------------------------------------------------

SliceSelector::SliceSelector(QWidget* const parent ) throw(): QWidget( parent )
{
    m_fctChangeIndexCallback = ::boost::bind( &::fwGuiQt::SliceSelector::printIndex, this, _1);
    m_fctChangeTypeCallback = ::boost::bind( &::fwGuiQt::SliceSelector::printType, this, _1);

    m_sliceType = new QComboBox( this );
    m_sliceType->addItems(m_sliceTypesArray);

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

SliceSelector::~SliceSelector() throw()
{
    QObject::disconnect(m_sliceIndex, SIGNAL(valueChanged(int)), this, SLOT(onSliceIndexChange(int)));
    QObject::disconnect(m_sliceType, SIGNAL(currentIndexChanged(int)), this, SLOT(onSliceTypeChange(int)));
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceRange( int min, int max )
{
    this->m_sliceIndex->setRange(min, max);
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceValue( int index )
{
    this->m_sliceIndex->setValue(index);

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

void SliceSelector::onSliceIndexChange( int value ) throw()
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

