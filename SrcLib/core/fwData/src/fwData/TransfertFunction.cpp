/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as 
 * published by the Free Software Foundation.  
 * ****** END LICENSE BLOCK ****** */

#include <fwCore/base.hpp>

#include <fwTools/ClassRegistrar.hpp>
#include <fwTools/Factory.hpp>

#include "fwData/Color.hpp"
#include "fwData/TransfertFunctionPoint.hpp"



#include "fwData/TransfertFunction.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::TransfertFunction, ::fwData::TransfertFunction );

namespace fwData
{

//------------------------------------------------------------------------------

const Object::FieldID TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS = "ID_TRANSFERTFUNCTIONPOINTS";
const std::string TransfertFunction::defaultTransfertFunctionName = "CT-GreyLevel";
//------------------------------------------------------------------------------

TransfertFunction::TransfertFunction ()
{
	SLM_TRACE("::fwData::TransfertFunction()");
	setField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS );
}

//------------------------------------------------------------------------------

TransfertFunction::~TransfertFunction ()
{
	SLM_WARN("::fwData::~TransfertFunction() : (ToDo) Destruction of study vector");
}

//------------------------------------------------------------------------------

TransfertFunction::TransfertFunction( const TransfertFunction & _transfertFunction )
{
	copy(_transfertFunction);
}

//------------------------------------------------------------------------------

TransfertFunction & TransfertFunction::operator=( const TransfertFunction & _transfertFunction )
{
	copy(_transfertFunction);
	return *this;
}
//------------------------------------------------------------------------------

void TransfertFunction::copy( const TransfertFunction & _transfertFunction )
{
	this->m_sName = _transfertFunction.m_sName;
	this->m_sEncoding = _transfertFunction.m_sEncoding;
	this->getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children() = _transfertFunction.getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children();
}

//------------------------------------------------------------------------------

TransfertFunction::sptr TransfertFunction::clone() const
{
	TransfertFunction::NewSptr pNewTransfertFunction;

	// Copy encoding
	pNewTransfertFunction->m_sEncoding = this->m_sEncoding;
	pNewTransfertFunction->m_sName = this->m_sName;

	// Copy TransfertFunctionPoints
	::std::pair< TransfertFunctionPointConstIterator, TransfertFunctionPointConstIterator > range = this->getTransfertFunctionPoints();
	TransfertFunctionPointConstIterator iterPoint = range.first;
	while( iterPoint != range.second )
	{
		pNewTransfertFunction->addFieldElement( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS, (*iterPoint)->clone() );
		++iterPoint;
	}

	return pNewTransfertFunction;
}

//------------------------------------------------------------------------------

boost::uint32_t  TransfertFunction::getTransfertFunctionPointSize() const
{
	return this->getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().size();
}

//------------------------------------------------------------------------------

std::pair< TransfertFunction::TransfertFunctionPointIterator, TransfertFunction::TransfertFunctionPointIterator > TransfertFunction::getTransfertFunctionPoints()
{
	TransfertFunctionPointIterator begin(  getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().begin() );
	TransfertFunctionPointIterator   end(  getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

std::pair< TransfertFunction::TransfertFunctionPointConstIterator, TransfertFunction::TransfertFunctionPointConstIterator > TransfertFunction::getTransfertFunctionPoints() const
{
	TransfertFunctionPointConstIterator begin(  getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().begin()   );
	TransfertFunctionPointConstIterator   end(  getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().end()   );
	return std::make_pair( begin, end );
}

//------------------------------------------------------------------------------

::fwData::Color::sptr TransfertFunction::getColor( ::fwData::TransfertFunctionPoint::TFValueType  _i32Value )
{
	::fwData::Color::sptr pColor;

	bool itemIsFound = false;

	// Find item
	TransfertFunctionPointIterator tfpItr;
	std::pair< TransfertFunctionPointIterator, TransfertFunctionPointIterator >  range = getTransfertFunctionPoints();
	tfpItr = range.first;
	while ( tfpItr != range.second && !itemIsFound )
	{
		if( (*tfpItr)->getValue() ==  _i32Value )
		{
			itemIsFound = true;
			pColor = (*tfpItr)->getColor();
		}
		else
		{
			++tfpItr;
		}
	}

	if( itemIsFound )
	{
	}
	else
	{
		::fwData::TransfertFunctionPoint::NewSptr pTransfertFunctionPoint;
		pTransfertFunctionPoint->setValue(_i32Value);
		this->addFieldElement( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS, pTransfertFunctionPoint );
		pColor = pTransfertFunctionPoint->getColor();
	}


	return pColor;
}

//------------------------------------------------------------------------------

void TransfertFunction::erase( ::fwData::TransfertFunctionPoint::TFValueType  _i32Value )
{
	bool itemIsFound = false;

	// Find item
	TransfertFunctionPointIterator tfpItr;
	std::pair< TransfertFunctionPointIterator, TransfertFunctionPointIterator >  range = getTransfertFunctionPoints();
	tfpItr = range.first;
	while ( tfpItr != range.second && !itemIsFound )
	{
		if( (*tfpItr)->getValue() ==  _i32Value )
		{
			itemIsFound = true;
		}
		else
		{
			++tfpItr;
		}
	}

	// Erase item
	if( itemIsFound )
	{
		getField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS )->children().erase( tfpItr.base() );
	}
}
//------------------------------------------------------------------------------
void TransfertFunction::insert( ::fwData::TransfertFunctionPoint::sptr pTransfertFunctionPoint )
{
	erase( pTransfertFunctionPoint->getValue() );
	this->addFieldElement( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS, pTransfertFunctionPoint );
}

//------------------------------------------------------------------------------

void TransfertFunction::clear()
{
	setField( TransfertFunction::ID_TRANSFERTFUNCTIONPOINTS );
}

//------------------------------------------------------------------------------

TransfertFunction::sptr TransfertFunction::createDefaultTransfertFunction( ::fwData::Image::sptr pImage )
{
	double center = pImage->getWindowCenter();
	double width = pImage->getWindowWidth();
	int min, max;

	if ( center == 0 && width == 0 )
	{
		min = -200;
		max = 300;
	}
	else
	{
		min = (int) ( center - width / 2.0 );
		max = (int) ( center + width / 2.0 );
	}

	TransfertFunction::sptr pTF = TransfertFunction::New();
	pTF->setCRefName(TransfertFunction::defaultTransfertFunctionName);
	Color::sptr color = pTF->getColor( min );
	color->getRefRGBA()[0] = 0;
	color->getRefRGBA()[1] = 0;
	color->getRefRGBA()[2] = 0;
	color->getRefRGBA()[3] = 0;

	color = pTF->getColor( max );
	color->getRefRGBA()[0] = 1;
	color->getRefRGBA()[1] = 1;
	color->getRefRGBA()[2] = 1;
	color->getRefRGBA()[3] = 1;

	return pTF;
}

//------------------------------------------------------------------------------

std::pair< double, ::boost::int32_t > TransfertFunction::getCenterWidth() const
{
	typedef ::fwData::TransfertFunction::TransfertFunctionPointConstIterator TFPCIterator;
	std::pair< TFPCIterator, TFPCIterator > range = this->getTransfertFunctionPoints();
	TFPCIterator firstPointIter = range.first;
	TFPCIterator lastPointIter = range.second - 1;

	::boost::int32_t min = (*firstPointIter)->getValue();
	::boost::int32_t width = (*lastPointIter)->getValue() - (*firstPointIter)->getValue();
	double center = min + width / 2.0;

	return std::pair< double, ::boost::int32_t >( center , width );
}

//------------------------------------------------------------------------------

std::pair< ::boost::int32_t, ::boost::int32_t > TransfertFunction::getMinMax() const
{
	typedef ::fwData::TransfertFunction::TransfertFunctionPointConstIterator TFPCIterator;
	std::pair< TFPCIterator, TFPCIterator > range = this->getTransfertFunctionPoints();
	TFPCIterator firstPointIter = range.first;
	TFPCIterator lastPointIter = range.second - 1;

	return std::pair< ::boost::int32_t, ::boost::int32_t >( (*firstPointIter)->getValue(), (*lastPointIter)->getValue() );
}

//------------------------------------------------------------------------------

void TransfertFunction::setMinMax( ::fwData::TransfertFunctionPoint::TFValueType _min, ::fwData::TransfertFunctionPoint::TFValueType _max )
{

	::fwData::TransfertFunction::sptr pTransfertFunctionClone = this->clone();
	this->clear();

	typedef ::fwData::TransfertFunction::TransfertFunctionPointIterator TFPCIterator;
	std::pair<TFPCIterator,TFPCIterator> range = pTransfertFunctionClone->getTransfertFunctionPoints();
	TFPCIterator iterTF = range.first;
	TFPCIterator end = range.second;

	// Old values
	::fwData::TransfertFunctionPoint::TFValueType oldMin = (*iterTF)->getValue();
	::fwData::TransfertFunctionPoint::TFValueType oldMax = (*(end - 1))->getValue();
	::fwData::TransfertFunctionPoint::TFValueType oldWidth = oldMax - oldMin;

	// New value
	::fwData::TransfertFunctionPoint::TFValueType newWidth = _max - _min;

	::fwData::Color::sptr colorOrigin;
	::fwData::Color::sptr color;
	while ( iterTF != end )
	{
		::fwData::TransfertFunctionPoint::TFValueType oldValue = (*iterTF)->getValue();
		::fwData::TransfertFunctionPoint::TFValueType newValue = _min +  ( oldValue - oldMin ) * newWidth / (float) oldWidth;

		colorOrigin = pTransfertFunctionClone->getColor( oldValue );
		color = this->getColor( newValue );
		color->getRefRGBA()[0] = colorOrigin->getRefRGBA()[0];
		color->getRefRGBA()[1] = colorOrigin->getRefRGBA()[1];
		color->getRefRGBA()[2] = colorOrigin->getRefRGBA()[2];
		color->getRefRGBA()[3] = colorOrigin->getRefRGBA()[3];
		iterTF++;
	}
}

//------------------------------------------------------------------------------

} // end namespace fwData
