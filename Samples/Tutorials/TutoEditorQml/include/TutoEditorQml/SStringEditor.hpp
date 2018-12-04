/************************************************************************
 *
 * Copyright (C) 2018 IRCAD France
 * Copyright (C) 2018 IHU Strasbourg
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

#include "TutoEditorQml/config.hpp"

#include <fwQml/IQmlEditor.hpp>

#include <QObject>

namespace TutoEditorQml
{

/**
 * @brief   This class is started when the bundles is loaded.
 */
class TUTOEDITORQML_CLASS_API SStringEditor : public ::fwQml::IQmlEditor
{

Q_OBJECT;
public:

    fwCoreServiceClassDefinitionsMacro( (SStringEditor)(::fwQml::IQmlEditor) )

    /// Constructor.
    TUTOEDITORQML_API SStringEditor() noexcept;

    /// Destructor. Do nothing.
    TUTOEDITORQML_API ~SStringEditor() noexcept;

Q_SIGNALS:
    void edited(const QString& str);

protected:

    /// Do nothing
    virtual void configuring() override;

    /// Call the IQmlEditor::starting method.
    virtual void starting() override;

    /// Call the IQmlEditor::stopping method.
    virtual void stopping() override;

    /// Update the displayed string
    virtual void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect String::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    virtual KeyConnectionsMap getAutoConnections() const override;

protected Q_SLOTS:

    /// Qt slot: update the data with the current string
    void updateString(const QString& str);

private:
    QString m_value;
};

} // namespace TutoEditorQml
