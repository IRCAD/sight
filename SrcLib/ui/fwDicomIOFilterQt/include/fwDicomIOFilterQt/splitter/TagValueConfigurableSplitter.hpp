/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "fwDicomIOFilterQt/config.hpp"

#include <fwDicomIOFilter/splitter/TagValueSplitter.hpp>

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilterQt
{
namespace splitter
{

/**
 * @brief Filter that uses a tag to split the instances.
 */
class FWDICOMIOFILTERQT_CLASS_API TagValueConfigurableSplitter : public ::fwDicomIOFilter::splitter::TagValueSplitter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagValueConfigurableSplitter)
                                            (::fwDicomIOFilter::splitter::TagValueSplitter),
                                            (()), ::fwDicomIOFilter::factory::New< TagValueConfigurableSplitter > );

    /// Constructor
    FWDICOMIOFILTERQT_API TagValueConfigurableSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTERQT_API virtual ~TagValueConfigurableSplitter();

    /// Return the name of the filter
    FWDICOMIOFILTERQT_API virtual std::string getName() const override;

    /// Return the description of the filter
    FWDICOMIOFILTERQT_API virtual std::string getDescription() const override;

    /// Return true if a configuration is required
    FWDICOMIOFILTERQT_API virtual bool isConfigurationRequired() const override;

    /// Return true if the filter is configurable using GUI
    FWDICOMIOFILTERQT_API virtual bool isConfigurableWithGUI() const override;

    /// Configure the filter using GUI dialog
    FWDICOMIOFILTERQT_API virtual void configureWithGUI() override;

protected:
    /// Filter name
    FWDICOMIOFILTERQT_API static const std::string s_FILTER_NAME;

    /// Filter description
    FWDICOMIOFILTERQT_API static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace fwDicomIOFilterQt
