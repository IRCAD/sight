/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GUIWX_EDITOR_DUMMYIEDITOR_HPP_
#define _GUIWX_EDITOR_DUMMYIEDITOR_HPP_

#include <wx/stattext.h>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "guiWx/config.hpp"

namespace gui
{

namespace editor
{


/**
 * @brief   Defines the service interface managing the basic editor service for object. Do nothing.
 * @class   DummyEditor.
 * @author  IRCAD (Research and Development Team).

 * @date    2009.
 *
 * @todo ACH: This class has been created in order to build test application. Do we remove it now ??
 */
class GUIWX_CLASS_API DummyEditor : public ::gui::editor::IEditor
{

public :


    fwCoreServiceClassDefinitionsMacro ( (DummyEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    GUIWX_API DummyEditor() throw() ;

    /// Destructor. Do nothing.
    GUIWX_API virtual ~DummyEditor() throw() ;

protected:

    /** @name Service methods ( override from ::fwServices::IService )
     * @{
     */
    /**
     * @brief This method launches the IEditor::starting method.
     */
    GUIWX_API virtual void starting() throw( ::fwTools::Failed ) ;

    /**
     * @brief This method launches the IEditor::stopping method.
     */
    GUIWX_API virtual void stopping() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to update services on notification. Do nothing.
     */
    GUIWX_API virtual void updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed) ;

    /**
     * @brief This method is used to update services. Do nothing.
     */
    GUIWX_API virtual void updating() throw(::fwTools::Failed);

    /**
     * @brief This method is used to configure the class parameters. Do nothing.
    */
    GUIWX_API virtual void configuring() throw( ::fwTools::Failed );

    /**
     * @brief This method is used to give information about the service. Do nothing.
     */
    GUIWX_API virtual void info(std::ostream &_sstream );

    ///@}

private:
    /**
     * @brief optional text
     */
    std::string m_text;
    wxStaticText*  m_staticText;
};

}
}

#endif /*_GUIWX_EDITOR_DUMMYIEDITOR_HPP_*/


