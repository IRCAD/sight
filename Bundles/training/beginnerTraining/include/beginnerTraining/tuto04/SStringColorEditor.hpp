/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// Qt objects
#pragma once

#include "beginnerTraining/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <QPointer>
#include <qtextedit.h>

namespace beginnerTraining
{
namespace tuto04
{

/**
 * @brief   Editor that display and modify the content of a ::fwData::String.

 * @section Slots Slots
 * - \b changeColor(std::array<std::uint8_t, 4>, std::string) : .
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::beginnerTraining::tuto04::SStringColorEditor">
            <inout key="editString" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b editString [::fwData::String]: string to display and to modify.
 */
class BEGINNERTRAINING_CLASS_API SStringColorEditor : public QObject,
                                                      public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (SStringColorEditor)(::fwGui::editor::IEditor) );

    static const ::fwCom::Slots::SlotKeyType s_CHANGE_COLOR_SLOT;

    BEGINNERTRAINING_API SStringColorEditor();
    BEGINNERTRAINING_API virtual ~SStringColorEditor() noexcept;

protected Q_SLOTS:

    BEGINNERTRAINING_API void onTextChanged();

protected:

    /// Overrides
    BEGINNERTRAINING_API virtual void configuring() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect String::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    BEGINNERTRAINING_API KeyConnectionsMap getAutoConnections() const override;

    /// Method to notify modification on data
    BEGINNERTRAINING_API void notifyMessage();

private:

    /// Slot: Modify one of the color attributes of the editor
    void changeColor(std::array<std::uint8_t, 4> color, std::string key);

    /// Text editor manage by the service
    QPointer<QTextEdit> m_textEditor;
};

} // namespace tuto04
} // namespace beginnerTraining
