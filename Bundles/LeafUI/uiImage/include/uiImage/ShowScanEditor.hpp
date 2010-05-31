/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGE_SHOW_SCAN_EDITOR_HPP_
#define _UIIMAGE_SHOW_SCAN_EDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImage/config.hpp"

namespace uiImage
{

/**
 * @brief   ShowScanEditor service.
 * @class   ShowScanEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGE_CLASS_API ShowScanEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ShowScanEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGE_API ShowScanEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGE_API virtual ~ShowScanEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() throw(::fwTools::Failed);

    ///This method launches the IEditor::stopping method.
    virtual void stopping() throw(::fwTools::Failed);

    /// Managment of observations ( overrides )
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    virtual void updating() throw(::fwTools::Failed);

    virtual void swapping() throw(::fwTools::Failed);

    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    void onChangeScanMode(  wxCommandEvent& event );

private:

    std::string m_adaptorUID;

    wxImage m_imageShowScan;
    wxImage m_imageHideScan;
    bool m_scanAreShown;
    wxBitmapButton* m_showScanButton;

};

} // uiImage

#endif /*_UIIMAGE_SHOW_SCAN_EDITOR_HPP_*/


