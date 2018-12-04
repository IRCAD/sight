/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/modifier/IModifier.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace modifier
{

/**
 * @brief Filter that uses the ImagepositionPatient tag to sort the instances.
 * The position increases along the direction given by cross product of the ImageOrientationPatient vectors.
 */
class FWDICOMIOFILTER_CLASS_API SliceThicknessModifier : public IModifier
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (SliceThicknessModifier)(IModifier),
                                            (()), ::fwDicomIOFilter::factory::New< SliceThicknessModifier > );

    /// Constructor
    FWDICOMIOFILTER_API SliceThicknessModifier(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~SliceThicknessModifier();

    /// Override
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const override;

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const override;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const override;

protected:
    /**
     * @brief Compute the Z coordinate of the slice according to the ImagePositionPatient and ImageOrientationPatient
     *  tags.
     *  @param[in] bufferObj BufferObject containing the slice
     */
    FWDICOMIOFILTER_API virtual double getInstanceZPosition(const ::fwMemory::BufferObject::sptr& bufferObj) const;

    /**
     * @brief Get the SliceThickness value from an instance.
     *  @param[in] bufferObj BufferObject containing the slice
     */
    FWDICOMIOFILTER_API virtual double getSliceThickness(const ::fwMemory::BufferObject::sptr& bufferObj) const;

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;
};

} // namespace modifier
} // namespace fwDicomIOFilter
