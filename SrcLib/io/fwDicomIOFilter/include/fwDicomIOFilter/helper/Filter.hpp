/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_HELPER_FILTER_HPP__
#define __FWDICOMIOFILTER_HELPER_FILTER_HPP__

#include "fwDicomIOFilter/IFilter.hpp"
#include "fwDicomIOFilter/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace helper
{

/**
 * @brief Filter Helper
 */
class FWDICOMIOFILTER_CLASS_API Filter
{
public:
    typedef std::vector< ::fwMedData::DicomSeries::sptr > DicomSeriesContainerType;

    /**
     * @brief Apply a filter to the DicomSeries
     * @param[in,out] dicomSeriesContainer DicomSeries that must be filtered
     * @param[in] filter Filter that must be applied
     * @return Return true if errors have been ignored
     */
    FWDICOMIOFILTER_API static bool applyFilter(DicomSeriesContainerType& dicomSeriesContainer,
                                                ::fwDicomIOFilter::IFilter::sptr filter, bool forcedApply = false,
                                                const ::fwLog::Logger::sptr& logger = ::fwLog::Logger::New());

};

} // namespace helper
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_HELPER_FILTER_HPP__ */
