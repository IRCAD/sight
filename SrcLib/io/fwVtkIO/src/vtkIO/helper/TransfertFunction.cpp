/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "vtkIO/helper/TransfertFunction.hpp"

namespace vtkIO
{
namespace helper
{

//------------------------------------------------------------------------------

void TransfertFunction::toVtkLookupTable(
        ::fwData::TransferFunction::sptr tf,
         vtkSmartPointer<vtkLookupTable> lt,
        bool allowTransparency,
        unsigned int size )
{
    SLM_WARN_IF( "Sorry, This transfer function conversion not managed clamped TF.", tf->getIsClamped() );

    // Configures basic parameters
    lt->SetNumberOfTableValues( size );
    lt->SetScaleToLinear();


    ::fwData::TransferFunction::TFValuePairType minMax = tf->getMinMaxTFValues();

    lt->SetTableRange( minMax.first, minMax.second );

    double delta = ( minMax.second - minMax.first ) / (double) (size - 1);
    ::fwData::TransferFunction::TFColor interpolatedColor;

    if ( allowTransparency )
    {
        for( unsigned int k=0; k<size; ++k )
        {
            interpolatedColor = tf->getInterpolatedColor( k*delta + minMax.first );
            lt->SetTableValue(k, interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, interpolatedColor.a);
        }
    }
    else
    {
        for( unsigned int k=0; k<size; ++k )
        {
            interpolatedColor = tf->getInterpolatedColor( k*delta + minMax.first );
            lt->SetTableValue(k, interpolatedColor.r, interpolatedColor.g, interpolatedColor.b, 1.0);
        }
    }

    lt->Modified();
}

//------------------------------------------------------------------------------

void TransfertFunction::toBWVtkLookupTable(
         double rangeMin,
         double rangeMax,
         vtkSmartPointer<vtkLookupTable> lt,
         unsigned int size )
{
    SLM_TRACE_FUNC();

    // Configures basic parameters
    lt->Allocate( size, size );
    lt->SetScaleToLinear();

    lt->SetRampToLinear();
    lt->SetTableRange( rangeMin, rangeMax );
    lt->SetAlphaRange( 1.0, 1.0 );
    lt->SetHueRange( 0.0, 0.0 );
    lt->SetSaturationRange( 0.0, 0.0 );
    lt->SetValueRange( 0.0, 1.0 );

    lt->Build();

    lt->Modified();
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace vtkIO
