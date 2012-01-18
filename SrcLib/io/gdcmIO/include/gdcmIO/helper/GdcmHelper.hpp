/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef _GDCMIO_GDCMHELPER_HPP_
#define _GDCMIO_GDCMHELPER_HPP_

// GDCM
#include <gdcmAttribute.h>
#include <gdcmDataSet.h>
#include <gdcmImage.h>
#include <gdcmSequenceOfItems.h>
#include <gdcmSurface.h>
#include <gdcmString.h>

// fwTools
#include <fwTools/DynamicType.hpp>
#include <fwTools/Failed.hpp>

// fwData
#include <fwData/Acquisition.hpp>
#include <fwData/Material.hpp>

#include "gdcmIO/config.hpp"
#include "gdcmIO/DicomSCoord.hpp"
#include "gdcmIO/DicomCodedAttribute.hpp"

namespace gdcmIO
{

namespace helper
{

/**
 * @brief   This class contains helpers to handle GDCM data.
 *
 * It implements many tag value getters/setters. It defines
 * also some tools for converting GDCM data to FW4SPL one.
 *
 * @class   GdcmData
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API GdcmData
{
public:

    /**
     * @brief    Return a trimmed string from a tag found in \p a_ds.
     *
     * An empty string returned means the tag is not found or empty.
     * The value is returned without binary space padding.
     *
     * @param   a_tag   Dicom tag to set
     * @param   a_ds    Data set of tags
     *
     * @tparam  GRP     Group of the tag
     * @tparam  EL      Element of the tag
     *
     * @return  The tag value
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static const std::string getTagValue(const ::gdcm::DataSet & a_ds)
    {
        if (a_ds.FindDataElement( ::gdcm::Attribute<GRP,EL>::GetTag() ))
        {
            const ::gdcm::DataElement & de = a_ds.GetDataElement( ::gdcm::Attribute<GRP,EL>::GetTag() );
            if (!de.IsEmpty()) // Can be type 2
            {
                std::stringstream strm;
                de.GetValue().Print(strm);

                // Delete binary space padding
                ::gdcm::String<> trimStr( strm.str() );
                return trimStr.Trim();
            }
        }

        return "";
    }

    /**
     * @brief    Return multiple values from a tag found in \p a_ds.
     *
     * A null pointer means tag is empty or not found.
     *
     * @param   a_ds    Data set of tags.
     *
     * @tparam  GRP     Group of the tag.
     * @tparam  EL      Element of the tag.
     * @tparam  T       Type of value.
     *
     * @return  Tag values.
     */
    template <uint16_t GRP, uint16_t EL, typename T>
    GDCMIO_TEMPLATE_API static const T * getTagValues(const ::gdcm::DataSet & a_ds)
    {
        if (a_ds.FindDataElement( ::gdcm::Attribute<GRP, EL>::GetTag() ))
        {
            ::gdcm::Attribute<GRP, EL> gAt;
            gAt.SetFromDataElement( a_ds.GetDataElement( ::gdcm::Attribute<GRP, EL>::GetTag() ) );
            return gAt.GetValues();
        }
        return 0;
    }

    /**
     * @brief    Return a value from a tag found in \p a_ds.
     *
     * @param   a_ds    Data set of tags.
     *
     * @tparam  GRP     Group of the tag.
     * @tparam  EL      Element of the tag.
     * @tparam  T       Type of value.
     *
     * @return  The tag value.
     */
    template <uint16_t GRP, uint16_t EL, typename T>
    GDCMIO_TEMPLATE_API static const T getTagValue(const ::gdcm::DataSet & a_ds)
    {
        ::gdcm::Attribute<GRP, EL> gAt;
        gAt.SetFromDataSet( a_ds );
        return gAt.GetValue();
    }

    /**
     * @brief   Insert a string of character in a tag of a data set.
     *
     * @param   a_value Value to set
     * @param   a_ds    Data set of tags
     *
     * @tparam  GRP     Group of the tag
     * @tparam  EL      Element of the tag
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void setTagValue(const std::string & a_value,
                                                ::gdcm::DataSet &   a_ds)
    {
        // WARNING : Do not use DataElement (for space padding)
        ::gdcm::Attribute<GRP, EL> gAt;
        gAt.SetValue(a_value);
        a_ds.Insert(gAt.GetAsDataElement());

    }

    /**
     * @brief   Insert the value of a tag in a data set.
     *
     * @param   a_value Value to set
     * @param   a_ds    Data set of tags
     *
     * @tparam  T       Type of value
     * @tparam  GRP     Group of the tag
     * @tparam  EL      Element of the tag
     */
    template <typename T, uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void setTagValue(const T &           a_value,
                                                ::gdcm::DataSet &   a_ds)
    {
        ::gdcm::Attribute<GRP, EL> gAt;
        gAt.SetValue(a_value);
        a_ds.Insert(gAt.GetAsDataElement());
    }

    /**
     * @brief   Insert multiple values of a tag in a data set.
     *
     * @param   a_ptr   A Array.
     * @param   len     Size of the array.
     * @param   a_ds    Data set of tags.
     *
     * @tparam  T       Type of pointer.
     * @tparam  GRP     Group of the tag.
     * @tparam  EL      Element of the tag.
     */
    template <typename T, uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void setTagValues(const T *          a_ptr,
                                                 const unsigned int len,
                                                 ::gdcm::DataSet &  a_ds)
    {
        ::gdcm::Attribute<GRP, EL> gAt;
        if (a_ptr)
        {
            gAt.SetValues(a_ptr, len);
        }
        a_ds.Insert(gAt.GetAsDataElement());
    }

    /**
     * @brief   Set and insert a sequence of items with a tag in the data set specified.
     *
     * @param   a_sq    Sequence of items to insert
     * @param   a_tag   Tag of the sequence
     * @param   a_ds    Data set
     */
    GDCMIO_API static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems >    a_sq,
                                 const ::gdcm::Tag &                                a_tag,
                                 ::gdcm::DataSet &                                  a_ds);

    /**
     * @brief   Set and insert a sequence of items with a tag in the data set specified.
     *
     * @param   a_sq    Sequence of items to insert
     * @param   a_ds    Data set
     *
     * @tparam  GRP     Group of the tag
     * @tparam  EL      Element of the tag
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems >   a_sq,
                                          ::gdcm::DataSet &                                 a_ds)
    {
        // Create the sequence of items
        gdcm::DataElement de( ::gdcm::Attribute<GRP,EL>::GetTag() );
        de.SetVR( ::gdcm::VR::SQ );
        de.SetValue(*a_sq);
        de.SetVL(a_sq->GetLength());

        // Insert the sequence of items
        a_ds.Insert(de);
    }

    /**
     * @brief   Insert a sequence of items with a tag in the data set specified.
     *
     * If the tag already exists, items of the old and new sequences are added in a new one.
     *
     * @param   a_sq    Sequence of items to insert
     * @param   a_ds    Data set
     *
     * @tparam  GRP     Group of the tag
     * @tparam  EL      Element of the tag
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void insertSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems >    a_sq,
                                             ::gdcm::DataSet &                                  a_ds)
    {
        // Set or add the SQ
        if ( !a_ds.FindDataElement( ::gdcm::Attribute<GRP,EL>::GetTag() ) )
        {
            setSQ<GRP,EL>(a_sq, a_ds);
        }
        else
        {
            // Get old SQ
            const ::gdcm::DataElement & de = a_ds.GetDataElement( ::gdcm::Attribute<GRP,EL>::GetTag() );
            ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqOther = de.GetValueAsSQ();

            // Add items of the new SQ to the old SQ
            const unsigned int nbItem = a_sq->GetNumberOfItems();
            for (unsigned int i = 1; i <= nbItem; ++i)  // WARN : item start at 1
            {
                gSqOther->AddItem(a_sq->GetItem(i));
            }
        }
    }

    /**
     * @brief   Return the pixel type of a gdcm Image.
     *
     * @param   a_gImg  Image of gdcm
     *
     * @return  The dynamic type of pixels (e.g: char, unsigned int, ...)
     */
    GDCMIO_API static const fwTools::DynamicType getPixelType(const ::gdcm::Image & a_gImg);

    /**
     * @brief   Return the pixel type of a fwData Image.
     *
     * @param   a_img   Image of fwData
     *
     * @return  The pixel format (e.g: INT8, UINT16, ...)
     */
    GDCMIO_API static const gdcm::PixelFormat getPixelType(const ::fwData::Image & a_img);

    /**
     * @brief   Return the photometric interpretation of an acquisition.
     *
     * @note Currently, return MONOCHROME2 (Other not supported).
     *
     * @param   a_serie Acquisition which contains image
     *
     * @return  The photometric interpretation (e.g: MONOCHROME2, RGB, ...)
     */
    GDCMIO_API static const gdcm::PhotometricInterpretation getPhotometricInterpretation(const ::fwData::Acquisition & a_serie);

    /**
     * @brief   Convert a GDCM Image buffer to a fwData Image one.
     *
     * Transform color image into grayscale one.
     *
     * @param   a_gImg      GDCM Image
     * @param   a_dest      fwData Image
     * @param   a_inBuffer  Raw buffer to convert
     */
    GDCMIO_API static void convertGdcmToDataBuffer(::gdcm::Image &          a_gImg,
                                                   ::fwData::Image::sptr    a_dest,
                                                    const void *            a_inBuffer) throw(::fwTools::Failed);

    /**
     * @brief   Convert a surface representation mode (FW4SPL) into recommended presentation type (DICOM).
     *
     * @see     Recommended Presentation Type Tag (0066,000d).
     *
     * @param   a_representationMode    Surface representation mode.
     *
     * @return  Corresponding recommended presentation type.
     */
    GDCMIO_API static ::gdcm::Surface::VIEWType convertToPresentationType(::fwData::Material::REPRESENTATION_MODE a_representationMode);

    /**
     * @brief   Convert a surface recommended presentation type (DICOM) into representation mode (FW4SPL).
     *
     * @param   a_presentationType    Surface recommended presentation type.
     *
     * @return  Corresponding representation mode
     */
    GDCMIO_API static ::fwData::Material::REPRESENTATION_MODE convertToRepresentationMode(::gdcm::Surface::VIEWType a_presentationType);
};

/**
 * @brief   This class contains helpers to set data into DICOM form.
 *
 * @class   DicomTools
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomTools
{

public:

    /**
     * @brief   Convert a representation mode into a DICOM form
     *  (eg : SURFACE, POINTS, WIREFRAME).
     *
     * @see DICOM tag (0x0066,0x000d).
     *
     * @param   a_representationMode    Reconstruction's material representation mode (eg : SURFACE, POINT, ...).
     */
    GDCMIO_API static const char * convertToPresentationTypeString(::fwData::Material::REPRESENTATION_MODE a_representationMode);

    /**
     * @brief   Write a basic coded entry (see PS 3.3  Table 8.8-1).
     *
     * @param   a_codedAt   Coded attributes to write.
     * @param   a_gDs       Data set where the Code Sequence will be write.
     *
     * @tparam  GRP         Tag group of the code sequence.
     * @tparam  EL          Element group of the code sequence.
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const DicomCodedAttribute &  a_codedAt,
                                                       ::gdcm::DataSet &            a_gDs)
    {
        createCodeSequence<GRP,EL>(a_codedAt.CV, a_codedAt.CSD, a_codedAt.CM, a_codedAt.CSV, a_gDs);
    }

    /**
     * @brief   Write a basic coded entry (see PS 3.3  Table 8.8-1).
     * The coding scheme version is undefined.
     *
     * @param   a_CV    Code Value.
     * @param   a_CSD   Coding Scheme Designator.
     * @param   a_CM    Code Meaning.
     * @param   a_gDs   Data set where the Code Sequence will be write.
     *
     * @tparam  GRP     Tag group of the code sequence.
     * @tparam  EL      Element group of the code sequence.
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const std::string & a_CV,
                                                       const std::string & a_CSD,
                                                       const std::string & a_CM,
                                                       ::gdcm::DataSet &   a_gDs)
    {
        createCodeSequence<GRP,EL>(a_CV, a_CSD, a_CM, "", a_gDs);
    }

    /**
     * @brief   Write a basic coded entry (see PS 3.3  Table 8.8-1).
     *
     * @param   a_CV    Code Value.
     * @param   a_CSD   Coding Scheme Designator.
     * @param   a_CM    Code Meaning.
     * @param   a_CSV   Coding Scheme Version;
     * @param   a_gDs   Data set where the Code Sequence will be write.
     *
     * @tparam  GRP     Tag group of the code sequence.
     * @tparam  EL      Element group of the code sequence.
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static void createCodeSequence(const std::string & a_CV,
                                                       const std::string & a_CSD,
                                                       const std::string & a_CM,
                                                       const std::string & a_CSV,
                                                       ::gdcm::DataSet &   a_gDs)
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqCodeSequence = new ::gdcm::SequenceOfItems();
        gSqCodeSequence->SetLengthToUndefined();
        {
            // Create item (shall be one)
            ::gdcm::Item        gIt;
            gIt.SetVLToUndefined();
            ::gdcm::DataSet &   gDs = gIt.GetNestedDataSet();

            //see: PS.3.3 Table 8.8-1
            GdcmData::setTagValue<0x0008,0x0100>(a_CV,   gDs);    // Code Value
            GdcmData::setTagValue<0x0008,0x0102>(a_CSD,  gDs);    // Coding Scheme Designator
            if (!a_CSV.empty())
                GdcmData::setTagValue<0x0008,0x0103>(a_CSV,  gDs);// Coding Scheme Version
            GdcmData::setTagValue<0x0008,0x0104>(a_CM,   gDs);    // Code Meaning

            // Insert in a sequence
            gSqCodeSequence->AddItem(gIt);

            GdcmData::insertSQ<GRP,EL>(gSqCodeSequence, a_gDs);
        }
    }

    /**
     * @brief   Read content of a code sequence (eg : Concept Name Code Sequence, ...)
     *
     * @note    This method can return a DicomCodedAttribute with empty members.
     *
     * @param   a_gDs   Data set where the code sequence is supposed to be.
     *
     * @tparam  GRP     Tag group of the code sequence.
     * @tparam  EL      Element group of the code sequence.
     */
    template <uint16_t GRP, uint16_t EL>
    GDCMIO_TEMPLATE_API static DicomCodedAttribute readCodeSequence(const ::gdcm::DataSet & a_gDs)
    {
        DicomCodedAttribute codedAttributes;

        const ::gdcm::Tag codeSQTag(GRP,EL);
        if ( !a_gDs.FindDataElement(codeSQTag))
        { // Return empty coded attributes
          return codedAttributes;
        }
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > codeSQ = a_gDs.GetDataElement(codeSQTag).GetValueAsSQ();

        if ( codeSQ->GetNumberOfItems() == 0)  // One Item shall be permitted
        { // Return empty coded attributes
          return codedAttributes;
        }

        const ::gdcm::DataSet & codeDS = codeSQ->GetItem(1).GetNestedDataSet();
        codedAttributes.CV  = GdcmData::getTagValue<0x0008,0x0100>(codeDS);
        codedAttributes.CSD = GdcmData::getTagValue<0x0008,0x0102>(codeDS);
        codedAttributes.CSV = GdcmData::getTagValue<0x0008,0x0103>(codeDS);
        codedAttributes.CM  = GdcmData::getTagValue<0x0008,0x0104>(codeDS);

        return codedAttributes;
    }

};

/**
 * @brief   This class contains helpers for DICOM Structured Reporting (SR).
 *
 * @class   DicomSR
 * @author  IRCAD (Research and Development Team).
 * @date    2011.
 */
class GDCMIO_CLASS_API DicomSR
{
public:

    /**
     * @brief    Get the data set which contains a coded container.
     *
     * A null pointer means any container was found.
     *
     * @param a_codeValue Code value to find.
     * @param a_ds Data set of a DICOM file.
     *
     * @return Data set pointer.
     */
    GDCMIO_API static const ::gdcm::DataSet * getCodedContainer(const std::string &     a_codeValue,
                                                                const ::gdcm::DataSet & a_ds);

    /**
     * @brief    Get the data set which contains a typed container.
     *
     * A null pointer means any container was found.
     *
     * @param a_typeValue Type value to find.
     * @param a_ds Data set of a DICOM file.
     *
     * @return Data set pointer.
     */
    GDCMIO_API static const ::gdcm::DataSet * getTypedContainer(const std::string &     a_typeValue,
                                                                const ::gdcm::DataSet & a_ds);

    /**
     * @brief   Create one Spatial COORDinate (SCOORD) item in a sequence of items.
     *
     * @see     PS 3.3 C.17.3 and C.18.6
     *
     * @param   a_sccord        SCOORD to save in data set.
     * @param   a_refFrames     Referenced Frame numbers to insert.
     * @param   a_classUID      Referenced SOP Class UID.
     * @param   a_instanceUID   Referenced SOP Instance UID.
     * @param   a_sq            Sequence of items where new item will be inserted.
     */
    GDCMIO_API static void createSCOORD(const SCoord &                                  a_scoord,
                                        const std::string &                             a_refFrames,
                                        const std::string &                             a_classUID,
                                        const std::string &                             a_instanceUID,
                                        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > a_sq);
    /**
     * @brief    Create one Spatial COORDinate (SCOORD) item in a sequence of items.
     *
     * @note    This SCOORD refers to an multi-frame image.
     *
     * @param   a_refFrames     Frame indexes of related image (equals to Z coordinates).
     * @param   a_classUID      Referenced SOP class UID of the image.
     * @param   a_instanceUID   Referenced SOP instance UID of the image.
     * @param   a_gDs           Data set where SCOORD node will be written.
     */
    GDCMIO_API static void createSCOORD(const SCoord &      a_scoord,
                                        const std::string & a_refFrames,
                                        const std::string & a_classUID,
                                        const std::string & a_instanceUID,
                                        ::gdcm::DataSet &   a_gDs);

    /**
     * @brief    Create one Spatial COORDinate (SCOORD) item in a sequence of items.
     *
     * @note    This SCOORD refers to an non multi-frame image.
     *
     * @param   a_sccord        SCOORD to save in data set.
     * @param   a_classUID      Referenced SOP class UID of the frame.
     * @param   a_instanceUID   Referenced SOP instance UID of the frame.
     * @param   a_gDs           Data set where SCOORD node will be written.
     */
    GDCMIO_API static void createSCOORD(const SCoord &      a_scoord,
                                        const std::string & a_classUID,
                                        const std::string & a_instanceUID,
                                        ::gdcm::DataSet &   a_gDs);

    /**
     * @brief   Create one IMAGE item in a sequence of items.
     *
     * @note    \p a_refFrames can be empty. This case is valid
     *          when image is a non multiframe one or all the frames
     *          of the 3D image is referenced.
     *
     * @see     PS 3.3 C.17.3 and C.18.4
     *
     * @param   a_refFrames     Referenced frame number.
     * @param   a_classUID      Referenced SOP class UID of the frame.
     * @param   a_instanceUID   Referenced SOP instance UID of the frame.
     * @param   a_sq            Sequence of items where IMAGE node will be written.
     */
    GDCMIO_API static void createIMAGE(const std::string &                              a_refFrames,
                                       const std::string &                              a_classUID,
                                       const std::string &                              a_instanceUID,
                                       ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_sq);

    /**
     * @brief   Convert an referenced SOP instance UID into a frame number.
     *
     * @param   a_instanceUID   A referenced SOP instance UID.
     * @param   a_referencedInstanceUIDs    Ordered list of all image SOP Instance UIDs.
     *
     * @return  A frame number.
     */
    GDCMIO_API static unsigned int instanceUIDToFrameNumber(const std::string &                 a_instanceUID,
                                                            const std::vector< std::string > &  a_referencedInstanceUIDs) throw(::fwTools::Failed);
    /**
     * @brief   Read a SCOORD from its data set.
     *
     * @param   a_gDs   The data set where the SCOORD is defined.
     *
     * @return  A SCoord.
     */
    GDCMIO_API static SCoord readSCOORD(const ::gdcm::DataSet & a_gDs);

};

} // namespace helper

} // namespace gdcmIO

#endif /*_GDCMIO_GDCMHELPER_HPP_*/
