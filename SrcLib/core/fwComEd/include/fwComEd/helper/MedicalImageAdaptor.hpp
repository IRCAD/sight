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


class FWCOMED_CLASS_API MedicalImageAdaptor
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

    FWCOMED_API MedicalImageAdaptor(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

    FWCOMED_API void setOrientation( int orientation );

    FWCOMED_API void getImageSpacing(double spacing[3]);
    FWCOMED_API void getImageDataSize(int size[3]);
    FWCOMED_API void getImageSize(double size[3]);
    FWCOMED_API void getCurrentSliceCenter(double center[3]);

    /// retreive the grey level from an image from physicial world
    //float getPixelvalue( double worldPosition[3]);

    FWCOMED_API void worldToSliceIndex(const double world[3],int index[3] );
    FWCOMED_API void worldToImageSliceIndex(const double world[3], int index[3] );
    /// return the 4 points of the image plane
    /// points are ordered in the following way : image origin is the first point
    /// points are insterted using the preference follow X axis if exist, if not exist follow Y axis
    /// if Y axis is not present follow Z axis
    FWCOMED_API void getPlane( double points[4][3], int sliceNumber );

    FWCOMED_API bool setSliceIndex(const int index[3]);
    FWCOMED_API void getSliceIndex(::fwData::Integer::sptr index[3]);

    FWCOMED_API ::fwData::Integer::sptr getCurrentSliceIndex();
    FWCOMED_API void setCurrentSliceIndex(::fwData::Integer::sptr);

    FWCOMED_API ::fwData::TransfertFunction::sptr getCurrentTransfertFunction();

    FWCOMED_API void updateImageInfos( ::fwData::Image::sptr image  );

    ::fwData::Image::sptr getImage();

    Orientation m_orientation;


    ::fwData::Image::wptr m_weakImage;

    ::fwData::Integer::sptr m_axialIndex;
    ::fwData::Integer::sptr m_frontalIndex;
    ::fwData::Integer::sptr m_sagittalIndex;

    ::fwData::Integer::sptr m_windowMin;
    ::fwData::Integer::sptr m_windowMax;

    ::fwData::Composite::sptr m_transfertFunctions;
    ::fwData::String::sptr m_transfertFunctionId;
};

struct Image0 {} ;
struct Image1 {} ;

template < typename IMAGEID >
class FWCOMED_CLASS_API MedicalImageAdaptorTpl : public MedicalImageAdaptor
{};

typedef MedicalImageAdaptorTpl<Image0> MedicalImageAdaptorImg0;
typedef MedicalImageAdaptorTpl<Image1> MedicalImageAdaptorImg1;

} //namespace helper

} //namespace fwComEd

#endif // _FWCOMED_IIMAGESADAPTOR_HPP_
