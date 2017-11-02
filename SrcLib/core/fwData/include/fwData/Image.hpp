/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDATA_IMAGE_HPP__
#define __FWDATA_IMAGE_HPP__

#include "fwData/Array.hpp"
#include "fwData/factory/new.hpp"
#include "fwData/Object.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

#include <boost/filesystem/path.hpp>
#include <boost/shared_array.hpp>

#include <vector>

fwCampAutoDeclareDataMacro((fwData)(Image), FWDATA_API);

namespace fwData
{

class Point;
class PointList;

/**
 * @brief This class defines an image
 *
 * An image contains a buffer and is defined by some parameters (size, spacing, pixel type, ...)
 */
class FWDATA_CLASS_API Image : public Object
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (Image)(::fwData::Object), (()), ::fwData::factory::New< Image > );
    fwCoreAllowSharedFromThis();
    fwCampMakeFriendDataMacro((fwData)(Image));

    /**
     * @brief Image size type
     */
    typedef ::fwData::Array::SizeType SizeType;

    /**
     * @brief Image spacing type
     */
    typedef std::vector< double > SpacingType;

    /**
     * @brief Image origin type
     */
    typedef std::vector< double > OriginType;

    typedef SizeType::value_type IndexType;
    typedef size_t BufferIndexType;
    typedef std::uint8_t BufferType;
    typedef ::boost::shared_array< BufferType > SharedArray;

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWDATA_API Image(::fwData::Object::Key key);

    /**
     * @brief Destructor
     */
    FWDATA_API virtual ~Image() noexcept;

    /// Defines shallow copy
    FWDATA_API void shallowCopy( const Object::csptr& _source ) override;

    /// Defines deep copy
    FWDATA_API void cachedDeepCopy(const Object::csptr& _source, DeepCopyCacheType& cache) override;

    /// @brief get image information from source. Informations are spacing,origin,size ... expect Fields
    FWDATA_API void copyInformation( Image::csptr _source );

    /// Number of dimension of the image (3 for 3D image)
    FWDATA_API size_t getNumberOfDimensions() const;

    /** @{
     * @brief get/set image spacing
     */

    FWDATA_API const SpacingType& getSpacing() const;
    FWDATA_API void setSpacing(const SpacingType& spacing);
    /// @}

    /** @{
     *  @brief get/set image origin
     */
    FWDATA_API const OriginType& getOrigin() const;
    FWDATA_API void setOrigin(const OriginType& origin);
    /// @}

    /** @{
     * @brief get/set image size
     */
    FWDATA_API const SizeType& getSize() const;
    FWDATA_API void setSize(const SizeType& size);
    /// @}

    /** @{
     *  @brief Get/set preferred window center
     */
    double  getWindowCenter () const;

    void setWindowCenter (double val);
    /// @}

    /** @{
     *  @brief Get/set preferred window width
     */
    double  getWindowWidth () const;
    void setWindowWidth (double val);
    /// @}

    /** @{
     *  @brief Get/set preferred window center
     */
    size_t  getNumberOfComponents() const;

    void setNumberOfComponents(size_t val);
    /// @}

    /**
     * @brief set data array
     *
     * @param[in] array data array
     * @param[in] copyArrayInfo if true, the image will copy the size and type information from the array
     *
     */
    FWDATA_API void setDataArray(::fwData::Array::sptr array, bool copyArrayInfo = true);

    ///get data array
    FWDATA_API ::fwData::Array::sptr getDataArray() const;

    /** @{
     * @brief get/set image type
     */
    FWDATA_API void setType(::fwTools::Type type);
    FWDATA_API void setType(const std::string& type);
    FWDATA_API ::fwTools::Type getType() const;
    /// @}

    /// get a DynamicType for retrocompatibility
    FWDATA_API ::fwTools::DynamicType getPixelType() const;

    /** @{
     * @brief Allocate image
     *
     * If the data array owns its buffer, these methods will always work (until it remain free memory)
     * Otherwise an exception is thrown :
     *  - if m_dataArray does not own it buffer and image's size and type combination do not match anymore array's one
     *  - if there is no memory left
     *
     * @return Allocated size in bytes
     */
    FWDATA_API size_t allocate();
    FWDATA_API size_t allocate(SizeType::value_type x, SizeType::value_type y,  SizeType::value_type z,
                               const ::fwTools::Type& type, size_t numberOfComponents = 1);
    FWDATA_API size_t allocate(const SizeType& size, const ::fwTools::Type& type, size_t numberOfComponents = 1);
    /// @}

    /// @brief return image size in bytes
    FWDATA_API size_t getSizeInBytes() const;
    /// @brief return allocated image size in bytes
    FWDATA_API size_t getAllocatedSizeInBytes() const;

    /**
     * @name Signals
     * @{
     */
    /// Type of signal when image's buffer is added
    typedef ::fwCom::Signal< void () > BufferModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_BUFFER_MODIFIED_SIG;

    /// Type of signal when a landmark is added
    typedef ::fwCom::Signal< void (SPTR(::fwData::Point)) > LandmarkAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_ADDED_SIG;

    /// Type of signal when a landmark is removed
    typedef ::fwCom::Signal< void (SPTR(::fwData::Point)) > LandmarkRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_REMOVED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (bool) > LandmarkDisplayedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_LANDMARK_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (bool) > DistanceDisplayedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_DISPLAYED_SIG;

    /// Type of signal when a distance is added
    typedef ::fwCom::Signal< void (SPTR(::fwData::PointList)) > DistanceAddedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_ADDED_SIG;

    /// Type of signal when a distance is removed
    typedef ::fwCom::Signal< void (CSPTR(::fwData::PointList)) > DistanceRemovedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_DISTANCE_REMOVED_SIG;

    /// Type of signal when slice index is modified (axial index, frontal index, sagittal index)
    typedef ::fwCom::Signal< void (int, int, int) > SliceIndexModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SLICE_INDEX_MODIFIED_SIG;

    /// Type of signal when slice type is modified (from slice type, to slice type)
    typedef ::fwCom::Signal< void (int, int) > SliceTypeModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_SLICE_TYPE_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef ::fwCom::Signal< void (bool) > VisibilityModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_VISIBILITY_MODIFIED_SIG;

    /// Type of signal when visibility is modified
    typedef ::fwCom::Signal< void () > TransparencyModifiedSignalType;
    FWDATA_API static const ::fwCom::Signals::SignalKeyType s_TRANSPARENCY_MODIFIED_SIG;

    /**
     * @}
     */

protected:

    //! Size of the image (in terms of points)
    SizeType m_size;

    //! type of image pixel
    ::fwTools::Type m_type;

    //! An array on the voxel size of the image
    SpacingType m_spacing;

    //! Origin of the image in 3D repair
    OriginType m_origin;

    //! Preferred window center/with
    ///@{
    double m_windowCenter;
    double m_windowWidth;
    ///@}

    //! Number of components
    size_t m_numberOfComponents;

    //! image buffer
    ::fwData::Array::sptr m_dataArray;
};

//-----------------------------------------------------------------------------

inline double Image::getWindowCenter () const
{
    return m_windowCenter;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowCenter (double val)
{
    m_windowCenter = val;
}

//-----------------------------------------------------------------------------

inline double Image::getWindowWidth () const
{
    return m_windowWidth;
}

//-----------------------------------------------------------------------------

inline void Image::setWindowWidth (double val)
{
    m_windowWidth = val;
}

//-----------------------------------------------------------------------------

inline size_t Image::getNumberOfComponents() const
{
    return m_numberOfComponents;
}

//-----------------------------------------------------------------------------

inline void Image::setNumberOfComponents(size_t val)
{
    m_numberOfComponents = val;
}

//-----------------------------------------------------------------------------

} // namespace fwData

#endif // __FWDATA_IMAGE_HPP__

