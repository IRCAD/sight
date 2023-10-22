/************************************************************************
 *
 * Copyright (C) 2020-2023 IRCAD France
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

#include <ui/__/dialog/selector_base.hpp>

#include <QObject>
#include <QVariant>
#include <QVector>

#include <vector>

namespace sight::ui::qml::dialog
{

//------------------------------------------------------------------------------

/**
 * @brief   selector allowing the choice of an element among severals (_selections)
 */
class UI_QML_CLASS_API selector : public QObject,
                                  public ui::dialog::selector_base
{
Q_OBJECT
Q_PROPERTY(QString message MEMBER m_message NOTIFY messageChanged)

public:

    SIGHT_DECLARE_CLASS(selector, ui::dialog::selector_base, ui::factory::make<selector>);

    UI_QML_API selector()           = default;
    UI_QML_API ~selector() override = default;

    /// The string list that can be chosen by the selector.
    UI_QML_API void set_choices_preset(choices_preset_t _selections) override;

    /// Sets the selector title.
    UI_QML_API void setTitle(std::string _title) override;

    /// Allows multiple selections (default = false).
    UI_QML_API void set_multiple(bool _multiple) override;

    /// Show the selector and return the selection.
    UI_QML_API selections_t show() override;

    /// Set the message
    UI_QML_API void setMessage(const std::string& _msg) override;

    /// unimplemented
    UI_QML_API void addCustomButton(const std::string& _label, std::function<void()> _clicked_fn) override;

Q_SIGNALS:

    /// notify the qml of property change
    void messageChanged();

protected Q_SLOTS:

    /// slot getting the result of the dialog when a button is pressed
    void resultDialog(QVariant _selection);

private:

    choices_preset_t m_choices;

    /// Dialog box message
    QString m_message;
    /// Dialog box title
    QString m_title;
    /// Dialog box selected RadioButton
    QString m_selection;
    /// Allows multiple selection
    bool m_multiple {false};
};

} // namespace sight::ui::qml::dialog
