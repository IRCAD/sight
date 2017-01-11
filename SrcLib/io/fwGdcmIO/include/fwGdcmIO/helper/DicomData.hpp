/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __FWGDCMIO_HELPER_DICOMDATA_HPP__
#define __FWGDCMIO_HELPER_DICOMDATA_HPP__

#include "fwGdcmIO/config.hpp"
#include "fwGdcmIO/container/DicomCodedAttribute.hpp"
#include "fwGdcmIO/exception/Failed.hpp"

#include <fwData/Image.hpp>
#include <fwData/Material.hpp>
#include <fwData/Point.hpp>
#include <fwTools/DynamicType.hpp>
#include <fwTools/Type.hpp>

#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmElement.h>
#include <gdcmImage.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmString.h>
#include <gdcmSurface.h>

#include <boost/algorithm/string/trim.hpp>

namespace fwGdcmIO
{

namespace helper
{

/**
 * @brief This class contains helpers to handle GDCM data.
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to FW4SPL data.
 */
class FWGDCMIO_CLASS_API DicomData
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
    static const std::string getTagValue(const ::gdcm::DataSet& dataset)
    {
        if (dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            const ::gdcm::DataElement& dataElement =
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
    static const std::string getTrimmedTagValue(const ::gdcm::DataSet& dataset)
    {
        std::string val = getTagValue< GROUP, ELEMENT >(dataset);
        ::boost::algorithm::trim(val);
        return val;
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
    static const T getTagValue(const ::gdcm::DataSet& dataset)
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
    static void setTagValue(const std::string& value, ::gdcm::DataSet& dataset)
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
    static void setTagValue(const T& value, ::gdcm::DataSet& dataset)
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
    static void setTagValues(const T* array, const unsigned int size, ::gdcm::DataSet& dataset)
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
    FWGDCMIO_API static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence, const ::gdcm::Tag& tag,
                                   ::gdcm::DataSet& dataset);

    /**
     * @brief Set and insert a sequence of items with a tag in the data set specified.
     * @param[in] sequence Sequence of items to insert
     * @param[in] dataset Data set
     * @tparam GROUP Group of the tag
     * @tparam ELEMENT Element of the tag
     */
    template< uint16_t GROUP, uint16_t ELEMENT >
    static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
                      ::gdcm::DataSet& dataset)
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
    static void insertSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems > sequence,
                         ::gdcm::DataSet& dataset)
    {
        // Set or add the SQ
        if (!dataset.FindDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag()))
        {
            setSQ< GROUP, ELEMENT >(sequence, dataset);
        }
        else
        {
            // Get old SQ
            const ::gdcm::DataElement& dataElement =
                dataset.GetDataElement(::gdcm::Attribute< GROUP, ELEMENT >::GetTag());
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > oldSequence = dataElement.GetValueAsSQ();

            // Add items of the new SQ to the old SQ
            auto nbItem = sequence->GetNumberOfItems();
            for (decltype(nbItem)i = 1; i <= nbItem; ++i)  // WARN : item start at 1
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
    template< typename T > static bool valueOf(const std::string& str, T& obj)
    {
        std::istringstream is(str);
        return is >> obj;
    }

    /**
     * @brief Return the pixel type from a gdcm pixel format.
     * @param[in] pixelFormat Pixel format in gdcm.
     * @return The dynamic type of pixels (e.g: char, unsigned int, ...)
     */
    FWGDCMIO_API static const ::fwTools::Type getPixelType(const ::gdcm::PixelFormat& pixelFormat);

    /**
     * @brief Return the pixel type of a gdcm Image.
     * @param[in] image GDCM image
     * @return The dynamic type of pixels (e.g: char, unsigned int, ...)
     */
    FWGDCMIO_API static const ::fwTools::Type getPixelType(const ::gdcm::Image& image);

    /**
     * @brief Return the pixel type of a fwData Image.
     * @param[in] image Image of fwData
     * @return The pixel format (e.g: INT8, UINT16, ...)
     */
    FWGDCMIO_API static const ::gdcm::PixelFormat getPixelType(const ::fwData::Image::csptr image);

    /**
     * @brief Return the photometric interpretation of an acquisition.
     * @param[in] image Image
     * @return The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    FWGDCMIO_API static const ::gdcm::PhotometricInterpretation getPhotometricInterpretation(
        const ::fwData::Image& image);

    /**
     * @brief Convert a surface representation mode (FW4SPL) into recommended presentation type (DICOM).
     * @see Recommended Presentation Type Tag (0066,000d).
     * @param[in] representationMode Surface representation mode.
     * @return Corresponding recommended presentation type.
     */
    FWGDCMIO_API static ::gdcm::Surface::VIEWType convertToPresentationType(
        ::fwData::Material::RepresentationType representationMode);

    /**
     * @brief Convert a surface recommended presentation type (DICOM) into representation mode (FW4SPL).
     * @param[in] presentationType Surface recommended presentation type.
     * @return Corresponding representation mode
     */
    FWGDCMIO_API static ::fwData::Material::RepresentationType convertToRepresentationMode(
        ::gdcm::Surface::VIEWType presentationType);

    /**
     * @brief Convert a representation mode into a DICOM form (eg : SURFACE, POINTS, WIREFRAME).
     * @see DICOM tag (0x0066,0x000d).
     * @param[in] representationMode Reconstruction's material representation mode (eg : SURFACE, POINT, ...).
     */
    FWGDCMIO_API static const char* convertToPresentationTypeString(
        ::fwData::Material::RepresentationType representationMode);

    /***
     * @brief Convert a 3D point to the closest frame number index
     * @param[in] image Image containing the point
     * @param[in] point 3D point
     */
    FWGDCMIO_API static int convertPointToFrameNumber(
        const ::fwData::Image::csptr& image, const ::fwData::Point::sptr& point) throw(::fwGdcmIO::exception::Failed);

    /***
     * @brief Convert a frame number to a Z coordinate
     * @param[in] image Image containing the point
     * @param[in] frameNumber Frame number
     */
    FWGDCMIO_API static double convertFrameNumberToZCoordinate(
        const ::fwData::Image::csptr& image, int frameNumber) throw(::fwGdcmIO::exception::Failed);
};

} // namespace helper

} // namespace fwGdcmIO

#endif /*__FWGDCMIO_HELPER_DICOMDATA_HPP__*/
