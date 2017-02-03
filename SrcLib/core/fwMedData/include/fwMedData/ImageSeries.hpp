/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWMEDDATA_IMAGESERIES_HPP__
#define __FWMEDDATA_IMAGESERIES_HPP__

#include "fwMedData/Series.hpp"
#include "fwMedData/config.hpp"
#include "fwMedData/types.hpp"

#include <fwData/factory/new.hpp>

fwCampAutoDeclareDataMacro((fwMedData)(ImageSeries), FWMEDDATA_API);

namespace fwData
{
class Image;
}

namespace fwMedData
{

/**
 * @brief Holds images data
 */
class FWMEDDATA_CLASS_API ImageSeries : public ::fwMedData::Series
{

public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageSeries)(::fwData::Object), (()),
                                            ::fwData::factory::New< ImageSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(ImageSeries));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ImageSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ImageSeries();

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source );

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache );

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Image container
     * @{ */
    SPTR(::fwData::Image) getImage() const;
    void setImage(const SPTR(::fwData::Image)& val);
    /**  @} */

    /**  @} */

protected:

    /// Image container
    SPTR(::fwData::Image) m_image;

};

//-----------------------------------------------------------------------------

inline SPTR(::fwData::Image) ImageSeries::getImage() const
{
    return m_image;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setImage(const SPTR(::fwData::Image)& val)
{
    m_image = val;
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData

#endif // __FWMEDDATA_IMAGESERIES_HPP__

