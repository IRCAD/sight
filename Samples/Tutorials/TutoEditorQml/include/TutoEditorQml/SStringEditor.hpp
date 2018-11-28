/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
