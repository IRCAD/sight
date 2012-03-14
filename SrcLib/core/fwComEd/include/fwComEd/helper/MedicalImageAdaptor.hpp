/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWCOMED_MEDICALIMAGEADAPTOR_HPP_
#define _FWCOMED_MEDICALIMAGEADAPTOR_HPP_

#include <vector>

#include <fwData/Composite.hpp>
#include <fwData/Integer.hpp>
#include <fwData/String.hpp>
#include <fwData/TransfertFunction.hpp>

#include "fwComEd/TransferFunctionMsg.hpp"
#include "fwComEd/helper/ImageGetter.hpp"
#include "fwComEd/export.hpp"


namespace fwComEd
{

namespace helper
{

/**
 * @class   MedicalImageAdaptor
 * @brief   Helpers for medical image.
 * @author  IRCAD (Research and Development Team).
 * @date    2010
 */
class FWCOMED_CLASS_API MedicalImageAdaptor
{

public:
    fwCoreNonInstanciableClassDefinitionsMacro( (MedicalImageAdaptor) );


    /// Image orientation
    typedef enum {
        X_AXIS = 0,
        Y_AXIS,
        Z_AXIS
    } Orientation ;

    /// Destructor. Do nothing.
    FWCOMED_API virtual ~MedicalImageAdaptor();

    /// Set the image orientation.
    FWCOMED_API void setOrientation( Orientation orientation );

    /// Return the image orientation.
    FWCOMED_API Orientation getOrientation(){ return m_orientation; }



    /**
     * @brief Set the tf pool and the key that represents the current selection of TransfertFunction.
     * By defaults use ::fwComEd::Dictionary::m_transfertFunctionId
     */
    FWCOMED_API void setTFParameters( ::fwData::Composite::sptr tfPool, std::string tfSelectionId );

    /// Set TF Pool fwID
    FWCOMED_API void setTFPoolFwID( const std::string & fwid );

    /// Get TF Pool fwID
    FWCOMED_API const std::string & getTFPoolFwID() const;

    /// Set selected TF Key
    FWCOMED_API void setSelectedTFKey( const std::string & key );

    /// Get selected TF Key
    FWCOMED_API const std::string & getSelectedTFKey() const;

    /// Set selected TF Key and TF Pool fwID from config
    FWCOMED_API void parseTFConfig( ::fwRuntime::ConfigurationElement::sptr configuration );

    /// Get the window of the selected tf
    FWCOMED_API double getWindow() const;

    /// Set the window of the selected tf
    FWCOMED_API void setWindow( double window );

    /// Set the window level of the selected tf from window min/max
    FWCOMED_API void setWindowLevel( double windowMin, double windowMax );

    /// Get the level of the selected tf
    FWCOMED_API double getLevel() const;

    /// Set the level of the selected tf
    FWCOMED_API void setLevel( double level );

protected:


    /// Constructor. Do nothing.
    FWCOMED_API MedicalImageAdaptor(); // this class VISUVTKADAPTOR_CLASS_API must be specialized

    /**
     * @brief Set the orientation of the image.
     * @param[in] orientation must be 0 (X_AXIS), 1 (Y_AXIS) or 2 (Z_AXIS).
     */
    FWCOMED_API void setOrientation( int orientation );

    /**
     * @brief Get the image spacing.
     * @param[out] spacing : the image spacing
     */
    FWCOMED_API void getImageSpacing(double spacing[3]);

    /**
     * @brief Get the image origin.
     * @param[out] origin : the image origin
     */
    FWCOMED_API void getImageOrigin(double origin[3]);

    /**
     * @brief Get the image data size (number of slices).
     * @param[out] size : the image size
     */
    FWCOMED_API void getImageDataSize(int size[3]);

    /**
     * @brief Get the image size ( = dataSize * spacing ).
     * @param[out] size : the image size
     */
    FWCOMED_API void getImageSize(double size[3]);

    /**
     * @brief Get the slice center
     * @param[out] center : the slice center
     */
    FWCOMED_API void getCurrentSliceCenter(double center[3]);

    // Retrieve the grey level from an image from physical world
    //float getPixelvalue( double worldPosition[3]);

    /**
     * @brief Convert world coordinates to slice index coordinates
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the slice index
     */
    FWCOMED_API void worldToSliceIndex(const double world[3],int index[3] );

    /**
     * @brief Convert coordinates in the world to coordinates in the image
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the image
     */
    FWCOMED_API void worldToImageSliceIndex(const double world[3], int index[3] );

    /**
     * @brief Convert from world coordinates system to image coordinates system
     * @param[in] index : coordinate in the image
     * @param[out] world : coordinate in the world
     */
    FWCOMED_API void sliceIndexToWorld(const int index[3], double world[3] );

    /**
     * @brief Get the image spacing.
     * @param[out] spacing : the image spacing
     */
    template< typename FLOAT_ARRAY_3 >
    void getImageSpacing(FLOAT_ARRAY_3 spacing);

    /**
     * @brief Get the image data size (number of slices).
     * @param[out] size : the image size
     */
    template< typename INT_INDEX >
    void getImageDataSize(INT_INDEX size);

    /**
     * @brief Convert world coordinates to slice index coordinates
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the slice index
     */
    template< typename WORLD, typename INT_INDEX >
    void worldToSliceIndex(const WORLD world, INT_INDEX index );

    /**
     * @brief Convert coordinates in the world to coordinates in the image
     * @param[in] world : coordinate in the world
     * @param[out] index : coordinate in the image
     */
    template< typename WORLD, typename INT_INDEX >
    void worldToImageSliceIndex(const WORLD world, INT_INDEX index );




    /**
     * @brief Return the 4 points of the image plane
     *
     * - points are ordered in the following way : image origin is the first point
     * - points are inserted using the preference follow X axis if exists, if not exists follow Y axis
     * if Y axis is not present follow Z axis
     */
    FWCOMED_API void getPlane( double points[4][3], int sliceNumber );

    /// Set the slice index
    FWCOMED_API bool setSliceIndex(const int index[3]);
    /// Get the slice index
    FWCOMED_API void getSliceIndex(::fwData::Integer::sptr index[3]);

    /// Get the current transfer function
    FWCOMED_API ::fwData::TransfertFunction_VERSION_II::sptr getTransferFunction() const;

    /// Update the image information (slice index, min/max, TF,...)
    FWCOMED_API void updateImageInfos( ::fwData::Image::sptr image  );

    /// Return the image
    ::fwData::Image::sptr getImage();

    /// Image orientation
    Orientation m_orientation;

    /// Current image
    ::fwData::Image::wptr m_weakImage;

    /// Axial slice index
    ::fwData::Integer::sptr m_axialIndex;
    /// Frontal slice index
    ::fwData::Integer::sptr m_frontalIndex;
    /// Sagittal slice index
    ::fwData::Integer::sptr m_sagittalIndex;

    // Install TF Observer ( com channel )
    FWCOMED_API void installTFObserver( ::fwServices::IService::sptr srv );

    // Remove TF Observer ( com channel )
    FWCOMED_API void removeTFObserver();

    // Helper to send a windowing notification in the current tf
    FWCOMED_API ::fwComEd::TransferFunctionMsg::sptr notifyTFWindowing( ::fwServices::IService::sptr srv );

private :

    ::fwServices::IService::wptr m_tfComChannelSrv;

    /// Transfer function pool
    ::fwData::Composite::wptr m_tfPool;

    /// fwID of tf pool ( used during configuration )
    std::string m_tfPoolFwID;

    /**
     * @brief Identifier of the field containing the current selection of TransfertFunction.
     * by defaults use ::fwComEd::Dictionary::m_transfertFunctionId
     */
    std::string m_selectedTFKey;

};

struct Image0 {} ;
struct Image1 {} ;

template < typename IMAGEID >
class FWCOMED_CLASS_API MedicalImageAdaptorTpl : public MedicalImageAdaptor
{};

typedef MedicalImageAdaptorTpl<Image0> MedicalImageAdaptorImg0;
typedef MedicalImageAdaptorTpl<Image1> MedicalImageAdaptorImg1;


//------------------------------------------------------------------------------
template< typename FLOAT_ARRAY_3 >
void MedicalImageAdaptor::getImageSpacing(FLOAT_ARRAY_3 spacing)
{
    ::fwData::Image::sptr image = this->getImage();;

    const ::fwData::Image::SpacingType imSpacing = image->getSpacing();
    std::copy(imSpacing.begin(), imSpacing.end(), spacing);
}

//------------------------------------------------------------------------------
template< typename INT_INDEX >
void MedicalImageAdaptor::getImageDataSize(INT_INDEX size)
{
    ::fwData::Image::sptr image = this->getImage();

    const ::fwData::Image::SizeType imSize = image->getSize();
    std::copy(imSize.begin(), imSize.end(), size);
}

//------------------------------------------------------------------------------

template< typename WORLD, typename INT_INDEX >
void MedicalImageAdaptor::worldToSliceIndex(const WORLD world, INT_INDEX index )
{
    double spacing[3];
    this->getImageSpacing(spacing);
    double origin[3];
    this->getImageOrigin(origin);
    for ( int i=0 ; i<3 ; ++i )
    {
        index[i] = static_cast< int >( ( (world[i] - origin[i])/spacing[i] ) + ( ( (world[i] - origin[i])/spacing[i] ) >= 0 ? 0.5 : -0.5 ) );
    }
}

//------------------------------------------------------------------------------

template< typename WORLD, typename INT_INDEX >
void MedicalImageAdaptor::worldToImageSliceIndex(const WORLD world, INT_INDEX index )
{
    int imageSize[3];
    this->getImageDataSize(imageSize);
    this->worldToSliceIndex(world, index);

    int idval;
    for (int i = 0; i < 3; i++)
    {
        int max = imageSize[i]-1;
        idval = index[i];
        if (idval < 0)
        {
            index[i] = 0;
        }
        else if (idval > max)
        {
            index[i] = max;
        }
    }
}

} //namespace helper

} //namespace fwComEd

#endif // _FWCOMED_MEDICALIMAGEADAPTOR_HPP_

