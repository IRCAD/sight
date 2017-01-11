/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __BEGINNERTRAINING_TUTO02_SSTRINGEDITOR_HPP__
#define __BEGINNERTRAINING_TUTO02_SSTRINGEDITOR_HPP__

#include "beginnerTraining/config.hpp"

#include <gui/editor/IEditor.hpp>

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
 * - \b string [::fwData::String]: string to display and to modify.
 */
class BEGINNERTRAINING_CLASS_API SStringEditor : public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SStringEditor)(::gui::editor::IEditor) );

    BEGINNERTRAINING_API SStringEditor();

    BEGINNERTRAINING_API virtual ~SStringEditor() throw();

protected:

    /// Overrides
    BEGINNERTRAINING_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() throw ( ::fwTools::Failed );

private:

    /// Text editor manage by the service
    QTextEdit* m_textEditor;
};

} // namespace tuto02

} // namespace beginnerTraining

#endif /*__BEGINNERTRAINING_TUTO02_SSTRINGEDITOR_HPP__*/

