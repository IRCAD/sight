/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEWX_SHOW_SCAN_EDITOR_HPP_
#define _UIIMAGEWX_SHOW_SCAN_EDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImageWx/config.hpp"

namespace uiImage
{

/**
 * @brief   This editor service represent a button to show/hide scan in a generic scene.
 * @class   ShowScanEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEWX_CLASS_API ShowScanEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (ShowScanEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEWX_API ShowScanEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEWX_API virtual ~ShowScanEditor() throw() ;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    /**
     * @brief Install the layout.
     *
     * This method launches the IEditor::starting method.
     */
    virtual void starting() throw(::fwTools::Failed);

    /**
     * @brief Destroy the layout.
     *
     * This method launches the IEditor::stopping method.
     */
    virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating( ::boost::shared_ptr< const fwServices::ObjectMsg > _msg ) throw(::fwTools::Failed);

    /// Do nothing
    virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    virtual void swapping() throw(::fwTools::Failed);

    /**
     * @brief Configure the editor.
     *
     * Example of configuration
     * @verbatim
     <service uid="showScanNegato3DEditor" type="::gui::editor::IEditor" implementation="::uiImage::ShowScanEditor" autoComChannel="no">
         <negatoAdaptor uid="myNegatoMPR" />
     </service>
       @endverbatim
       \b myNegatoMPR is the uid of the ::visuVTKAdaptor::NegatoMPR service where the scan will be show/hide.
     */
    virtual void configuring() throw(fwTools::Failed);

    /// Overrides
    virtual void info( std::ostream &_sstream ) ;

    /**
     * @brief This method is called when the scan button is clicked.
     *
     * This service notifies the modification.
     */
    void onChangeScanMode(  wxCommandEvent& event );

private:

    std::string m_adaptorUID;

    wxImage m_imageShowScan;
    wxImage m_imageHideScan;
    bool m_scanAreShown;
    wxBitmapButton* m_showScanButton;

};

} // uiImage

#endif /*_UIIMAGEWX_SHOW_SCAN_EDITOR_HPP_*/


