/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "beginnerTraining/config.hpp"

#include <fwGui/editor/IEditor.hpp>

#include <qtextedit.h>

namespace beginnerTraining
{
namespace tuto02
{

/**
 * @brief   Editor that display and modify the content of a ::fwData::String.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::beginnerTraining::tuto02::SStringEditor">
            <inout key="editString" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b editString [::fwData::String]: string to display and to modify.
 */
class BEGINNERTRAINING_CLASS_API SStringEditor : public ::fwGui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SStringEditor)(::fwGui::editor::IEditor) );

    BEGINNERTRAINING_API SStringEditor();

    BEGINNERTRAINING_API virtual ~SStringEditor() noexcept;

protected:

    /// Overrides
    BEGINNERTRAINING_API virtual void configuring() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() override;

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() override;

private:

    /// Text editor manage by the service
    QTextEdit* m_textEditor;
};

} // namespace tuto02

} // namespace beginnerTraining
