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
		wxString( _( "Sagittal"    ) ),
		wxString( _( "Frontal"  ) ),
		wxString( _( "Axial" ) )
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

	m_sliceType = new wxChoice( this, ID_SLICE_TYPE, wxDefaultPosition, wxDefaultSize, m_sliceTypesArray );
	m_sliceIndex = new wxSlider( this, ID_SLICE_INDEX, 0, -1, 1);
	m_pSliceIndexText = new wxTextCtrl( this, ID_SLICE_TEXT, _(""), wxDefaultPosition, wxSize(75,-1), wxTE_READONLY|wxTE_CENTRE);

	wxSizer* const sizer = new wxBoxSizer( wxHORIZONTAL );
	sizer->Add( m_sliceType      , 0, ( wxEXPAND | wxALL )  , 1 );
	sizer->Add( m_sliceIndex     , 1, ( wxEXPAND | wxALL )  , 1 );
	sizer->Add( m_pSliceIndexText, 0, ( wxALL )             , 1 );

	this->SetSizer( sizer );
	this->Layout();
	sizer->Fit( this );
}

//------------------------------------------------------------------------------

SliceSelector::~SliceSelector() throw()
{

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
	this->m_sliceType->SetSelection(type);
}

//------------------------------------------------------------------------------

void SliceSelector::onSliceTypeChange( wxCommandEvent& event )
{
	m_fctChangeTypeCallback( static_cast< int >( this->m_sliceType->GetCurrentSelection() ));
    this->setSliceValue( this->m_sliceIndex->GetValue());
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

// wxWidgets event table
BEGIN_EVENT_TABLE( SliceSelector, wxPanel )
	EVT_CHOICE        ( ID_SLICE_TYPE  , SliceSelector::onSliceTypeChange  )
    EVT_COMMAND_SCROLL( ID_SLICE_INDEX , SliceSelector::onSliceIndexChange )
END_EVENT_TABLE()

} // fwWX

