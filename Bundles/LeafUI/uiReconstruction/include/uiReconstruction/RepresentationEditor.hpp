#ifndef _UIRECONSTRUCTION_ORGAN_MATERIAL_EDITOR_HPP_
#define _UIRECONSTRUCTION_ORGAN_MATERIAL_EDITOR_HPP_

#include <wx/slider.h>
#include <wx/clrpicker.h>

#include <fwTools/Failed.hpp>

#include <gui/editor/IEditor.hpp>

#include "uiReconstruction/config.hpp"

namespace uiReconstruction
{

/**
 * @brief   RepresentationEditor service.
 * @class   RepresentationEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIRECONSTRUCTION_CLASS_API RepresentationEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (RepresentationEditor)(::gui::editor::IEditor::Baseclass) ) ;

    /// Constructor. Do nothing.
    UIRECONSTRUCTION_API RepresentationEditor() throw() ;

    /// Destructor. Do nothing.
    UIRECONSTRUCTION_API virtual ~RepresentationEditor() throw() ;

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

#endif /*_UIRECONSTRUCTION_ORGAN_MATERIAL_EDITOR_HPP_*/


