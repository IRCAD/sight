/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTER_SPLITTER_ACQUISITIONNUMBERSPLITTER_HPP__
#define __FWDICOMIOFILTER_SPLITTER_ACQUISITIONNUMBERSPLITTER_HPP__

#include "fwDicomIOFilter/config.hpp"
#include "fwDicomIOFilter/splitter/TagValueSplitter.hpp"

#include <string>

namespace fwDicomIOFilter
{
namespace splitter
{

/**
 * @brief Filter that uses the AcquisitionNumber tag to split the instances.
 */
class FWDICOMIOFILTER_CLASS_API AcquisitionNumberSplitter : public ::fwDicomIOFilter::splitter::TagValueSplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (AcquisitionNumberSplitter)(ISplitter),
                                            (()), ::fwDicomIOFilter::factory::New< AcquisitionNumberSplitter > );

    /// Constructor
    FWDICOMIOFILTER_API AcquisitionNumberSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTER_API virtual ~AcquisitionNumberSplitter();

    /// Return the name of the filter
    FWDICOMIOFILTER_API virtual std::string getName() const override;

    /// Return the description of the filter
    FWDICOMIOFILTER_API virtual std::string getDescription() const override;

    /// Return true if a configuration is required
    FWDICOMIOFILTER_API virtual bool isConfigurationRequired() const override;

protected:
    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace fwDicomIOFilter

#endif /* __FWDICOMIOFILTER_SPLITTER_ACQUISITIONNUMBERSPLITTER_HPP__ */
