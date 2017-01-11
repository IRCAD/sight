/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_CUSTOM_DEFAULTDICOMFILTER_HPP__
#define __FWDICOMIOFILTER_CUSTOM_DEFAULTDICOMFILTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/custom/ICustom.hpp"

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilter
{
namespace custom
{

/**
 * @brief Default DICOM Filter
 */
class FWDICOMIOFILTER_CLASS_API DefaultDicomFilter : public ICustom
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (DefaultDicomFilter)(ICustom),
                                            (()), ::fwDicomIOFilter::factory::New< DefaultDicomFilter > );

    /// Constructor
    FWDICOMIOFILTER_API DefaultDicomFilter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~DefaultDicomFilter();

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

} // namespace custom
} // namespace fwDicomIOFilter


#endif /* __FWDICOMIOFILTER_CUSTOM_DEFAULTDICOMFILTER_HPP__ */
