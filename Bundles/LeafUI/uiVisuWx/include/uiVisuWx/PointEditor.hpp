/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIVISUWX_POINT_EDITOR_HPP
#define _UIVISUWX_POINT_EDITOR_HPP


#include <wx/textctrl.h>
#include <wx/listbox.h>

#include <fwTools/Failed.hpp>

#include <fwData/Point.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiVisuWx/config.hpp"

namespace uiVisu
{

/**
 * @brief   PointEditor service allows to display point information.
 * @class   PointEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIVISUWX_CLASS_API PointEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (PointEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIVISUWX_API PointEditor() throw() ;

    /// Destructor. Do nothing.
    UIVISUWX_API virtual ~PointEditor() throw() ;

protected:


    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    void updating() throw(::fwTools::Failed);

    void swapping() throw(::fwTools::Failed);

    void configuring() throw( ::fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;


private:

    wxTextCtrl* m_textCtrl_x;
    wxTextCtrl* m_textCtrl_y;
    wxTextCtrl* m_textCtrl_z;
    double m_valueX, m_valueY, m_valueZ;


};

} // uiVisu

#endif /*_UIVISUWX_POINT_EDITOR_HPP_*/
