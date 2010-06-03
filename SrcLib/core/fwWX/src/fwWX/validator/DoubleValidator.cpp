/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwWX/validator/DoubleValidator.hpp"
#include "fwWX/convert.hpp"

namespace fwWX
{
namespace validator
{

//------------------------------------------------------------------------------

DoubleValidator::DoubleValidator( const DoubleValidator & validator ):
    m_value( validator.m_value ),
    m_minValue( validator.m_minValue ),
    m_maxValue( validator.m_maxValue )
{
    this->SetStyle(wxFILTER_NUMERIC);
}

//------------------------------------------------------------------------------

DoubleValidator::DoubleValidator( double & value):
    m_value( value ),
    m_minValue( 0.0 ),
    m_maxValue( 0.0 )
{}

//------------------------------------------------------------------------------

DoubleValidator::DoubleValidator( double & value, double minValue, double maxValue ):
    m_value( value ),
    m_minValue( minValue ),
    m_maxValue( maxValue )
{}

//------------------------------------------------------------------------------

wxObject * DoubleValidator::Clone() const
{
    return new DoubleValidator( *this );
}

//------------------------------------------------------------------------------

bool DoubleValidator::TransferFromWindow()
{
    wxTextCtrl * textCtrl( wxStaticCast(GetWindow(), wxTextCtrl) );
    wxString textValue( textCtrl->GetValue() );
    double doubleValue;
    bool success;

    success = textValue.ToDouble( &doubleValue );
    OSLM_DEBUG( "DoubleValidator::TransferFromWindow() => " << doubleValue );
    if( success &&
        (m_minValue == m_maxValue || doubleValue >= m_minValue && doubleValue <= m_maxValue))
    {
        this->m_value = (double) doubleValue;
    }
    return success;
}

//------------------------------------------------------------------------------

bool DoubleValidator::TransferToWindow()
{
    wxTextCtrl  * textCtrl = wxStaticCast( GetWindow(), wxTextCtrl );
    OSLM_DEBUG( "DoubleValidator::TransferFromWindow() => " << this->m_value );
    wxString    value = wxString::Format( _("%0.4f"), this->m_value );

    if( textCtrl->GetValue() != value )
    {
        textCtrl->SetValue( value );
    }
    return true;
}

//------------------------------------------------------------------------------

bool DoubleValidator::Validate( wxWindow * parent )
{
    bool success = this->TransferFromWindow();
    if( !success)
    {
        std::stringstream ss;
        ss << "At least one value is either empty or not a valid number. "<< std::endl;
        if (m_minValue != m_maxValue)
        {
            ss << "It must be between " << m_minValue<< " and " << m_maxValue;
        }

        ::wxMessageBox(
                ::fwWX::std2wx( ss.str() ),
                _("Invalid value"),
                wxOK|wxICON_ERROR,
                parent );
        GetWindow()->SetFocus();
    }

    return success;
}

//------------------------------------------------------------------------------

} // validator
} // fwWX
