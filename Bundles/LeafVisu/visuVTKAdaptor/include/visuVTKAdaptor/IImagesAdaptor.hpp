/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _VISUVTKADAPTOR_IIMAGESADAPTOR_HPP_
#define _VISUVTKADAPTOR_IIMAGESADAPTOR_HPP_

#include <vector>

#include <fwData/Integer.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

#include "visuVTKAdaptor/config.hpp"


namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API IImagesAdaptor: public ::fwRenderVTK::IVtkAdaptorService
{

public:


    fwCoreServiceClassDefinitionsMacro ( (IImagesAdaptor)(::fwRenderVTK::IVtkAdaptorService::Baseclass) ) ;

    typedef enum {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    } Orientation ;

    VISUVTKADAPTOR_API virtual ~IImagesAdaptor() throw();

    VISUVTKADAPTOR_API void setOrientation( Orientation orientation );
    VISUVTKADAPTOR_API Orientation getOrientation(){ return m_orientation; }

protected:

    IImagesAdaptor() throw(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

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

    Orientation m_orientation;
};




} //namespace visuVTKAdaptor

#endif // _VISUVTKADAPTOR_IIMAGESADAPTOR_HPP_
