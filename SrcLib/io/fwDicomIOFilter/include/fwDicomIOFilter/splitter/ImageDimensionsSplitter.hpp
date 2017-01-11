/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SPLITTER_IMAGEDIMENSIONSSPLITTER_HPP__
#define __FWDICOMIOFILTER_SPLITTER_IMAGEDIMENSIONSSPLITTER_HPP__

#include "fwDicomIOFilter/splitter/ISplitter.hpp"
#include "fwDicomIOFilter/config.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @brief Filter that uses the image dimensions to split the instances
 */
class FWDICOMIOFILTER_CLASS_API ImageDimensionsSplitter : public ISplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (ImageDimensionsSplitter)(ISplitter),
                                            (()), ::fwDicomIOFilter::factory::New< ImageDimensionsSplitter > );

    /// Constructor
    FWDICOMIOFILTER_API ImageDimensionsSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~ImageDimensionsSplitter();

    /// Override
    FWDICOMIOFILTER_API virtual DicomSeriesContainerType apply(
        const ::fwMedData::DicomSeries::sptr& series, const ::fwLog::Logger::sptr& logger) const
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

} // namespace splitter
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_SPLITTER_IMAGEDIMENSIONSSPLITTER_HPP__ */
