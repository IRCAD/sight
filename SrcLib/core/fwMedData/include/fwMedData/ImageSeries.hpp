/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "fwMedData/config.hpp"
#include "fwMedData/DicomSeries.hpp"
#include "fwMedData/Series.hpp"
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
    fwCoreClassDefinitionsWithFactoryMacro( (ImageSeries)(::fwData::Object),
                                            ::fwData::factory::New< ImageSeries >);

    fwCampMakeFriendDataMacro((fwMedData)(ImageSeries));

    /**
     * @brief Constructor
     * @param key Private construction key
     */
    FWMEDDATA_API ImageSeries(::fwData::Object::Key key);

    /// Destructor
    FWMEDDATA_API virtual ~ImageSeries() override;

    /// Defines shallow copy
    FWMEDDATA_API void shallowCopy( const ::fwData::Object::csptr& _source ) override;

    /// Defines deep copy
    FWMEDDATA_API void cachedDeepCopy( const ::fwData::Object::csptr& _source, DeepCopyCacheType& cache ) override;

    /**
     * @name Getters / Setters
     * @{ */

    /**
     * @brief Image container
     * @{ */
    SPTR(::fwData::Image) getImage() const;
    void setImage(const SPTR(::fwData::Image)& val);
    /**  @} */

    /**
     * @brief Dicom reference use to generate valid Dicom Segmentation
     * @{ */
    ::fwMedData::DicomSeries::csptr getDicomReference() const;
    void setDicomReference(const ::fwMedData::DicomSeries::csptr& reference);
    /**  @} */

    /**  @} */

protected:

    /// Image container
    SPTR(::fwData::Image) m_image;

    /// Dicom reference used to generate a valid Dicom Segmentation
    ::fwMedData::DicomSeries::sptr m_dicomReference;
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

inline ::fwMedData::DicomSeries::csptr ImageSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setDicomReference(const ::fwMedData::DicomSeries::csptr& reference)
{
    m_dicomReference = std::const_pointer_cast< ::fwMedData::DicomSeries >( reference );
}

//-----------------------------------------------------------------------------

}   //end namespace fwMedData
