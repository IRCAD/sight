/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <wx/event.h>
#include <wx/panel.h>
#include <wx/sizer.h>
#include <wx/slider.h>
#include <wx/textctrl.h>
#include <wx/choice.h>

#include <fwCore/base.hpp>

#include "fwWX/convert.hpp"
#include "fwWX/SliceSelector.hpp"

namespace fwWX
{

//------------------------------------------------------------------------------

const wxString SliceSelector::m_sliceTypes[] =
{
                wxString( _("Sagittal") ),
                wxString( _("Frontal") ),
                wxString( _("Axial") )
};

//------------------------------------------------------------------------------

const wxArrayString SliceSelector::m_sliceTypesArray(
                sizeof( m_sliceTypes ) / sizeof( *m_sliceTypes ),
                m_sliceTypes );

//------------------------------------------------------------------------------

SliceSelector::SliceSelector(wxWindow* const parent, const wxWindowID id ) throw(): wxPanel( parent, id )
{
        m_fctChangeIndexCallback = ::boost::bind( &::fwWX::SliceSelector::printIndex, this, _1);
        m_fctChangeTypeCallback = ::boost::bind( &::fwWX::SliceSelector::printType, this, _1);

#ifdef __MACOSX__
    m_sliceMenu = new wxMenu();
    m_sliceType = new wxButton( this, ID_SLICE_TYPE, _("Planes"), wxDefaultPosition, wxDefaultSize );
#else
        m_sliceType = new wxChoice( this, ID_SLICE_TYPE, wxDefaultPosition, wxDefaultSize, m_sliceTypesArray );
#endif
        m_sliceIndex = new wxSlider( this, ID_SLICE_INDEX, 0, 0, 0);
        m_pSliceIndexText = new wxTextCtrl( this, ID_SLICE_TEXT, _(""), wxDefaultPosition, wxSize(75,-1), wxTE_READONLY|wxTE_CENTRE);

        wxSizer* const sizer = new wxBoxSizer( wxHORIZONTAL );
        sizer->Add( m_sliceType      , 0, ( wxEXPAND | wxALL )  , 1 );
        sizer->Add( m_sliceIndex     , 1, ( wxEXPAND | wxALL )  , 1 );
        sizer->Add( m_pSliceIndexText, 0, ( wxALL )             , 1 );

#ifdef __MACOSX__
        m_sliceTypeSelection = 0;
    m_pItemAxial = new wxMenuItem(m_sliceMenu, ID_AXIAL_BTN, m_sliceTypes[2] , wxEmptyString, wxITEM_RADIO);
    m_pItemFrontal = new wxMenuItem(m_sliceMenu, ID_FRONTAL_BTN, m_sliceTypes[1] , wxEmptyString, wxITEM_RADIO);
    m_pItemSagittal = new wxMenuItem(m_sliceMenu, ID_SAGITTAL_BTN, m_sliceTypes[0] , wxEmptyString, wxITEM_RADIO);

    m_sliceMenu->Append(m_pItemAxial);
    m_sliceMenu->Append(m_pItemFrontal);
    m_sliceMenu->Append(m_pItemSagittal);
#endif

        this->SetSizer( sizer );
        this->Layout();
}

//------------------------------------------------------------------------------

SliceSelector::~SliceSelector() throw()
{
#ifdef __MACOSX__
        m_sliceMenu->Remove(m_pItemAxial);
        m_sliceMenu->Remove(m_pItemFrontal);
        m_sliceMenu->Remove(m_pItemSagittal);
#endif
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceRange( int min, int max )
{
        this->m_sliceIndex->SetRange(min, max);
}

//------------------------------------------------------------------------------

void SliceSelector::setSliceValue( int index )
{
        this->m_sliceIndex->SetValue(index);

        std::stringstream ss;
        ss << index << " / " << this->m_sliceIndex->GetMax();
        this->m_pSliceIndexText->SetValue( ::fwWX::std2wx(ss.str()));
        this->m_pSliceIndexText->Update();
}

//------------------------------------------------------------------------------

void SliceSelector::setTypeSelection( int type )
{
#ifdef __MACOSX__
        m_sliceTypeSelection = type;
        switch(type)
        {
        case 0 :
        {
                m_pItemSagittal->Check(true);
                break ;
        }
        case 1 :
        {
                m_pItemFrontal->Check(true);
                break ;
        }
        case 2 :
        {
                m_pItemAxial->Check(true);
                break ;
        }
        default :
        {
                OSLM_FATAL("Unknown slice type: "<<type);
                break ;
        }
        }
#else
        this->m_sliceType->SetSelection(type);
#endif
}

//------------------------------------------------------------------------------

void SliceSelector::onSliceIndexChange( wxScrollEvent& event ) throw()
{
        SLM_TRACE_FUNC();
        m_fctChangeIndexCallback( static_cast< unsigned int >( event.GetPosition() ));
        this->setSliceValue( this->m_sliceIndex->GetValue());
}

//------------------------------------------------------------------------------

void SliceSelector::setChangeIndexCallback(ChangeIndexCallback fct)
{
        m_fctChangeIndexCallback = fct;
}

//------------------------------------------------------------------------------

void SliceSelector::setChangeTypeCallback(ChangeTypeCallback fct)
{
        m_fctChangeTypeCallback = fct;
}

//------------------------------------------------------------------------------

void SliceSelector::printIndex(int index)
{
        OSLM_TRACE("index= "<<index);
}

//------------------------------------------------------------------------------

void SliceSelector::printType(int type)
{
        OSLM_TRACE("type= "<<type);
}

//------------------------------------------------------------------------------

void SliceSelector::setEnable(bool enable)
{
        m_sliceType->Enable(enable);
        m_sliceIndex->Enable(enable);
        m_pSliceIndexText->Enable(enable);
}

//------------------------------------------------------------------------------
#ifdef __MACOSX__
void SliceSelector::onSliceDownButton( wxCommandEvent& event )
{
        SLM_TRACE_FUNC();
        this->m_sliceType->PopupMenu(m_sliceMenu, m_sliceType->GetSize().GetWidth(),0);
}
//------------------------------------------------------------------------------
void SliceSelector::onSliceTypeDownButton (  wxCommandEvent& event )
{
        switch(event.GetId())
        {
        case ID_SAGITTAL_BTN :
        {
                m_sliceTypeSelection = 0;
                break ;
        }
        case ID_FRONTAL_BTN :
        {
                m_sliceTypeSelection = 1;
                break ;
        }
        case ID_AXIAL_BTN :
        {
                m_sliceTypeSelection = 2;
                break ;
        }
        default :
        {
                OSLM_WARN("Unknown slice type button event id: "<<event.GetId());
                m_sliceTypeSelection = 0;
                break ;
        }
        }
        m_fctChangeTypeCallback( m_sliceTypeSelection );
        this->setSliceValue( this->m_sliceIndex->GetValue());
}
#else
//------------------------------------------------------------------------------
void SliceSelector::onSliceTypeChange( wxCommandEvent& event )
{
        m_fctChangeTypeCallback( static_cast< int >( this->m_sliceType->GetCurrentSelection() ));
    this->setSliceValue( this->m_sliceIndex->GetValue());
}
#endif

//------------------------------------------------------------------------------

// wxWidgets event table
BEGIN_EVENT_TABLE( SliceSelector, wxPanel )
#ifdef __MACOSX__
    EVT_BUTTON        ( ID_SLICE_TYPE, SliceSelector::onSliceDownButton  )
        EVT_MENU          ( ID_AXIAL_BTN, SliceSelector::onSliceTypeDownButton  )
        EVT_MENU          ( ID_FRONTAL_BTN, SliceSelector::onSliceTypeDownButton  )
        EVT_MENU          ( ID_SAGITTAL_BTN, SliceSelector::onSliceTypeDownButton  )
#else
    EVT_CHOICE        ( ID_SLICE_TYPE, SliceSelector::onSliceTypeChange )
#endif
    EVT_COMMAND_SCROLL( ID_SLICE_INDEX, SliceSelector::onSliceIndexChange )
END_EVENT_TABLE()

} // fwWX

