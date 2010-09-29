/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include "fwWX/validator/LongValidator.hpp"
#include "fwWX/convert.hpp"

namespace fwWX
{
namespace validator
{

//------------------------------------------------------------------------------

LongValidator::LongValidator( const LongValidator & validator ):
    m_value( validator.m_value ),
    m_minValue( validator.m_minValue ),
    m_maxValue( validator.m_maxValue )

{
    this->SetStyle(wxFILTER_INCLUDE_CHAR_LIST);
    this->SetCharIncludes("-0123456789");
}

//------------------------------------------------------------------------------

LongValidator::LongValidator( long & value):
    m_value( value ),
    m_minValue( 0 ),
    m_maxValue( 0 )
{}

//------------------------------------------------------------------------------

LongValidator::LongValidator( long & value, long minValue):
    m_value( value ),
    m_minValue( minValue ),
    m_maxValue( minValue-1 )
{}

//------------------------------------------------------------------------------

LongValidator::LongValidator( long & value, long minValue, long maxValue ):
    m_value( value ),
    m_minValue( minValue ),
    m_maxValue( maxValue )
{}

//------------------------------------------------------------------------------

wxObject * LongValidator::Clone() const
{
    return new LongValidator( *this );
}

//------------------------------------------------------------------------------

bool LongValidator::TransferFromWindow()
{
    wxTextCtrl * textCtrl( wxStaticCast(GetWindow(), wxTextCtrl) );
    wxString textValue( textCtrl->GetValue() );
    long longValue;
    bool success;

    success = textValue.ToLong( &longValue );
    if( success &&
        (   m_minValue == m_maxValue ||
            longValue >= m_minValue && longValue <= m_maxValue ||
            m_maxValue < m_minValue && longValue >= m_minValue))
    {
        this->m_value = (long) longValue;
    }
    return success;
}

//------------------------------------------------------------------------------

bool LongValidator::TransferToWindow()
{
    wxTextCtrl  * textCtrl = wxStaticCast( GetWindow(), wxTextCtrl );
    wxString    value = wxString::Format( _("%ld"), this->m_value );

    if( textCtrl->GetValue() != value )
    {
        textCtrl->SetValue( value );
    }
    return true;
}

//------------------------------------------------------------------------------

bool LongValidator::Validate( wxWindow * parent )
{
    bool success = this->TransferFromWindow();
    if( !success )
    {
        std::stringstream ss;
        ss << "At least one value is either empty or not a valid integer. "<< std::endl;
        if (m_minValue < m_maxValue)
        {
            ss << "It must be an integer between " << m_minValue<< " and " << m_maxValue;
        }
        else if (m_minValue > m_maxValue)
        {
            ss << "It must be an integer greater than " << m_minValue;
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
