/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__
#define __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__

#include <fwDicomData/DicomSeries.hpp>

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/config.hpp"

namespace fwDicomIOFilter
{
namespace composite
{

/**
 * @class IComposite
 * @brief Base class for Dicom instance composite filter.
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API IComposite : public ::fwDicomIOFilter::IFilter
{
public:
    fwCoreNonInstanciableClassDefinitionsMacro( (IComposite)(::fwDicomIOFilter::IFilter) );

    typedef std::vector< ::fwDicomIOFilter::IFilter::sptr > FilterContainerType;

    /// Destructor
    FWDICOMIOFILTER_API virtual ~IComposite();

    /// Return filter type
    FWDICOMIOFILTER_API FilterType getFilterType() const;

    /// Override
    FWDICOMIOFILTER_API DicomSeriesContainerType apply(::fwDicomData::DicomSeries::sptr series) const
    throw(::fwDicomIOFilter::exceptions::FilterFailure);

    /**
     * @brief Apply every filters without throwing any exceptions.
     * If an exception is thrown, it is ignored and the next filter is applied.
     * @return Returns one or more Dicom Instance Group
     */
    FWDICOMIOFILTER_API DicomSeriesContainerType forcedApply(::fwDicomData::DicomSeries::sptr series) const;

    /// Add a filter to the composite
    FWDICOMIOFILTER_API void addChild(::fwDicomIOFilter::IFilter::sptr filter);

    /// Remove a filter from the composite
    FWDICOMIOFILTER_API void removeChild(::fwDicomIOFilter::IFilter::sptr filter);

    /// Return composite container
    FWDICOMIOFILTER_API FilterContainerType &getChildren();

protected:
    /// Constructor
    FWDICOMIOFILTER_API IComposite();

    /// Filter container
    FilterContainerType m_filterContainer;

};

} // namespace composite
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_COMPOSITE_ICOMPOSITE_HPP__ */
