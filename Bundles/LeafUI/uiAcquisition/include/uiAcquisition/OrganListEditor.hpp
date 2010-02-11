#ifndef _UIACQUISITION_ORGAN_LIST_EDITOR_HPP_
#define _UIACQUISITION_ORGAN_LIST_EDITOR_HPP_

#include <wx/checklst.h>
#include <wx/checkbox.h>

#include <fwTools/Failed.hpp>
#include <gui/editor/IEditor.hpp>

#include "uiAcquisition/config.hpp"

namespace uiAcquisition
{

/**
 * @brief   OrganListEditor service.
 * @class   OrganListEditor.
 * @author  IRCAD (Research and Development Team).
 * @date    2010.
 */
class UIACQUISITION_CLASS_API OrganListEditor : public ::gui::editor::IEditor
{

public :

    fwCoreServiceClassDefinitionsMacro ( (OrganListEditor)(::gui::editor::IEditor) ) ;

    /// Constructor. Do nothing.
    UIACQUISITION_API OrganListEditor() throw() ;

    /// Destructor. Do nothing.
    UIACQUISITION_API virtual ~OrganListEditor() throw() ;

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

    typedef std::map< std::string, ::fwData::Reconstruction::sptr > OrganNameReconstruction;

    void updateReconstructions();
    void notifyOrganChoiceSelection();
    void onShowReconstructions(wxCommandEvent & event );
    void onOrganChoiceVisibility(wxCommandEvent & event );
    void onOrganChoiceSelection(wxCommandEvent & event );

private:

    void refreshVisibility();
    wxCheckBox* m_showCheckBox;
    wxCheckListBox* m_organChoice;
    OrganNameReconstruction m_map ;

};

} // uiAcquisition

#endif /*_UIACQUISITION_ORGAN_LIST_EDITOR_HPP_*/


