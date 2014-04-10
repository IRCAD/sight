/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2014.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __GDCMIO_HELPER_DICOMDATA_HPP__
#define __GDCMIO_HELPER_DICOMDATA_HPP__

#include <boost/algorithm/string/trim.hpp>

// GDCM
#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmImage.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmSurface.h>
#include <gdcmString.h>

// fwTools
#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

// fwData
#include <fwData/Image.hpp>
#include <fwData/Material.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/exception/Failed.hpp"
#include "gdcmIO/container/DicomSCoord.hpp"
#include "gdcmIO/container/DicomCodedAttribute.hpp"

namespace gdcmIO
{

namespace helper
{

/**
 * @class DicomData
 * @brief This class contains helpers to handle GDCM data.
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to FW4SPL data.
 */
class GDCMIO_CLASS_API DicomData
{
public:

    /**
     * @brief Return a string from a tag found in \p dataset.
     * An empty string returned means the tag is not found or empty.
     * The value is returned without binary space padding.
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     * @return The tag value
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static const std::string getTagValue(const ::gdcm::DataSet &dataset)
    {
        if (dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            const ::gdcm::DataElement &dataElement =
                    dataset.GetDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());

            if (!dataElement.IsEmpty()) // Can be type 2
            {
                std::stringstream ss;
                dataElement.GetValue().Print(ss);
                return ss.str();
            }
        }

        return "";
    }

    /**
     * @brief Return a trimmed string from a tag found in \p dataset
     * @param[in] dataset Data set of tags
     * @return The trimmed tag value
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static const std::string getTrimmedTagValue(const ::gdcm::DataSet &dataset)
    {
        std::string val = getTagValue< GROUP, ELEMENT >(dataset);
        ::boost::algorithm::trim(val);
        return val;
    }

    /**
     * @brief Return multiple values from a tag found in \p dataset.
     * A null pointer means tag is empty or not found.
     * @param[in] dataset Data set of tags.
     * @tparam GROUP Group of the tag.
     * @tparam ELEMENT Element of the tag.
     * @tparam T Type of value.
     * @return Tag values.
     */
    template< uint16_t GROUP, uint16_t ELEMENT, typename T >
    GDCMIO_TEMPLATE_API static const T* getTagValues(const ::gdcm::DataSet &dataset)
    {
        if (dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            ::gdcm::Attribute< GROUP, ELEMENT > gAt;
            gAt.SetFromDataElement(dataset.GetDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()));
            return gAt.GetValues();
        }
        return 0;
    }

    /**
     * @brief Return a value from a tag found in \p dataset.
     * @param[in] dataset Data set of tags.
     * @tparam GROUP Group of the tag.
     * @tparam ELEMENT Element of the tag.
     * @tparam T Type of value.
     * @return The tag value.
     */
    template< uint16_t GROUP, uint16_t ELEMENT, typename T >
    GDCMIO_TEMPLATE_API static const T getTagValue(const ::gdcm::DataSet &dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        attribute.SetFromDataSet(dataset);
        return attribute.GetValue();
    }

    /**
     * @brief Insert a string of character in a tag of a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static void setTagValue(const std::string &value, ::gdcm::DataSet &dataset)
    {
        // WARNING : Do not use DataElement (for space padding)
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        attribute.SetValue(value);
        dataset.Insert(attribute.GetAsDataElement());

    }

    /**
     * @brief Insert the value of a tag in a data set.
     * @param[in] value Value to set
     * @param[in] dataset Data set of tags
     * @tparam T Type of value
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< typename T, uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static void setTagValue(const T &value, ::gdcm::DataSet &dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        attribute.SetValue(value);
        dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Insert multiple values of a tag in a data set.
     * @param[in] array An Array.
     * @param[in] size Size of the array.
     * @param[in] dataset Data set of tags.
     * @tparam T Type of pointer.
     * @tparam GROUP Group of the tag.
     * @tparam ELEMENT Element of the tag.
     */
    template< typename T, uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static void setTagValues(const T * array, const unsigned int size, ::gdcm::DataSet &dataset)
    {
        ::gdcm::Attribute< GROUP, ELEMENT > attribute;
        if (array)
        {
            attribute.SetValues(array, size);
        }
        dataset.Insert(attribute.GetAsDataElement());
    }

    /**
     * @brief Set and insert a sequence of items with a tag in the data set specified.
     * @param[in] sequence Sequence of items to insert
     * @param[in] tag Tag of the sequence
     * @param[in] dataset Data set
     */
    GDCMIO_API static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence, const ::gdcm::Tag &tag,
            ::gdcm::DataSet &dataset);

    /**
     * @brief Set and insert a sequence of items with a tag in the data set specified.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
            ::gdcm::DataSet &dataset)
    {
        // Create the sequence of items
        ::gdcm::DataElement dataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());
        dataElement.SetVR(::gdcm::VR::SQ);
        dataElement.SetValue(*sequence);
        dataElement.SetVL(sequence->GetLength());

        // Insert the sequence of items
        dataset.Insert(dataElement);
    }

    /**
     * @brief Insert a sequence of items with a tag in the data set specified.
     * If the tag already exists, items of the old and new sequences are added in a new one.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     *
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    GDCMIO_TEMPLATE_API static void insertSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
            ::gdcm::DataSet &dataset)
    {
        // Set or add the SQ
        if (!dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            setSQ< GROUP, ELEMENT >(sequence, dataset);
        }
        else
        {
            // Get old SQ
            const ::gdcm::DataElement &dataElement =
                    dataset.GetDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > oldSequence = dataElement.GetValueAsSQ();

            // Add items of the new SQ to the old SQ
            const unsigned int nbItem = sequence->GetNumberOfItems();
            for (unsigned int i = 1; i <= nbItem; ++i) // WARN : item start at 1 TODO: Check that the item start at 1
            {
                oldSequence->AddItem(sequence->GetItem(i));
            }
        }
    }

    /**
     * @brief Return the pixel type of a gdcm Image.
     * @param[in] str String to convert
     * @param[in] obj The content of the string converted in T type.
     * @return return true if the conversion worked.
     */

    template< typename T > static bool valueOf(const std::string &str, T &obj)
    {
        std::istringstream is(str);
        return is >> obj;
    }

    /**
     * @brief Return the pixel type from a gdcm pixel format.
     * @param[in] pixelFormat Pixel format in gdcm.
     * @return The dynamic type of pixels (e.g: char, unsigned int, ...)
     */
    GDCMIO_API static const ::fwTools::Type getPixelType(const ::gdcm::PixelFormat &pixelFormat);

    /**
     * @brief Return the pixel type of a gdcm Image.
     * @param[in] image GDCM image
     * @return The dynamic type of pixels (e.g: char, unsigned int, ...)
     */
    GDCMIO_API static const ::fwTools::Type getPixelType(const ::gdcm::Image &image);

    /**
     * @brief Return the pixel type of a fwData Image.
     * @param[in] image Image of fwData
     * @return The pixel format (e.g: INT8, UINT16, ...)
     */
    GDCMIO_API static const ::gdcm::PixelFormat getPixelType(const ::fwData::Image::csptr image);

    /**
     * @brief Return the photometric interpretation of an acquisition.
     * @param[in] image Image
     * @return The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    GDCMIO_API static const ::gdcm::PhotometricInterpretation getPhotometricInterpretation(
            const ::fwData::Image &image);

    /**
     * @brief Convert a surface representation mode (FW4SPL) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] representationMode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    GDCMIO_API static ::gdcm::Surface::VIEWType convertToPresentationType(
            ::fwData::Material::REPRESENTATION_MODE representationMode);

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (FW4SPL).
     * @param[in] presentationType Surface recommended presentation type.
     * @return Corresponding representation mode
     */
    GDCMIO_API static ::fwData::Material::REPRESENTATION_MODE convertToRepresentationMode(
            ::gdcm::Surface::VIEWType presentationType);
};

} // namespace helper

} // namespace gdcmIO

#endif /*__GDCMIO_HELPER_DICOMDATA_HPP__*/
