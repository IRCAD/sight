/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__
#define __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/IFilter.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace composite
{

/**
 * @brief Base class for Dicom instance composite filter.
 */
class FWDICOMIOFILTER_CLASS_API IComposite : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IComposite)(::fwDicomIOFilter::IFilter) );

    typedef std::vector< ::fwDicomIOFilter::IFilter::sptr > FilterContainerType;

    /// Destructor
    FWDICOMIOFILTER_API virtual ~IComposite();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const override;

    /// Override
    FWDICOMIOFILTER_API DicomSeriesContainerType apply(
        const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const override;

    /**
     * @brief Apply every filters without throwing any exceptions.
     * If an exception is thrown, it is ignored and the next filter is applied.
     * @return Returns one or more Dicom Instance Group
     */
    FWDICOMIOFILTER_API DicomSeriesContainerType forcedApply(
        const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const;

    /// Add a filter to the composite
    FWDICOMIOFILTER_API void addChild(
        const ::fwDicomIOFilter::IFilter::sptr& filter);

    /// Remove a filter from the composite
    FWDICOMIOFILTER_API void removeChild(
        const ::fwDicomIOFilter::IFilter::sptr& filter);

    /// Return composite container
    FWDICOMIOFILTER_API FilterContainerType& getChildren();

protected:
    /// Constructor
    FWDICOMIOFILTER_API IComposite();

    /// Filter container
    FilterContainerType m_filterContainer;

};

} // namespace composite
} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__ */
