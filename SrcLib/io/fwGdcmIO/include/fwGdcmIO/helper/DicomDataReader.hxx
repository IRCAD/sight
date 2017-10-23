/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMDATAREADER_HXX__
#define __FWGDCMIO_HELPER_DICOMDATAREADER_HXX__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/helper/Encoding.hpp"

#include <fwLog/Logger.hpp>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief This class contains helpers to handle GDCM data reading.
 */
class FWGDCMIO_CLASS_API DicomDataReader
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
     * definied and tag's VR is SH, LO, ST, PN, LT or UT.
     * @return The tag value as string
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static std::string getTagValue(const ::gdcm::DataSet &dataset,
                                   const std::string &charset = "",
                                   const ::fwLog::Logger::sptr& logger = nullptr)
    {
        std::string result = "";

        const ::gdcm::Tag tag(GROUP, ELEMENT);

        if (dataset.FindDataElement(tag))
        {
            const ::gdcm::DataElement &dataElement = dataset.GetDataElement(tag);

            if (!dataElement.IsEmpty()) // Can be type 2
            {
                // Retrieve buffer
                const ::gdcm::ByteValue *bv = dataElement.GetByteValue();

                if(bv)
                {
                    std::string buffer(bv->GetPointer(), bv->GetLength());

                    // Trim buffer
                    const std::string trimmedBuffer = ::gdcm::LOComp::Trim(buffer.c_str());

                    try
                    {
                        result = ::fwGdcmIO::helper::Encoding::convertString(trimmedBuffer, charset, logger);
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
     * definied and tag's VR is SH, LO, ST, PN, LT or UT.
     * @return The tag value as string
     */
    template< std::uint16_t GROUP, std::uint16_t ELEMENT >
    static std::string getTagValue(const std::string& buffer,
                                   const std::string &charset = "",
                                   const ::fwLog::Logger::sptr& logger = 0)
    {
        std::string result = "";

        const ::gdcm::Tag tag = ::gdcm::Attribute< GROUP, ELEMENT >::GetTag();

        // Trim buffer
        const std::string trimmedBuffer = ::gdcm::LOComp::Trim(buffer.c_str());

        try
        {
            result = ::fwGdcmIO::helper::Encoding::convertString(trimmedBuffer, charset, logger);
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
    template< std::uint16_t GROUP, std::uint16_t ELEMENT, typename T >
    static const T getTagValue(const ::gdcm::DataSet &dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        attribute.SetFromDataSet(dataset);
        return attribute.GetValue();
    }

};

} // namespace helper
} // namespace fwGdcmIO

#endif // __FWGDCMIO_HELPER_DICOMDATAREADER_HXX__

