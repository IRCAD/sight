/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwTools/ClassRegistrar.hpp>

#include "fwData/Histogram.hpp"


REGISTER_BINDING_BYCLASSNAME( ::fwTools::Object, ::fwData::Histogram, ::fwData::Histogram);


namespace fwData
{

Histogram::Histogram()
{
    SLM_TRACE_FUNC();
    m_binsWidth = 1;
    (*this).push_back( 0 ); // init
}

//------------------------------------------------------------------------------

Histogram::~Histogram()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

Histogram &Histogram::getRefContainer()
{
    return *this;
}

//------------------------------------------------------------------------------

Histogram const &Histogram::getRefContainer() const
{
    return *this;
}

//------------------------------------------------------------------------------

void Histogram::shallowCopy( Histogram::csptr _source )
{
    ::fwTools::Object::shallowCopyOfChildren( _source );
    (ObjectHistogramType)(*this) = (ObjectHistogramType)(*(_source.get()));
}

//------------------------------------------------------------------------------

void Histogram::deepCopy( Histogram::csptr _source )
{
    ::fwTools::Object::deepCopyOfChildren( _source );

    this->clear();

    for(    Histogram::Container::const_iterator iter = _source->begin();
            iter != _source->end();
            ++iter )
    {
        this->push_back( *iter );
    }
}

//------------------------------------------------------------------------------

void Histogram::addPixel( signed int _pixel )
{
    int index = _pixel / m_binsWidth;
    int size  = (*this).size(); 
 
    if( index > size )  // the given pixel is outside the current size of the vector
    {
        for(int i = size; i < index; i++)   // the vector is extended 
        {
            (*this).push_back( 0 );
        }
        
        (*this).push_back( (isInRange( _pixel ) ? 1 : 0) );
    }
    else if ( isInRange( _pixel ) )
    { 
        if( index == 0 )
        {
            (*this).at( index )++;
        }
        else
        {
            (*this).at( index )++;
        }
    }
}

//------------------------------------------------------------------------------

void Histogram::addPixel( float _pixel )
{
    addPixel( static_cast< signed int >( _pixel ) );
}

//------------------------------------------------------------------------------

long Histogram::getNbPixels( double _min, double _max )
{
    SLM_ASSERT("The minimum value can't be greater or equal than the maximum value", _min < _max);

    long min = static_cast< long >( _min ) / m_binsWidth;
    long max = static_cast< long >( _max ) / m_binsWidth;
    long nbPixels = 0;
    
    if( max < (*this).size() )
    {
        for(long i = min; i < max; i++)
        {
            nbPixels += (*this).at( i );
        }
    }

    return nbPixels; 
}

//------------------------------------------------------------------------------

bool Histogram::isInRange( signed int _pixel )
{
    return ( _pixel <= m_maxValue && _pixel >= m_minValue );
}

}

