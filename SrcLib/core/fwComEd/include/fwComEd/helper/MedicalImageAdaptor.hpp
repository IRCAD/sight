/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_IIMAGESADAPTOR_HPP_
#define _FWCOMED_IIMAGESADAPTOR_HPP_

#include <vector>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransfertFunction.hpp>

#include "fwComEd/helper/ImageGetter.hpp"
#include "fwComEd/export.hpp"


namespace fwComEd
{

namespace helper
{



class FWCOMED_CLASS_API MedicalImageAdaptor: public ::fwComEd::helper::ImageGetter<>
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (MedicalImageAdaptor) );


     
    typedef enum {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    } Orientation ;

    FWCOMED_API virtual ~MedicalImageAdaptor();

    FWCOMED_API void setOrientation( Orientation orientation );
    FWCOMED_API Orientation getOrientation(){ return m_orientation; }

protected:

    MedicalImageAdaptor(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

    void setOrientation( int orientation );

    void getImageSpacing(double spacing[3]);
    void getImageDataSize(int size[3]);
    void getImageSize(double size[3]);
    void getCurrentSliceCenter(double center[3]);

    /// retreive the grey level from an image from physicial world
    //float getPixelvalue( double worldPosition[3]);

    void worldToSliceIndex(const double world[3],int index[3] );
    void worldToImageSliceIndex(const double world[3], int index[3] );
    /// return the 4 points of the image plane
    /// points are ordered in the following way : image origin is the first point
    /// points are insterted using the preference follow X axis if exist, if not exist follow Y axis
    /// if Y axis is not present follow Z axis
    void getPlane( double points[4][3], int sliceNumber );

    bool setSliceIndex(const int index[3]);
    void getSliceIndex(::fwData::Integer::sptr index[3]);

    ::fwData::Integer::sptr getCurrentSliceIndex();
    void setCurrentSliceIndex(::fwData::Integer::sptr);

    ::fwData::TransfertFunction::sptr getCurrentTransfertFunction();

    void updateImageInfos( ::fwData::Image::sptr image  );

    Orientation m_orientation;

    ::fwData::Integer::sptr m_axialIndex;
    ::fwData::Integer::sptr m_frontalIndex;
    ::fwData::Integer::sptr m_sagittalIndex;

    ::fwData::Integer::sptr m_windowMin;
    ::fwData::Integer::sptr m_windowMax;

    ::fwData::Composite::sptr m_transfertFunctions;
    ::fwData::String::sptr m_transfertFunctionId;
};


} //namespace helper

} //namespace fwComEd

#endif // _FWCOMED_IIMAGESADAPTOR_HPP_
