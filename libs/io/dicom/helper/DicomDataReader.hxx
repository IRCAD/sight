/************************************************************************
 *
 * Copyright (C) 2017-2023 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "io/dicom/config.hpp"
#include "io/dicom/helper/Encoding.hpp"

#include <core/log/logger.hpp>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>

namespace sight::io::dicom::helper
{

/**
 * @brief This class contains helpers to handle GDCM data reading.
 */
class IO_DICOM_CLASS_API DicomDataReader
{
public:

    /**
     * @brief Return a string from a tag found in \p dataset.
     * An empty string returned means the tag is not found or empty.
     * The value is returned without binary space padding.
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     * @param[in] dataset Data set containing the tag
     * @param[in] charset Specific Character Set (if empty assuming ASCII).
     * @param[in] logger Logger used for encoding issue
     * @note charset shall be set if SpecificCharacterSet (0008,0005) is
     * defined and tag's VR is SH, LO, ST, PN, LT or UT.
     * @return The tag value as string
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static std::string getTagValue(
        const gdcm::DataSet& dataset,
        const std::string& charset            = "",
        const core::log::logger::sptr& logger = nullptr
)
    {
        std::string result;

        const gdcm::Tag tag(GROUP, ELEMENT);

        if(dataset.FindDataElement(tag))
        {
            const gdcm::DataElement& dataElement = dataset.GetDataElement(tag);

            if(!dataElement.IsEmpty()) // Can be type 2
            {
                // Retrieve buffer
                const gdcm::ByteValue* bv = dataElement.GetByteValue();

                if(bv)
                {
                    std::string buffer(bv->GetPointer(), bv->GetLength());

                    // Trim buffer
                    const std::string trimmedBuffer = gdcm::LOComp::Trim(buffer.c_str());

                    try
                    {
                        result = io::dicom::helper::Encoding::convertString(trimmedBuffer, charset, logger);
                    }
                    catch(const std::runtime_error& e)
                    {
                        if(logger)
                        {
                            std::stringstream ss;
                            ss << "Could not read tag " << tag << " : " << e.what();
                            logger->warning(ss.str());
                        }

                        result = trimmedBuffer;
                    }
                }
            }
        }

        return result;
    }

    /**
     * @brief Return an utf-8 tag value from the tag value's buffer
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     * @param[in] buffer Tag value's buffer
     * @param[in] charset Specific Character Set (if empty assuming ASCII).
     * @param[in] logger Logger used for encoding issue
     * @note charset shall be set if SpecificCharacterSet (0008,0005) is
     * defined and tag's VR is SH, LO, ST, PN, LT or UT.
     * @return The tag value as string
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT>
    static std::string getTagValue(
        const std::string& buffer,
        const std::string& charset            = "",
        const core::log::logger::sptr& logger = nullptr
)
    {
        std::string result;

        const gdcm::Tag tag = gdcm::Attribute<GROUP, ELEMENT>::GetTag();

        // Trim buffer
        const std::string trimmedBuffer = gdcm::LOComp::Trim(buffer.c_str());

        try
        {
            result = io::dicom::helper::Encoding::convertString(trimmedBuffer, charset, logger);
        }
        catch(const std::runtime_error& e)
        {
            if(logger)
            {
                std::stringstream ss;
                ss << "Could not read tag " << tag << " : " << e.what();
                logger->warning(ss.str());
            }

            result = trimmedBuffer;
        }

        return result;
    }

    /**
     * @brief Return a value from a tag found in dataset.
     * @tparam GROUP Group of the tag.
     * @tparam ELEMENT Element of the tag.
     * @tparam T Type of value.
     * @param[in] dataset Data set of tags.
     * @return The tag value.
     */
    template<std::uint16_t GROUP, std::uint16_t ELEMENT, typename T>
    static T getTagValue(const gdcm::DataSet& dataset)
    {
        gdcm::Attribute<GROUP, ELEMENT> attribute {};
        attribute.SetFromDataSet(dataset);
        return attribute.GetValue();
    }
};

} // namespace sight::io::dicom::helper
