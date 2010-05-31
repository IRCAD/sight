/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISU_SNAPSHOTEDITOR_HPP_
#define _UIVISU_SNAPSHOTEDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiVisu/config.hpp"

namespace uiVisu
{

/**
 * @brief   SnapshotEditor service.
 * @class   SnapshotEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISU_CLASS_API SnapshotEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SnapshotEditor)(::gui::editor::IEditor::Baseclass) ) ;

    /// Constructor. Do nothing.
    UIVISU_API SnapshotEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISU_API virtual ~SnapshotEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    void onSnapButton( wxCommandEvent& event );

private:
    std::string requestFileName();

    std::vector< std::string > m_scenesUID;
    int m_idSnapButton;

};

} // uiVisu

#endif /*_UIVISU_SNAPSHOTEDITOR_HPP_*/


