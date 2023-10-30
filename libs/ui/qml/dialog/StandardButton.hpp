/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
 * Copyright (C) 2019 IHU Strasbourg
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

#include "ui/qml/config.hpp"

#include <QObject>
#include <QVector>

#include <string>

namespace sight::ui::qml::dialog
{

/**
 * @brief The enum to setup the button mask for StandardButtons
 */
class UI_QML_CLASS_API standard_button : public QObject
{
Q_OBJECT
Q_PROPERTY(button_list button MEMBER m_button WRITE set_button READ get_button NOTIFY button_changed)
Q_ENUMS(standard_button::ButtonList)

public:

    /// List of QmlButton
    enum button_list
    {
        no_button        = 0x00000000,
        ok               = 0x00000400,
        save             = 0x00000800,
        save_all         = 0x00001000,
        open             = 0x00002000,
        yes              = 0x00004000,
        yes_to_all       = 0x00008000,
        no               = 0x00010000,
        no_to_all        = 0x00020000,
        abort            = 0x00040000,
        retry            = 0x00080000,
        ignore           = 0x00100000,
        close            = 0x00200000,
        cancel           = 0x00400000,
        discard          = 0x00800000,
        help             = 0x01000000,
        apply            = 0x02000000,
        reset            = 0x04000000,
        restore_defaults = 0x08000000
    };
    Q_ENUM(button_list)

    UI_QML_API standard_button();

    UI_QML_API ~standard_button() override;

    /// Set the button mask
    UI_QML_API void set_button(button_list _button);

    /// Get the button mask
    UI_QML_API const button_list& get_button();

Q_SIGNALS:

    /// notify the qml of property change
    void button_changed();

private:

    /// Button mask
    button_list m_button {button_list::no_button};
};

} // namespace sight::ui::qml::dialog
