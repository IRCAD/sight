/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

#include "ui/dicom/config.hpp"

#include <data/DicomSeries.hpp>

#include <filter/dicom/splitter/TagValueSplitter.hpp>

namespace sight::ui::dicom
{
namespace splitter
{

/**
 * @brief Filter that uses a tag to split the instances.
 */
class UI_DICOM_CLASS_API TagValueConfigurableSplitter : public filter::dicom::splitter::TagValueSplitter
{
public:
    SIGHT_DECLARE_CLASS(TagValueConfigurableSplitter, filter::dicom::splitter::TagValueSplitter,
                        filter::dicom::factory::New< TagValueConfigurableSplitter >);

    /// Constructor
    UI_DICOM_API TagValueConfigurableSplitter(filter::dicom::IFilter::Key key);

    /// Destructor
    UI_DICOM_API virtual ~TagValueConfigurableSplitter();

    /// Return the name of the filter
    UI_DICOM_API virtual std::string getName() const override;

    /// Return the description of the filter
    UI_DICOM_API virtual std::string getDescription() const override;

    /// Return true if a configuration is required
    UI_DICOM_API virtual bool isConfigurationRequired() const override;

    /// Return true if the filter is configurable using GUI
    UI_DICOM_API virtual bool isConfigurableWithGUI() const override;

    /// Configure the filter using GUI dialog
    UI_DICOM_API virtual void configureWithGUI() override;

protected:
    /// Filter name
    UI_DICOM_API static const std::string s_FILTER_NAME;

    /// Filter description
    UI_DICOM_API static const std::string s_FILTER_DESCRIPTION;

};

} // namespace splitter
} // namespace sight::ui::dicom
