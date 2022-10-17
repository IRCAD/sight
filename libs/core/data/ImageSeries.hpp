/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#include "data/config.hpp"
#include "data/DicomSeries.hpp"
#include "data/factory/new.hpp"
#include "data/Image.hpp"
#include "data/Series.hpp"
#include "data/types.hpp"

namespace sight::data
{

class Image;

} // namespace sight::data

namespace sight::data
{

/**
 * @brief Holds a medical images data.
 */
class DATA_CLASS_API ImageSeries final : public Image,
                                         public Series
{
public:

    SIGHT_DECLARE_CLASS(ImageSeries, Image, factory::New<ImageSeries>);

    /**
     * @brief Creates the series.
     * @param _key private construction key.
     */
    DATA_API ImageSeries(Object::Key _key);

    /// Destroys the series.
    DATA_API ~ImageSeries() noexcept override = default;

    /// Gets the DICOM reference used to generate valid Dicom Segmentation.
    DicomSeries::csptr getDicomReference() const;

    /// Sets the DICOM reference used to generate valid Dicom Segmentation.
    void setDicomReference(const DicomSeries::csptr& _reference);

    /// Getter/Setter of DICOM VOI LUT Module Module related attributes
    /// @note the definition is in Series.cpp
    /// @note Image also implements a getter/setter for this attribute that is overriden here
    /// @{
    DATA_API void setWindowCenter(const std::vector<double>& windowCenters) override;
    DATA_API std::vector<double> getWindowCenter() const noexcept override;
    DATA_API void setWindowWidth(const std::vector<double>& windowWidths) override;
    DATA_API std::vector<double> getWindowWidth() const noexcept override;
    /// @}

    /// Getter/Setter of DICOM Image Pixel Module
    /// @note the definition is in Series.cpp
    /// @note Image also somewhat implements a getter/setter for this attribute
    /// @{
    DATA_API void setRows(const std::optional<std::uint16_t>& rows       = std::nullopt) override;
    DATA_API void setColumns(const std::optional<std::uint16_t>& columns = std::nullopt) override;
    /// @}

    /// Equality comparison operators
    /// @{
    DATA_API bool operator==(const ImageSeries& other) const noexcept;
    DATA_API bool operator!=(const ImageSeries& other) const noexcept;
    /// @}

    /// Defines shallow copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param[in] source the source object to copy
    DATA_API void shallowCopy(const Object::csptr& source) override;

    /// Defines deep copy
    /// @throws data::Exception if an errors occurs during copy
    /// @param source source object to copy
    /// @param cache cache used to deduplicate pointers
    DATA_API void deepCopy(
        const Object::csptr& source,
        const std::unique_ptr<DeepCopyCacheType>& cache = std::make_unique<DeepCopyCacheType>()
    ) override;

private:

    /// Contains the DICOM reference used to generate a valid DICOM Segmentation.
    DicomSeries::sptr m_dicomReference;
};

//-----------------------------------------------------------------------------

inline DicomSeries::csptr ImageSeries::getDicomReference() const
{
    return m_dicomReference;
}

//-----------------------------------------------------------------------------

inline void ImageSeries::setDicomReference(const DicomSeries::csptr& _reference)
{
    m_dicomReference = std::const_pointer_cast<DicomSeries>(_reference);
}

} //end namespace sight::data.
