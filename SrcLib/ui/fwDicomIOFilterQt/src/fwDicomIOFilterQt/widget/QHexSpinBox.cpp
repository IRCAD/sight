/************************************************************************
 *
 * Copyright (C) 2009-2016 IRCAD France
 * Copyright (C) 2012-2016 IHU Strasbourg
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
