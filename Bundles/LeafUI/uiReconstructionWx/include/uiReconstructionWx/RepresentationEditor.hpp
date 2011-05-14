#ifndef _UIRECONSTRUCTIONWX_ORGAN_MATERIAL_EDITOR_HPP_
#define _UIRECONSTRUCTIONWX_ORGAN_MATERIAL_EDITOR_HPP_

#include <wx/slider.h>
#include <wx/clrpicker.h>
#include <wx/radiobox.h>
#include <wx/checkbox.h>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiReconstructionWx/config.hpp"

namespace uiReconstruction
{

/**
 * @brief   RepresentationEditor service.
 * @class   RepresentationEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIRECONSTRUCTIONWX_CLASS_API RepresentationEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (RepresentationEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIRECONSTRUCTIONWX_API RepresentationEditor() throw() ;

    /// Destructor. Do nothing.
    UIRECONSTRUCTIONWX_API virtual ~RepresentationEditor() throw() ;

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

    void onChangeRepresentation( wxCommandEvent & event );
    void onChangeShading( wxCommandEvent & event );
    void onShowNormals(wxCommandEvent & event );
    void notifyMaterial();
    void notifyTriangularMesh();

private:

    void refreshNormals();
    void refreshRepresentation() ;
    void refreshShading() ;

    wxRadioBox *m_radioBoxRepresentation;
    wxRadioBox *m_radioBoxShading;
    wxCheckBox *m_normalsCheckBox;

    ::fwData::Material::sptr m_material ;

};

} // uiReconstruction

#endif /*_UIRECONSTRUCTIONWX_ORGAN_MATERIAL_EDITOR_HPP_*/


