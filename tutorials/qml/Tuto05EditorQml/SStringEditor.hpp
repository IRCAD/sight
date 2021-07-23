/************************************************************************
 *
 * Copyright (C) 2018-2021 IRCAD France
 * Copyright (C) 2018-2020 IHU Strasbourg
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

#include "Tuto05EditorQml/config.hpp"

#include <data/String.hpp>

#include <ui/qml/IQmlEditor.hpp>

#include <QObject>

namespace Tuto05EditorQml
{

/**
 * @brief   This class is started when the module is loaded.
 */
class TUTO05EDITORQML_CLASS_API SStringEditor : public sight::ui::qml::IQmlEditor
{
Q_OBJECT;

public:

    SIGHT_DECLARE_SERVICE(SStringEditor, ::sight::ui::qml::IQmlEditor);

    /// Constructor.
    TUTO05EDITORQML_API SStringEditor() noexcept;

    /// Destructor. Do nothing.
    TUTO05EDITORQML_API ~SStringEditor() noexcept;

Q_SIGNALS:

    void edited(const QString& str);

protected:

    /// Do nothing
    void configuring() override;

    /// Call the IQmlEditor::starting method.
    void starting() override;

    /// Call the IQmlEditor::stopping method.
    void stopping() override;

    /// Update the displayed string
    void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect String::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    KeyConnectionsMap getAutoConnections() const override;

protected Q_SLOTS:

    /// Qt slot: update the data with the current string
    void updateString(const QString& str);

private:

    QString m_value;

    static const sight::service::key_t s_STRING_INOUT;
    sight::data::ptr<sight::data::String, sight::data::Access::inout> m_string {this, s_STRING_INOUT, true};
};

} // namespace Tuto05EditorQml
