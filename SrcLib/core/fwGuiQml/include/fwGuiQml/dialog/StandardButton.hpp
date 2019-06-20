/************************************************************************
 *
 * Copyright (C) 2019 IRCAD France
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

#include "fwGuiQml/config.hpp"

#include <QObject>
#include <QVector>

#include <string>

namespace fwGuiQml
{
namespace dialog
{

/**
 * @brief The enum to setup the button mask for StandardButtons
 */
class FWGUIQML_CLASS_API StandardButton : public QObject
{
Q_OBJECT
Q_PROPERTY(ButtonList button MEMBER m_button WRITE setButton READ getButton NOTIFY buttonChanged)
Q_ENUMS(StandardButton::ButtonList)

public:

    /// List of QmlButton
    enum ButtonList
    {
        NoButton        = 0x00000000,
        Ok              = 0x00000400,
        Save            = 0x00000800,
        SaveAll         = 0x00001000,
        Open            = 0x00002000,
        Yes             = 0x00004000,
        YesToAll        = 0x00008000,
        No              = 0x00010000,
        NoToAll         = 0x00020000,
        Abort           = 0x00040000,
        Retry           = 0x00080000,
        Ignore          = 0x00100000,
        Close           = 0x00200000,
        Cancel          = 0x00400000,
        Discard         = 0x00800000,
        Help            = 0x01000000,
        Apply           = 0x02000000,
        Reset           = 0x04000000,
        RestoreDefaults = 0x08000000
    };
    Q_ENUM(ButtonList)

    FWGUIQML_API StandardButton();

    FWGUIQML_API virtual ~StandardButton() override;

    /// Set the button mask
    FWGUIQML_API void setButton(ButtonList button);

    /// Get the button mask
    FWGUIQML_API const ButtonList& getButton();

Q_SIGNALS:
    /// notify the qml of property change
    void buttonChanged();

protected:

    /// Button mask
    ButtonList m_button;
};

} // namespace dialog
} // namespace fwGuiQml
