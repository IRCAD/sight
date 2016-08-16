/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomIOFilterQt/widget/QHexSpinBox.hpp"

namespace fwDicomIOFilterQt
{
namespace widget
{


//-----------------------------------------------------------------------------

QHexSpinBox::QHexSpinBox(QWidget* parent) : QSpinBox(parent)
{
    this->setRange(0,0xFFFF);
    m_validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,4}"), this);
}

//-----------------------------------------------------------------------------

QValidator::State QHexSpinBox::validate(QString& text, int& pos) const
{
    return m_validator->validate(text, pos);
}

//-----------------------------------------------------------------------------

int QHexSpinBox::valueFromText(const QString& text) const
{
    bool ok;
    return text.toInt(&ok, 16);
}

//-----------------------------------------------------------------------------

QString QHexSpinBox::textFromValue(int value) const
{
    QString res = QString::number(value, 16).toUpper();
    for(unsigned int i = res.size(); i < 4; ++i)
    {
        res = "0" + res;
    }
    return res;
}

} // namespace widget
} // namespace fwDicomIOFilterQt
