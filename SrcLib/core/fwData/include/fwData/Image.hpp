/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _FWDATA_IMAGE_HPP_
#define _FWDATA_IMAGE_HPP_

#include <vector>

#include <boost/filesystem/path.hpp>
#include <boost/cstdint.hpp>

#include <fwTools/DynamicType.hpp>

#include "fwData/Object.hpp"
#include "fwData/IBufferDelegate.hpp"


namespace fwData
{

/**
 * @class   Image
 * @brief   This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 *
 * @author  IRCAD (Research and Development Team).
 * @date    2007-2009.
 */
class FWDATA_CLASS_API Image : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Image)(::fwData::Object), (()), ::fwTools::Factory::New< Image > ) ;
    fwCoreAllowSharedFromThis();

    typedef ::boost::uint64_t VoxelIndexType;
    typedef ::boost::uint64_t BufferIndexType;
    typedef ::boost::uint8_t  BufferType;
    /**
     * @brief Constructor
     */
    FWDATA_API Image();

    /**
     * @brief Destructor
     */
    FWDATA_API virtual ~Image() throw();


    fwDataObjectMacro();

    /// Defines shallow copy
    FWDATA_API void shallowCopy( Image::csptr _source );

    /// Defines deep copy
    FWDATA_API void deepCopy( Image::csptr _source );

    /// image get image information from source. Informations are spacing,origin,size ... expect Fields and bufferDelegate
    FWDATA_API void getInformation( Image::csptr _source );


    /**
     * @brief an R/W accessor to the buffer (can involve unserialization/restoration)
     */
    FWDATA_API void * getBuffer() const;

    /**
     * @brief Essentially used by ImageIOInventory to restore the image buffer
     * @note launch a imageModifiedEvent if _buffer is not NULL
     */
    FWDATA_API void setBuffer(void *_buffer);


    // Generator result---------------------------------------------------------

    /** @name ManagesBuff accessor */
    /**
     * @brief Get/Set if buffer is managed.
     *
     * True if buffer is managed.
     * @{
     */
    FWDATA_API void setManagesBuff( const bool _bManagesBuff );
    FWDATA_API void setCRefManagesBuff( const bool & _bManagesBuff );
    FWDATA_API const bool getManagesBuff() const;
    FWDATA_API const bool & getCRefManagesBuff() const;
    FWDATA_API bool & getRefManagesBuff();
    //@}

    fwGettersSettersDocMacro(Dimension, ui8Dimension, ::boost::uint8_t, the image dimension (3 for 3D image));

    fwGettersSettersDocMacro(PixelType, dtPixelType, ::fwTools::DynamicType, type of image pixel (float, short, ...));

    fwGettersSettersDocMacro(Spacing, vSpacing, std::vector<double>, an array on the voxel size of the image);

    fwGettersSettersDocMacro(Origin, vOrigin, std::vector<double>, the origin of the image in 3D repair);

    fwGettersSettersDocMacro(Size, vSize, std::vector<boost::int32_t>, the size of the image (in terms of points));

    fwGettersSettersDocMacro(Filename, fsFilename, ::boost::filesystem::path, the path to save/load this image);

    fwGettersSettersDocMacro(WindowCenter, dWindowCenter, double, window level);

    fwGettersSettersDocMacro(WindowWidth, dWindowWidth, double, window width);

    fwGettersSettersDocMacro(RescaleIntercept, dRescaleIntercept, double, rescale intercept);

    fwGettersSettersDocMacro(BufferDelegate, bufferDelegate, ::fwData::IBufferDelegate::sptr, by default use StandardBuffer);

    FWDATA_API void* getPixelBuffer( ::boost::int32_t x, ::boost::int32_t y, ::boost::int32_t z );
    FWDATA_API void* getPixelBuffer( VoxelIndexType index );
    FWDATA_API ::boost::shared_ptr< BufferType > getPixelBufferCopy( ::boost::int32_t x, ::boost::int32_t y, ::boost::int32_t z );
    FWDATA_API ::boost::shared_ptr< BufferType > getPixelBufferCopy( VoxelIndexType index );

    FWDATA_API void setPixelBuffer( VoxelIndexType index , Image::BufferType * pixBuf);


protected :

    //! image dimension, ex : 3 for image 3D
    ::boost::uint8_t  m_ui8Dimension;

    //! type of image pixel
    ::fwTools::DynamicType m_dtPixelType;

    //! An array on the voxel size of the image
    std::vector< double > m_vSpacing;

    //! Origin of the image in 3D repair
    std::vector< double > m_vOrigin;

    //! Size of the image (in terms of points)
    std::vector< ::boost::int32_t >  m_vSize;

    //! the path to save/load this image
    ::boost::filesystem::path m_fsFilename;

    /** @{
     * @brief Visu control to adjust contrast image (come from dicom image ?)
     */
    double m_dWindowCenter;
    double m_dWindowWidth;
    double m_dRescaleIntercept;
    // @}

    //! image buffer
    ::fwData::IBufferDelegate::sptr  m_bufferDelegate;

};

/// @brief return the size in bytes of the image
FWDATA_API boost::int32_t  imageSizeInBytes( const ::fwData::Image &image);

/**
 * @brief Get a string containing pixel value
 * @param[in] _image image containing the pixel
 * @param[in] _x x coordinate of the pixel
 * @param[in] _y y coordinate of the pixel
 * @param[in] _z z coordinate of the pixel
 *
 * @return pixel value
 */
FWDATA_API std::string  getPixelAsString( ::fwData::Image::csptr _image, unsigned int _x, unsigned int _y, unsigned int _z );

};

#endif //_FWDATA_IMAGE_HPP_

