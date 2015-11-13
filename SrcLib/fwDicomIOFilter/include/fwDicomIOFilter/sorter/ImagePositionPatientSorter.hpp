/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SORTER_IMAGEPOSITIONPATIENTSORTER_HPP__
#define __FWDICOMIOFILTER_SORTER_IMAGEPOSITIONPATIENTSORTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/sorter/ISorter.hpp"

#include <fwDicomData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace sorter
{

/**
 * @class ImagePositionPatientSorter
 * @brief Filter that uses the ImagepositionPatient tag to sort the instances.
 * The position increases along the direction given by cross product of the ImageOrientationPatient vectors.
 * @date  2014.
 */
class FWDICOMIOFILTER_CLASS_API ImagePositionPatientSorter : public ISorter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImagePositionPatientSorter)(ISorter),
                                            (()), ::fwDicomIOFilter::factory::New< ImagePositionPatientSorter > );

    /// Constructor
    FWDICOMIOFILTER_API ImagePositionPatientSorter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ImagePositionPatientSorter();

    /// Override
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const ::fwDicomData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
    throw(::fwDicomIOFilter::exceptions::FilterFailure);

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const;

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

};

} // namespace sorter
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_SORTER_IMAGEPOSITIONPATIENTSORTER_HPP__ */
