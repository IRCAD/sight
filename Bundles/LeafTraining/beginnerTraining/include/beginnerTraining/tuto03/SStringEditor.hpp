/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP_
#define _BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP_

// Qt objects
#include <qtextedit.h>

#include <gui/editor/IEditor.hpp>

#include "beginnerTraining/config.hpp"

namespace beginnerTraining
{
namespace tuto03
{

class BEGINNERTRAINING_CLASS_API SStringEditor : public ::gui::editor::IEditor
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SStringEditor)(::gui::editor::IEditor) ) ;

    /// Overrides
    BEGINNERTRAINING_API virtual void configuring() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void starting() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void stopping() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating() throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw ( ::fwTools::Failed );

    /// Overrides
    BEGINNERTRAINING_API virtual void swapping() throw ( ::fwTools::Failed );

protected :

    /// Constructor
    BEGINNERTRAINING_API SStringEditor();

    /// Destructor
    BEGINNERTRAINING_API virtual ~SStringEditor() throw();

    /// Text editor manage by the service
    QTextEdit * m_textEditor;
};



} // namespace tuto03
} // namespace beginnerTraining

#endif /*_BEGINNERTRAINING_TUTO03_SSTRINGEDITOR_HPP_*/

