/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWDICOMIOFILTERQT_SORTER_TAGVALUECONFIGURABLESORTER_HPP__
#define __FWDICOMIOFILTERQT_SORTER_TAGVALUECONFIGURABLESORTER_HPP__

#include "fwDicomIOFilterQt/config.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwDicomIOFilter/sorter/TagValueSorter.hpp>

namespace fwDicomIOFilterQt
{
namespace sorter
{

/**
 * @brief Filter that uses a tag to sort the instances.
 */
class FWDICOMIOFILTERQT_CLASS_API TagValueConfigurableSorter : public ::fwDicomIOFilter::sorter::TagValueSorter
{
public:
    fwCoreClassDefinitionsWithFactoryMacro( (TagValueConfigurableSorter)(::fwDicomIOFilter::sorter::TagValueSorter),
                                            (()), ::fwDicomIOFilter::factory::New< TagValueConfigurableSorter > );

    /// Constructor
    FWDICOMIOFILTERQT_API TagValueConfigurableSorter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTERQT_API virtual ~TagValueConfigurableSorter();

    /// Return the name of the filter
    FWDICOMIOFILTERQT_API virtual std::string getName() const;

    /// Return the description of the filter
    FWDICOMIOFILTERQT_API virtual std::string getDescription() const;

    /// Return true if a configuration is required
    FWDICOMIOFILTERQT_API virtual bool isConfigurationRequired() const;

    /// Return true if the filter is configurable using GUI
    FWDICOMIOFILTERQT_API virtual bool isConfigurableWithGUI() const;

    /// Configure the filter using GUI dialog
    FWDICOMIOFILTERQT_API virtual void configureWithGUI();

protected:
    /// Filter name
    FWDICOMIOFILTERQT_API static const std::string s_FILTER_NAME;

    /// Filter description
    FWDICOMIOFILTERQT_API static const std::string s_FILTER_DESCRIPTION;

};

} // namespace sorter
} // namespace fwDicomIOFilterQt


#endif /* __FWDICOMIOFILTERQT_SORTER_TAGVALUECONFIGURABLESORTER_HPP__ */
