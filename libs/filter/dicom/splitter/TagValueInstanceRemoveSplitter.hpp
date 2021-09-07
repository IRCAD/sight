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

#include "filter/dicom/config.hpp"
#include "filter/dicom/splitter/ISplitter.hpp"

#include <data/DicomSeries.hpp>

#include <dcmtk/dcmdata/dctagkey.h>

namespace sight::filter::dicom
{

namespace splitter
{

/**
 * @brief Filter that remove instances according to tag value
 */
class FILTER_DICOM_CLASS_API TagValueInstanceRemoveSplitter : public ISplitter
{
public:

    SIGHT_DECLARE_CLASS(
        TagValueInstanceRemoveSplitter,
        ISplitter,
        filter::dicom::factory::New<TagValueInstanceRemoveSplitter>
    );

    /// Constructor
    FILTER_DICOM_API TagValueInstanceRemoveSplitter(filter::dicom::IFilter::Key key);

    /// Destructor
    FILTER_DICOM_API virtual ~TagValueInstanceRemoveSplitter();

    /// Override
    FILTER_DICOM_API virtual DicomSeriesContainerType apply(
        const data::DicomSeries::sptr& series,
        const core::log::Logger::sptr& logger
    ) const override;

    /// Return the name of the filter
    FILTER_DICOM_API std::string getName() const override;

    /// Return the description of the filter
    FILTER_DICOM_API std::string getDescription() const override;

    /// Return true if a configuration is required
    FILTER_DICOM_API bool isConfigurationRequired() const override;

    /**
     * @brief Tag used to sort instances
     * @{ */
    //------------------------------------------------------------------------------

    DcmTagKey& getTag()
    {
        return this->m_tag;
    }

    //------------------------------------------------------------------------------

    const DcmTagKey& getTag() const
    {
        return this->m_tag;
    }

    //------------------------------------------------------------------------------

    void setTag(const DcmTagKey& _tag)
    {
        this->m_tag = _tag;
    }

    /**  @} */

    /**
     * @brief Tag value used to determine if an instance must be removed
     * @{ */
    //------------------------------------------------------------------------------

    std::string& getTagValue()
    {
        return this->m_tagValue;
    }

    //------------------------------------------------------------------------------

    const std::string& getTagValue() const
    {
        return this->m_tagValue;
    }

    //------------------------------------------------------------------------------

    void setTagValue(const std::string& _tagValue)
    {
        this->m_tagValue = _tagValue;
    }

/**  @} */

protected:

    /// Filter name
    static const std::string s_FILTER_NAME;

    /// Filter description
    static const std::string s_FILTER_DESCRIPTION;

    /// Tag used to sort instances
    DcmTagKey m_tag;

    /// Tag value used to determine if an instance must be removed
    std::string m_tagValue;
};

} // namespace splitter

} // namespace sight::filter::dicom
