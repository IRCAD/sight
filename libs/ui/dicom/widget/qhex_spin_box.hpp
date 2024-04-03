/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
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

#pragma once

#include <sight/ui/dicom/config.hpp>

#include <QRegExpValidator>
#include <QSpinBox>
#include <QWidget>

namespace sight::ui::dicom::widget
{

/**
 * @brief QSpinBox for hexadecimal values
 */
class q_hex_spin_box : public QSpinBox
{
Q_OBJECT;

public:

    /// Constructor
    q_hex_spin_box(QWidget* _parent = nullptr);

protected:

    /// Override
    QValidator::State validate(QString& _text, int& _pos) const override;

    /// Override
    [[nodiscard]] int valueFromText(const QString& _text) const override;

    /// Override
    [[nodiscard]] QString textFromValue(int _value) const override;

    QRegExpValidator* m_validator;
};

} // namespace sight::ui::dicom::widget
