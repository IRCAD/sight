/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _UIIMAGEWX_SLICE_LIST_EDITOR_HPP_
#define _UIIMAGEWX_SLICE_LIST_EDITOR_HPP_

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiImageWx/config.hpp"

namespace uiImage
{

/**
 * @brief   SliceListEditor service.
 * @class   SliceListEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIIMAGEWX_CLASS_API SliceListEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (SliceListEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIIMAGEWX_API SliceListEditor() throw() ;

    /// Destructor. Do nothing.
    UIIMAGEWX_API virtual ~SliceListEditor() throw() ;

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

    void onDropDownButton( wxCommandEvent& event );

    void onChangeSliceMode(  wxCommandEvent& event );

private:

    std::string m_adaptorUID;
    int m_idDropDown;

    wxMenu* m_pDropDownMenu;
    wxButton* m_dropDownButton;
    wxMenuItem * m_oneSliceItem;
    wxMenuItem * m_threeSlicesItem;
    wxMenuItem * m_obliqueSliceItem;
    int m_nbSlice;

};

} // uiImage

#endif /*_UIIMAGEWX_SLICE_LIST_EDITOR_HPP_*/


