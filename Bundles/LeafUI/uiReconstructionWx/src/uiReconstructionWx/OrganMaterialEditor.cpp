#include <wx/window.h>
#include <wx/event.h>
#include <wx/sizer.h>

#include <fwCore/base.hpp>

#include <fwTools/Object.hpp>

#include <fwData/Reconstruction.hpp>

#include <fwComEd/MaterialMsg.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>
#include <fwServices/IService.hpp>
#include <fwServices/op/Get.hpp>
#include <fwServices/IEditionService.hpp>

#include <fwWX/convert.hpp>

#include <fwGuiWx/container/WxContainer.hpp>

#include "uiReconstructionWx/OrganMaterialEditor.hpp"

namespace uiReconstruction
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiReconstruction::OrganMaterialEditor , ::fwData::Reconstruction ) ;


OrganMaterialEditor::OrganMaterialEditor() throw()
{
    handlingEventOff();
}

//------------------------------------------------------------------------------

OrganMaterialEditor::~OrganMaterialEditor() throw()
{}

//------------------------------------------------------------------------------

void OrganMaterialEditor::starting() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->create();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    m_colourButton = new wxColourPickerCtrl( container, wxNewId() ) ;
    m_colourButton->SetToolTip(_T("Selected organ's color"));
    m_colourButton->SetMinSize(wxSize(120,35));

    wxStaticText* transparencyST = new wxStaticText( container, wxID_ANY, _("Transparency : "));
    m_opacitySlider = new wxSlider( container, wxNewId(), 100, 0, 100, wxDefaultPosition, wxDefaultSize , wxSL_BOTH|wxSL_HORIZONTAL|wxSL_LABELS|wxSL_TOP ) ;
    m_opacitySlider->SetToolTip(_T("Selected organ's opacity"));

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );
    sizer->Add( m_colourButton, 0, wxEXPAND|wxALL ) ;

    wxSizer* transparencySizer = new wxBoxSizer( wxHORIZONTAL );
    transparencySizer->Add( transparencyST, 0, wxALL ) ;
    transparencySizer->Add( m_opacitySlider, 1, wxEXPAND|wxALL );
    sizer->Add( transparencySizer, 0, wxEXPAND|wxALL ) ;

    container->SetSizer( sizer );
    container->Layout();

    container->Enable(false);

    container->Bind( wxEVT_COMMAND_SLIDER_UPDATED, &OrganMaterialEditor::onOpacitySlider, this,  m_opacitySlider->GetId());
    container->Bind( wxEVT_COMMAND_COLOURPICKER_CHANGED, &OrganMaterialEditor::onColorButton, this,  m_colourButton->GetId());
    this->updating();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::stopping() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    container->Unbind( wxEVT_COMMAND_SLIDER_UPDATED, &OrganMaterialEditor::onOpacitySlider, this,  m_opacitySlider->GetId());
    container->Unbind( wxEVT_COMMAND_COLOURPICKER_CHANGED, &OrganMaterialEditor::onColorButton, this,  m_colourButton->GetId());

    wxContainer->clean();
    this->destroy();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    this->initialize();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::updating() throw(::fwTools::Failed)
{
    this->refreshMaterial();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::info( std::ostream &_sstream )
{
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::onColorButton(wxColourPickerEvent & event )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial() ;
    float red   = event.GetColour().Red()/255.0;
    float green = event.GetColour().Green()/255.0;
    float blue  = event.GetColour().Blue()/255.0;
    //float alpha = event.GetColour().Alpha()/255.0;

    material->ambient()->red() = red;
    material->ambient()->green() = green;
    material->ambient()->blue() = blue;

    this->materialNotification();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::onOpacitySlider(wxCommandEvent & event )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwData::Material::sptr material = reconstruction->getMaterial() ;

    float a = m_opacitySlider->GetValue() ;
    a = (a*255)/100;
    material->ambient()->alpha() = (float) (a / 255.0) ;

    this->materialNotification();
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::refreshMaterial( )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwGuiWx::container::WxContainer::sptr wxContainer =  ::fwGuiWx::container::WxContainer::dynamicCast( this->getContainer() );
    wxWindow* const container = wxContainer->getWxContainer();
    assert( container ) ;

    container->Enable(!reconstruction->getOrganName().empty());

    ::fwData::Material::sptr material = reconstruction->getMaterial() ;
    wxColour wxMaterialColor = wxColour (
                material->ambient()->red()*255,
                material->ambient()->green()*255,
                material->ambient()->blue()*255,
                material->ambient()->alpha()*255
                );

    m_colourButton->SetColour( wxMaterialColor ) ;
    int a = (int) (material->ambient()->alpha()*255) ;
    a = (a*100)/255;
    m_opacitySlider->SetValue( a ) ;
}

//------------------------------------------------------------------------------

void OrganMaterialEditor::materialNotification( )
{
    ::fwData::Reconstruction::sptr reconstruction = this->getObject< ::fwData::Reconstruction>();
    SLM_ASSERT("No Reconstruction!", reconstruction);

    ::fwComEd::MaterialMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::MaterialMsg::MATERIAL_IS_MODIFIED ) ;
    ::fwServices::IEditionService::notify(this->getSptr(), reconstruction->getMaterial(), msg);
}

//------------------------------------------------------------------------------

}

