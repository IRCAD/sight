/************************************************************************
 *
 * Copyright (C) 2009-2019 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#pragma once

#include "fwDicomIOFilterQt/config.hpp"

#include <fwDicomIOFilter/splitter/TagValueInstanceRemoveSplitter.hpp>

#include <fwMedData/DicomSeries.hpp>

namespace fwDicomIOFilterQt
{
namespace splitter
{

/**
 * @brief Filter that remove instances if a tag has a certain value.
 */
class FWDICOMIOFILTERQT_CLASS_API TagValueInstanceRemoveConfigurableSplitter :
    public ::fwDicomIOFilter::splitter::TagValueInstanceRemoveSplitter
{
public:
    fwCoreClassMacro(TagValueInstanceRemoveConfigurableSplitter,
                     ::fwDicomIOFilter::splitter::TagValueInstanceRemoveSplitter,
                     ::fwDicomIOFilter::factory::New< TagValueInstanceRemoveConfigurableSplitter >);

    /// Constructor
    FWDICOMIOFILTERQT_API TagValueInstanceRemoveConfigurableSplitter(::fwDicomIOFilter::IFilter::Key key);

    /// Destructor
    FWDICOMIOFILTERQT_API virtual ~TagValueInstanceRemoveConfigurableSplitter();

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
