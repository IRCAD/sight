/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */
/* ***** BEGIN CONTRIBUTORS BLOCK *****
 * Initial authors:
 *  - Jean-Baptiste.Fasquel (LISA Laboratory, Angers University, France)
 * ****** END CONTRIBUTORS BLOCK ****** */

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/wx.h>
#include <wx/window.h>
#include <wx/frame.h>
#include <wx/colour.h>
#include <wx/slider.h>

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/lambda/lambda.hpp>
#include <boost/lexical_cast.hpp>

#include <fwData/Image.hpp>
#include <fwData/Integer.hpp>
#include <fwData/Float.hpp>
#include <fwData/Composite.hpp>

#include <fwRuntime/ConfigurationElement.hpp>
#include <fwRuntime/operations.hpp>

#include <fwCore/base.hpp>

#include <fwServices/helper.hpp>
#include <fwServices/macros.hpp>
#include <fwServices/ObjectServiceRegistry.hpp>

#include <fwComEd/fieldHelper/MedicalImageHelpers.hpp>
#include <fwComEd/ImageMsg.hpp>
#include <fwComEd/Dictionary.hpp>

#include <fwWX/convert.hpp>

#include "uiImage/OpacityEditor.hpp"

namespace uiImage
{

REGISTER_SERVICE( ::gui::editor::IEditor , ::uiImage::OpacityEditor , ::fwData::Image ) ;

//------------------------------------------------------------------------------
/**
 * @brief   Helper class used to manage interactions with the slider related to the image opacity.
 * @class   Slider.
 * @note     Correspond to an adaptation of the ::fwWX::SliceSelector class
 */
class Slider : public wxPanel
{
public:

    Slider( wxWindow* const parent, const wxWindowID id = wxID_ANY ) throw() : wxPanel( parent, id )
    {
        m_fctChangeIndexCallback = ::boost::bind( &::uiImage::Slider::printIndex, this, _1);

        m_sliceIndex = new wxSlider( this, ID_SLICE_INDEX, 0,0,0);
        m_pSliceIndexText = new wxTextCtrl( this, ID_SLICE_TEXT, _(""), wxDefaultPosition, wxSize(75,-1), wxTE_READONLY|wxTE_CENTRE);

        wxSizer* const sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_sliceIndex     , 1, ( wxEXPAND | wxALL )  , 1 );
        sizer->Add( m_pSliceIndexText, 0, ( wxALL )             , 1 );

        this->SetSizer( sizer );
        this->Layout();
    }

    virtual ~Slider() throw() {}

    void setSliceRange( int min, int max )
    {
        this->m_sliceIndex->SetRange(min, max);
    }

    void setSliceValue( int index )
    {
        this->m_sliceIndex->SetValue(index);
        std::stringstream ss;
        ss << index << " / " << this->m_sliceIndex->GetMax();
        this->m_pSliceIndexText->SetValue( ::fwWX::std2wx(ss.str()));
        this->m_pSliceIndexText->Update();
    }

    void setEnable(bool enable)
    {
        m_sliceIndex->Enable(enable);
        m_pSliceIndexText->Enable(enable);
    }

    typedef ::boost::function1<void, int > ChangeIndexCallback;
    void setChangeIndexCallback(ChangeIndexCallback fct)
    {
        m_fctChangeIndexCallback = fct;
    }

protected:

    void onSliceIndexChange( wxScrollEvent& event ) throw()
    {
        SLM_TRACE_FUNC();
        m_fctChangeIndexCallback( static_cast< unsigned int >( event.GetPosition() ));
        this->setSliceValue( this->m_sliceIndex->GetValue());
    }

    /// @brief wxWidgets event table.
    /// @{
    DECLARE_EVENT_TABLE()
    /// @}

private:

    /// @brief The IDs for the wxWidgets controls.
    enum {
        ID_SLICE_INDEX = wxID_HIGHEST + 1,
        ID_SLICE_TEXT
    };

    /// @brief The slice index slider widget.
    wxSlider*   m_sliceIndex;
    wxTextCtrl * m_pSliceIndexText;

    void printIndex(int index)
    {
        OSLM_TRACE("index= "<<index);
    }

    ChangeIndexCallback m_fctChangeIndexCallback;
};

// wxWidgets event table
BEGIN_EVENT_TABLE( Slider, wxPanel )
    EVT_COMMAND_SCROLL( ID_SLICE_INDEX, Slider::onSliceIndexChange )
END_EVENT_TABLE()


//------------------------------------------------------------------------------

OpacityEditor::OpacityEditor() throw()
{
    addNewHandledEvent( ::fwComEd::ImageMsg::OPACITY );
    addNewHandledEvent( ::fwComEd::ImageMsg::BUFFER );
    addNewHandledEvent( ::fwComEd::ImageMsg::NEW_IMAGE );
}

//------------------------------------------------------------------------------

OpacityEditor::~OpacityEditor() throw()
{}

//------------------------------------------------------------------------------

void OpacityEditor::starting() throw(::fwTools::Failed)
{
    ::gui::editor::IEditor::starting();

    wxSizer* sizer = new wxBoxSizer( wxVERTICAL );

    m_sliceSelectorPanel = new ::uiImage::Slider( m_container );
    m_sliceSelectorPanel->setEnable(false);

    ::uiImage::Slider::ChangeIndexCallback changeOpacityCallback;
    changeOpacityCallback = ::boost::bind( &::uiImage::OpacityEditor::sliceOpacityNotification, this, _1);
    m_sliceSelectorPanel->setChangeIndexCallback(changeOpacityCallback);

    sizer->Add( m_sliceSelectorPanel, 1, wxALL|wxEXPAND, 1 );

    m_container->SetSizer( sizer );
    m_container->Layout();
    this->updating();
}

//------------------------------------------------------------------------------

void OpacityEditor::stopping() throw(::fwTools::Failed)
{
    if(m_sliceSelectorPanel)
    {
        delete m_sliceSelectorPanel;
        m_sliceSelectorPanel = 0;
    }

    ::gui::editor::IEditor::stopping();
}

//------------------------------------------------------------------------------

void OpacityEditor::configuring() throw(fwTools::Failed)
{
    SLM_TRACE_FUNC();
    ::gui::editor::IEditor::configuring();
}

//------------------------------------------------------------------------------

void OpacityEditor::updating() throw(::fwTools::Failed)
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool imageIsValid = ::fwComEd::fieldHelper::MedicalImageHelpers::checkOpacity( image );
    m_sliceSelectorPanel->setEnable(true);
    this->updateOpacity();
}

//------------------------------------------------------------------------------

void OpacityEditor::swapping() throw(::fwTools::Failed)
{
    this->updating();
}
//------------------------------------------------------------------------------

void OpacityEditor::updating( ::fwServices::ObjectMsg::csptr _msg ) throw(::fwTools::Failed)
{
    ::fwComEd::ImageMsg::csptr imageMessage = fwComEd::ImageMsg::dynamicConstCast( _msg );

    if ( imageMessage )
    {
        if ( imageMessage->hasEvent( ::fwComEd::ImageMsg::OPACITY ) )
        {
            //Updates slider
            this->updateOpacity();
            //Enable slider
            this->update();
        }
        if ( imageMessage->hasEvent( ::fwComEd::ImageMsg::BUFFER ) || ( imageMessage->hasEvent( ::fwComEd::ImageMsg::NEW_IMAGE )) )
        {
            //Updates slider
            this->updateOpacity();
            //Enable slider
            this->update();
        }

    }
}

//------------------------------------------------------------------------------

void OpacityEditor::info( std::ostream &_sstream )
{

}

//------------------------------------------------------------------------------

void OpacityEditor::updateOpacity()
{
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    bool fieldIsModified = ::fwComEd::fieldHelper::MedicalImageHelpers::checkOpacity( image );
    float opacity = image->getFieldSingleElement< ::fwData::Float >( ::fwComEd::Dictionary::m_opacityId )->value();
    assert( opacity >= 0.0 && opacity <= 1.0 );
    // Update wxSlider
    m_sliceSelectorPanel->setSliceRange( 0, 100 );
    m_sliceSelectorPanel->setSliceValue( (unsigned int) opacity*100 );

}

//------------------------------------------------------------------------------

void OpacityEditor::sliceOpacityNotification(unsigned int index)
{
    ::fwComEd::ImageMsg::NewSptr msg;
    msg->addEvent( ::fwComEd::ImageMsg::OPACITY ) ;
    ::fwData::Image::sptr image = this->getObject< ::fwData::Image >();
    image->getFieldSingleElement< ::fwData::Float >( ::fwComEd::Dictionary::m_opacityId )->value() = float( index )/100.0;
    ::fwServices::IEditionService::notify(this->getSptr(),  image, msg);
}


}

