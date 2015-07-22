/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_HELPER_FILTER_HPP__
#define __FWDICOMIOFILTER_HELPER_FILTER_HPP__

#include <fwDicomData/DicomSeries.hpp>

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/config.hpp"

namespace fwDicomIOFilter
{
namespace helper
{

/**
 * @class Filter
 * @brief Filter Helper
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API Filter
{
public:
    typedef std::vector< ::fwDicomData::DicomSeries::sptr > DicomSeriesContainerType;

    /**
     * @brief Apply a filter to the DicomSeries
     * @param[in,out] dicomSeriesContainer DicomSeries that must be filtered
     * @param[in] filter Filter that must be applied
     */
    FWDICOMIOFILTER_API static void applyFilter(DicomSeriesContainerType& dicomSeriesContainer,
                                                ::fwDicomIOFilter::IFilter::sptr filter, bool forcedApply = false);

};

} // namespace helper
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_HELPER_FILTER_HPP__ */
