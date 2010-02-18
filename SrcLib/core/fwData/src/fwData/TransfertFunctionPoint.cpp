/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/TransfertFunctionPoint.hpp"
#include "fwData/Color.hpp"

#include <fwTools/Factory.hpp>


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::TransfertFunctionPoint, ::fwData::TransfertFunctionPoint );
namespace fwData
{
const Object::FieldID TransfertFunctionPoint::ID_COLOR = "ID_COLOR";

//------------------------------------------------------------------------------

TransfertFunctionPoint::TransfertFunctionPoint () :
m_i32Value(0)
{
    SLM_TRACE("::fwData::TransfertFunctionPoint()");
    this->setFieldSingleElement( TransfertFunctionPoint::ID_COLOR, ::fwTools::Factory::New< Color >() );
}

//------------------------------------------------------------------------------

TransfertFunctionPoint::~TransfertFunctionPoint ()
{
    SLM_TRACE("::fwData::~TransfertFunctionPoint()");
}

//------------------------------------------------------------------------------

void TransfertFunctionPoint::shallowCopy( TransfertFunctionPoint::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    this->m_i32Value = _source->m_i32Value;
}

//------------------------------------------------------------------------------

void TransfertFunctionPoint::deepCopy( TransfertFunctionPoint::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );
    this->m_i32Value = _source->m_i32Value;
}

//------------------------------------------------------------------------------

//TransfertFunctionPoint::sptr TransfertFunctionPoint::clone() const
//{
//    TransfertFunctionPoint::sptr pNewTransfertFunctionPoint = ::fwTools::Factory::New< TransfertFunctionPoint >();
//
//    // Copy encoding
//    pNewTransfertFunctionPoint->m_i32Value = this->m_i32Value;
//    pNewTransfertFunctionPoint->setColor( this->getColor()->clone() );
//
//    return pNewTransfertFunctionPoint;
//}

//------------------------------------------------------------------------------

void TransfertFunctionPoint::setColor( ::fwData::Color::sptr _pColor )
{
    if ( _pColor == 0 )
    {
        SLM_WARN("TransfertFunctionPoint::setColor : the Color pointer is null.");
        this->setField( TransfertFunctionPoint::ID_COLOR ); // the previous Color is deleted
    }
    else
    {
        this->setFieldSingleElement( TransfertFunctionPoint::ID_COLOR, _pColor );
    }
}

//------------------------------------------------------------------------------

::fwData::Color::csptr TransfertFunctionPoint::getColor() const
{
    ::fwData::Color::csptr _pColor;

    const unsigned int NbChildren = this->getField( TransfertFunctionPoint::ID_COLOR )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the Color is allocated
    if( NbChildren == 1 )
    {
        _pColor = ::fwData::Color::dynamicCast ( this->getField( TransfertFunctionPoint::ID_COLOR )->children()[0] );
    }
    else
    {
        SLM_WARN("TransfertFunctionPoint::getColor : return an Color pointer is null.");
    }

    return _pColor;
}

//------------------------------------------------------------------------------

::fwData::Color::sptr TransfertFunctionPoint::getColor()
{
    ::fwData::Color::sptr _pColor;

    const unsigned int NbChildren = this->getField( TransfertFunctionPoint::ID_COLOR )->children().size();

    assert ( NbChildren <= 1 );

    // Test if the Color is allocated
    if( NbChildren == 1 )
    {
        _pColor = ::fwData::Color::dynamicCast ( this->getField( TransfertFunctionPoint::ID_COLOR )->children()[0] );
    }
    else
    {
        SLM_WARN("TransfertFunctionPoint::getColor : return an Color pointer is null.");
    }

    return _pColor;
}

//------------------------------------------------------------------------------

} // namespace fwData
