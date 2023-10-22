/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

#include "ui/dicom/widget/QHexSpinBox.hpp"

namespace sight::ui::dicom::widget
{

//-----------------------------------------------------------------------------

QHexSpinBox::QHexSpinBox(QWidget* _parent) :
    QSpinBox(_parent)
{
    this->setRange(0, 0xFFFF);
    m_validator = new QRegExpValidator(QRegExp("[0-9A-Fa-f]{1,4}"), this);
}

//-----------------------------------------------------------------------------

QValidator::State QHexSpinBox::validate(QString& _text, int& _pos) const
{
    return m_validator->validate(_text, _pos);
}

//-----------------------------------------------------------------------------

int QHexSpinBox::valueFromText(const QString& _text) const
{
    bool ok = false;
    return _text.toInt(&ok, 16);
}

//-----------------------------------------------------------------------------

QString QHexSpinBox::textFromValue(int _value) const
{
    QString res = QString::number(_value, 16).toUpper();
    for(int i = res.size() ; i < 4 ; ++i)
    {
        res = "0" + res;
    }

    return res;
}

} // namespace sight::ui::dicom::widget
