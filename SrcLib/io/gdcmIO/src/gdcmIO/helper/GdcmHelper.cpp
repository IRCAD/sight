/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <cmath>

#include <gdcmGlobal.h>
#include <gdcmDicts.h>
#include <gdcmPhotometricInterpretation.h>
#include <gdcmPixelFormat.h>
#include <gdcmImageApplyLookupTable.h>
#include <gdcmScanner.h>
#include <gdcmRescaler.h>

#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <boost/bimap/bimap.hpp>
#include <boost/assign/list_of.hpp>

#include <fwCore/base.hpp>

#include <fwTools/Dispatcher.hpp>
#include <fwTools/IntrinsicTypes.hpp>
#include <fwTools/DynamicTypeKeyTypeMapping.hpp>

#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------
//                                  GdcmData
//------------------------------------------------------------------------------

struct DynamicTypeOrderer
{
  bool operator()(const fwTools::DynamicType & d1, const fwTools::DynamicType &d2) const
  {
    return d1.string() < d2.string();
  }
};

//------------------------------------------------------------------------------

// BOOST 1.38 -> ::boost::bimaps::bimap<....>    note bimapS  ## BOOST 1.39 -> ::boost::bimap::bimap<....>
typedef  ::boost::bimaps::bimap<
                    ::boost::bimaps::set_of< fwTools::DynamicType , DynamicTypeOrderer  >
                  , ::boost::bimaps::set_of< gdcm::PixelFormat::ScalarType >
                   > TypeTranslator;

//------------------------------------------------------------------------------

TypeTranslator PixelTypeTranslation = boost::assign::list_of< TypeTranslator::relation >
( fwTools::makeDynamicType<unsigned char>(),    ::gdcm::PixelFormat::UINT8 )
( fwTools::makeDynamicType<signed char>(),      ::gdcm::PixelFormat::INT8 )
//( fwTools::makeDynamicType<unsigned short>(), ::gdcm::PixelFormat::UINT12 )   // Unsupported by VTK Render
//( fwTools::makeDynamicType<signed short>(),   ::gdcm::PixelFormat::INT12 )    // Unsupported by VTK Render
( fwTools::makeDynamicType<unsigned short>(),   ::gdcm::PixelFormat::UINT16 )
( fwTools::makeDynamicType<signed short>(),     ::gdcm::PixelFormat::INT16 )
( fwTools::makeDynamicType<unsigned int>(),     ::gdcm::PixelFormat::UINT32 )
( fwTools::makeDynamicType<signed int>(),       ::gdcm::PixelFormat::INT32 )
//( fwTools::makeDynamicType<float>(),          ::gdcm::PixelFormat::FLOAT16 )  // Unsupported by VTK Render
( fwTools::makeDynamicType<float>(),            ::gdcm::PixelFormat::FLOAT32 )
( fwTools::makeDynamicType<double>(),           ::gdcm::PixelFormat::FLOAT64 );

//------------------------------------------------------------------------------

void GdcmData::setSQ(::gdcm::SmartPointer< ::gdcm::SequenceOfItems >    a_sq,
                     const ::gdcm::Tag &                                a_tag,
                     ::gdcm::DataSet &                                  a_ds)
{
    if(a_sq->GetNumberOfItems() > 0)
    {
        // Create the sequence of items
        gdcm::DataElement de(a_tag);
        de.SetVR( ::gdcm::VR::SQ );
        de.SetValue(*a_sq);
        de.SetVL(a_sq->GetLength());

        // Insert or replace the sequence of items
        a_ds.Insert(de);
    }
}

//------------------------------------------------------------------------------

const fwTools::DynamicType GdcmData::getPixelType(const ::gdcm::PixelFormat & gPixFormat)
{
    assert ( PixelTypeTranslation.right.find( gPixFormat.GetScalarType() )!= PixelTypeTranslation.right.end() );
    return PixelTypeTranslation.right.at( gPixFormat.GetScalarType() );
}

//------------------------------------------------------------------------------

const fwTools::DynamicType GdcmData::getPixelType(const ::gdcm::Image & a_gImg)
{
    const ::gdcm::PixelFormat & gPixFormat = a_gImg.GetPixelFormat();

    assert ( PixelTypeTranslation.right.find( gPixFormat.GetScalarType() )!= PixelTypeTranslation.right.end() );

    return PixelTypeTranslation.right.at( gPixFormat.GetScalarType() );
}

//------------------------------------------------------------------------------

const gdcm::PixelFormat GdcmData::getPixelType(const ::fwData::Image & a_img)
{
    const ::gdcm::PixelFormat::ScalarType st = PixelTypeTranslation.left.at( a_img.getPixelType() );

    assert ( PixelTypeTranslation.right.find( st )!= PixelTypeTranslation.right.end() );

    return gdcm::PixelFormat(st);
}


//------------------------------------------------------------------------------
gdcm::PixelFormat::ScalarType GdcmData::findPixelTypeFromFiles(const std::vector< std::string > & filenames)
{
    const ::gdcm::Tag rescaleInterceptTag(0x0028,0x1052);
    const ::gdcm::Tag rescaleSlopeTag(0x0028,0x1053);
    const ::gdcm::Tag samplesPerPixelTag(0x0028,0x0002);
    const ::gdcm::Tag bitsAllocatedTag(0x0028,0x0100);
    const ::gdcm::Tag bitsStoredTag(0x0028,0x0101);
    const ::gdcm::Tag hightBitTag(0x0028,0x0102);
    const ::gdcm::Tag pixelRepresentationTag(0x0028,0x0103);

    double intercept;
    double slope;

    unsigned short samplesPerPixel;
    unsigned short bitsAllocated;
    unsigned short bitsStored;
    unsigned short highBit;
    unsigned short pixelRepresentation;
    gdcm::PixelFormat::ScalarType outputPixelType = gdcm::PixelFormat::UNKNOWN;

    std::set< gdcm::PixelFormat::ScalarType > pixelTypes;
    gdcm::PixelFormat::ScalarType pixelType;
    gdcm::Scanner gScanner;
    gScanner.AddTag(rescaleSlopeTag);
    gScanner.AddTag(rescaleInterceptTag);

    gScanner.AddTag(samplesPerPixelTag);
    gScanner.AddTag(bitsAllocatedTag);
    gScanner.AddTag(bitsStoredTag);
    gScanner.AddTag(hightBitTag);
    gScanner.AddTag(pixelRepresentationTag);

    bool scanOk = gScanner.Scan( filenames );
    if(!scanOk)
    {
        SLM_ERROR("Scanner failed");
        return outputPixelType;
    }
    gdcm::Directory::FilenamesType keys = gScanner.GetKeys();
    gdcm::Directory::FilenamesType::const_iterator it = keys.begin();
    for(; it != keys.end(); ++it)
    {
        const char *filename = it->c_str();
        assert( gScanner.IsKey( filename ) );

        valueOf(std::string(gScanner.GetValue( filename, rescaleInterceptTag)), intercept);
        valueOf(std::string(gScanner.GetValue( filename, rescaleSlopeTag)), slope);
        valueOf(std::string(gScanner.GetValue( filename, samplesPerPixelTag)),samplesPerPixel );
        valueOf(std::string(gScanner.GetValue( filename, bitsAllocatedTag)), bitsAllocated);
        valueOf(std::string(gScanner.GetValue( filename, bitsStoredTag)), bitsStored);
        valueOf(std::string(gScanner.GetValue( filename, hightBitTag)), highBit);
        valueOf(std::string(gScanner.GetValue( filename, pixelRepresentationTag)),pixelRepresentation );

        gdcm::PixelFormat pixelFormat(samplesPerPixel, bitsAllocated, bitsStored, highBit, pixelRepresentation);
        pixelType = pixelFormat.GetScalarType();

        gdcm::Rescaler r;
        r.SetIntercept( intercept );
        r.SetSlope( slope );
        r.SetPixelFormat( pixelType);
        outputPixelType = r.ComputeInterceptSlopePixelType();
        pixelTypes.insert(outputPixelType);
    }

    if( pixelTypes.size() == 1 )
    {
        outputPixelType = *pixelTypes.begin();
    }
    else
    {
        SLM_FATAL_IF("Sorry, The pixel type can't be deduced from series (various pixel type exist).", pixelTypes.count( gdcm::PixelFormat::FLOAT64 ) != 0 );
        outputPixelType = gdcm::PixelFormat::FLOAT64;
    }
    return (outputPixelType);
}

//------------------------------------------------------------------------------

const gdcm::PhotometricInterpretation GdcmData::getPhotometricInterpretation(const ::fwData::Acquisition & a_serie)
{
    // TODO: uncomment if FW4SPL handle color image.
//    unsigned int components = a_serie.getNumberOfScalarPerPixel();
//
//    // Attempt a guess (VTK do the same choice)
//    switch (components)
//    {
//    case 1:// It could well be MONOCHROME1
//        return ::gdcm::PhotometricInterpretation::MONOCHROME2;
//    case 3:// It could well be YBR
//        return ::gdcm::PhotometricInterpretation::RGB;
//    case 4:// It could well be CMYK
//        return ::gdcm::PhotometricInterpretation::ARGB;
//    default:
//        OSLM_ERROR("Photometric interpretation not found");
//        return ::gdcm::PhotometricInterpretation::UNKNOW;
//    }

    // FW4SPL just handle grey level image and lose color image data.
    return ::gdcm::PhotometricInterpretation::MONOCHROME2;
}

/**
 * @brief   Convert color image to grayscale image.
 *
 * This struct have to be use with ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , ColorToGrayFunctor >::invoke(pixelType, param).
 */
struct ColorToGrayFunctor
{
    /**
     * @brief   It defines required parameter for ColorToGrayFunctor
     */
    struct Parameter
    {
        const void *    a_src;                      ///< Source Image buffer
        void *          a_dst;                      ///< Destination Image buffer
        unsigned int    len_dst;                    ///< Length of the grayscale image buffer

        unsigned int    gPC;                        ///< Planar Configuration
                                                    ///< (e.g for RGB image : 0 => R1,G1,B1,R2,... ; 1 => R1,R2,...,G1,...)
        ::gdcm::PhotometricInterpretation gPIType;  ///< Photometric Interpretation type (e.g : RGB, YBR, ...)
    };

    /**
     * @brief   Convert RGB image to grayscale image
     */
    template<class PIXELTYPE>
    void RGBtoGray(PIXELTYPE * a_src, PIXELTYPE * a_dst, const PIXELTYPE * a_dstEnd, const unsigned int * a_jump)
    {
        float valR, valG, valB;
        while (a_dst < a_dstEnd)
        {
            valR = *(a_src + a_jump[0]) * 0.30;
            valG = *(a_src + a_jump[1]) * 0.59;
            valB = *(a_src + a_jump[2]) * 0.11;

            (*a_dst) = (PIXELTYPE)(valR + valG + valB);

            a_src += a_jump[3];
            a_dst++;
        }
    }

    /**
     * @brief   Convert HSV image to grayscale image
     */
    template<class PIXELTYPE>
    void HSVtoGray(PIXELTYPE * a_src, PIXELTYPE * a_dst, const PIXELTYPE * a_dstEnd, const unsigned int * a_jump)
    {
        while (a_dst < a_dstEnd)
        {
            // Get the Value component or Lightness one for HSL
            (*a_dst) = (PIXELTYPE)( *(a_src + a_jump[2]) );

            a_src += a_jump[3];
            a_dst++;
        }
    }

    /**
     * @brief   Convert color image to grayscale image
     */
    template<class PIXELTYPE>
    void operator()(Parameter & p) throw(::fwTools::Failed)
    {
        // Cast buffers
        PIXELTYPE       * srcTyped      = (PIXELTYPE *) p.a_src;
        PIXELTYPE       * dstTyped      = (PIXELTYPE *) p.a_dst;
        const PIXELTYPE * dstTypedEnd   = dstTyped + p.len_dst;

        // Defined a jump between each component of a pixel
        unsigned int jump[4] = {0, 1, 2, 3};    // The last jump is for the buffer pointer
        if (p.gPC == 1)
        {
            jump[1] = p.len_dst;    jump[2] = 2 * p.len_dst;    jump[3] = 1;
        }

        // Convert to grayscale
        if ( p.gPIType == ::gdcm::PhotometricInterpretation::RGB )
            RGBtoGray<PIXELTYPE>(srcTyped, dstTyped, dstTypedEnd, jump);
        else if ( p.gPIType == ::gdcm::PhotometricInterpretation::HSV )
            HSVtoGray<PIXELTYPE>(srcTyped, dstTyped, dstTypedEnd, jump);
        else
            throw ::fwTools::Failed("Photometric interpretation not handle");
    }
};

//------------------------------------------------------------------------------

void GdcmData::convertGdcmToDataBuffer(::gdcm::Image & a_gImg,
                                       ::fwData::Image::sptr a_dest,
                                        const void * a_inBuffer) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    assert(a_inBuffer);

    // Get color space
    ::gdcm::PhotometricInterpretation gPI = a_gImg.GetPhotometricInterpretation();

    if (gPI == ::gdcm::PhotometricInterpretation::MONOCHROME1)
        throw ::fwTools::Failed("Monochrome 1 not handled");

    // First, convert LUT to RGB image
    if ( gPI == ::gdcm::PhotometricInterpretation::PALETTE_COLOR )
    {
        ::gdcm::ImageApplyLookupTable applyLUT;
        applyLUT.SetInput( a_gImg );
        applyLUT.Apply();
        a_gImg = applyLUT.GetOutput();
    }

    // Initialize
    const unsigned int      components          = a_gImg.GetPixelFormat().GetSamplesPerPixel();
    const unsigned long     size                = a_gImg.GetBufferLength() / components;

    ::fwTools::DynamicType  pixelType           = GdcmData::getPixelType(a_gImg);
    const unsigned int      bytesPerComponent   = pixelType.sizeOf();
    if (bytesPerComponent > 4)
        throw ::fwTools::Failed("Pixel type not handled : " + pixelType.string());

    // Initialize fwData::Image Buffer
    void * destBuffer;
    try
    {
        destBuffer = new char[size];
    }
    catch(...)
    {
        throw ::fwTools::Failed("Need more memory");
    }

    // Translate GDCM into fwData::Image Buffer
    switch (components)
    {
    case 1:
        // For grayscale image
        memcpy(destBuffer, a_inBuffer, size);
        break;
    case 3:
    {
        // Convert color into grayscale image
        ColorToGrayFunctor::Parameter saverParam;
        saverParam.a_src    = a_inBuffer;
        saverParam.a_dst    = destBuffer;
        saverParam.len_dst  = size;
        saverParam.gPC      = a_gImg.GetPlanarConfiguration();
        saverParam.gPIType  = a_gImg.GetPhotometricInterpretation();

        try
        {
            ::fwTools::Dispatcher< ::fwTools::IntrinsicTypes , ColorToGrayFunctor >::invoke(pixelType, saverParam);
        }
        catch (::fwTools::Failed & e)
        {
            throw;
        }
        break;
    }
    default:
        throw ::fwTools::Failed("Color space not handled");
    }

    delete (char*) a_inBuffer;

    // Set fwData::Image Buffer
    a_dest->setBuffer(destBuffer);
}

//------------------------------------------------------------------------------

::gdcm::Surface::VIEWType GdcmData::convertToPresentationType(::fwData::Material::REPRESENTATION_MODE a_representationMode)
{
    switch (a_representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
        return ::gdcm::Surface::SURFACE;
        break;
    case ::fwData::Material::MODE_POINT:
        return ::gdcm::Surface::POINTS;
        break;
    case ::fwData::Material::MODE_WIREFRAME:
        return ::gdcm::Surface::WIREFRAME;
        break;
    default:
        OSLM_WARN( "Representation type not handle (changed to : SURFACE)" );
        return ::gdcm::Surface::SURFACE;
    }
}

//------------------------------------------------------------------------------

::fwData::Material::REPRESENTATION_MODE GdcmData::convertToRepresentationMode(::gdcm::Surface::VIEWType a_presentationType)
{
    switch (a_presentationType)
    {
    case ::gdcm::Surface::SURFACE:
        return ::fwData::Material::MODE_SURFACE;
        break;
    case ::gdcm::Surface::WIREFRAME:
        return ::fwData::Material::MODE_WIREFRAME;
        break;
    case ::gdcm::Surface::POINTS:
        return ::fwData::Material::MODE_POINT;
        break;
    default:
        OSLM_WARN( "Presentation type not handle (changed to : SURFACE)" );
        return ::fwData::Material::MODE_SURFACE;
    }
}

//------------------------------------------------------------------------------
//                                  DicomTools
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------

const char * DicomTools::convertToPresentationTypeString(::fwData::Material::REPRESENTATION_MODE a_representationMode)
{
    switch (a_representationMode)
    {
    case ::fwData::Material::MODE_SURFACE:
        return "SURFACE";
        break;
    case ::fwData::Material::MODE_POINT:
        return "POINTS";
        break;
    case ::fwData::Material::MODE_WIREFRAME:
        return "WIREFRAME";
        break;
    default:
        OSLM_WARN( "Presentation type not handle (changed to : SURFACE)" );
        return "SURFACE";
    }
}

//------------------------------------------------------------------------------
//                                  DicomSR
//------------------------------------------------------------------------------

const ::gdcm::DataSet * DicomSR::getCodedContainer(const std::string &      a_codeValue,
                                                   const ::gdcm::DataSet &  a_ds)
{
    SLM_TRACE_FUNC();

    if (a_ds.FindDataElement( ::gdcm::Tag(0x0040,0xa043) ))    // Concept Name Code Sequence
    {// Search coded container at this level
        const ::gdcm::DataElement &                             gDeContent = a_ds.GetDataElement( ::gdcm::Tag(0x0040,0xa043) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >   gSqContent = gDeContent.GetValueAsSQ();
        if (gSqContent->GetNumberOfItems() > 0)
        {
            const ::gdcm::Item &    gItCode = gSqContent->GetItem(1);
            const ::gdcm::DataSet & gDsCode = gItCode.GetNestedDataSet();
            if (GdcmData::getTagValue<0x0008,0x0100>(gDsCode) == a_codeValue)// Code Value
            {
                return &a_ds;
            }
        }
    }
    else if (a_ds.FindDataElement( ::gdcm::Tag(0x0040,0xa730) ))    // Content Sequence
    {// Search in children
        const ::gdcm::DataElement &                             gDeContent = a_ds.GetDataElement( ::gdcm::Tag(0x0040,0xa730) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >   gSqContent = gDeContent.GetValueAsSQ();

        // Traverse each item to find coded container
        ::gdcm::SequenceOfItems::ConstIterator it       = gSqContent->Begin();
        ::gdcm::SequenceOfItems::ConstIterator itEnd    = gSqContent->End();
        for (; it != itEnd; ++it)                                   // For each item
        {
            const ::gdcm::DataSet * ans = DicomSR::getCodedContainer(a_codeValue, it->GetNestedDataSet());
            if (ans != 0)
                return ans;
        }
    }

    return 0;
}

//------------------------------------------------------------------------------

const ::gdcm::DataSet * DicomSR::getTypedContainer(const std::string &      a_typeValue,
                                                   const ::gdcm::DataSet &  a_ds)
{
    SLM_TRACE_FUNC();

    if (GdcmData::getTagValue<0x0040,0xa040>(a_ds) == a_typeValue)  // Type Value
    {// Search typed container at this level
        return &a_ds;
    }
    else if (a_ds.FindDataElement( ::gdcm::Tag(0x0040,0xa730) ))    // Content Sequence
    {// Search in children
        const ::gdcm::DataElement & gDeContent = a_ds.GetDataElement( ::gdcm::Tag(0x0040,0xa730) );
        const ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqContent = gDeContent.GetValueAsSQ();

        // Traverse each item to find typed container
        ::gdcm::SequenceOfItems::ConstIterator it       = gSqContent->Begin();
        ::gdcm::SequenceOfItems::ConstIterator itEnd    = gSqContent->End();
        for (; it != itEnd; ++it)
        {
            const ::gdcm::DataSet * ans = DicomSR::getTypedContainer(a_typeValue, it->GetNestedDataSet());
            if (ans != 0)
                return ans;
        }
    }
    // else, the data set is not a SR one

    return 0;
}

//------------------------------------------------------------------------------

void DicomSR::createSCOORD(const SCoord &                                   a_scoord,
                           const std::string &                              a_refFrame,
                           const std::string &                              a_classUID,
                           const std::string &                              a_instanceUID,
                           ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >  a_sq)
{
    //*** Create a SCOORD ***//
    // see: PS 3.3 Table 10-2 Content Item Macro Attributes Description

    ::gdcm::Item gItSCOORD;
    gItSCOORD.SetVLToUndefined();
    ::gdcm::DataSet & gDsSCOORD = gItSCOORD.GetNestedDataSet();

    // Relationship Type
    GdcmData::setTagValue<0x0040,0xa010>(DicomDictionarySR::getRelationshipString(DicomDictionarySR::INFERRED_FROM),gDsSCOORD);

    // Type Value
    GdcmData::setTagValue<0x0040,0xa040>(DicomDictionarySR::getTypeString(DicomDictionarySR::SCOORD),               gDsSCOORD);

    // Concept Name Code Sequence
    // Inferred from PS 3.3 C.17.3.2.1 and 3.16 Context ID 7003
    DicomTools::createCodeSequence<0x0040,0xa043>(a_scoord.getCRefContext(), gDsSCOORD);        //Type 1C

    // Write coordinates
    const unsigned int dataSize = a_scoord.getGraphicDataSize();
    unsigned int nbCoords;
    if (a_scoord.getGraphicType() == DicomDictionarySR::getGraphicTypeString(DicomDictionarySR::POINT))
        nbCoords = 2;
    else
        nbCoords = dataSize;

    assert( nbCoords%2 == 0 );

    float * graphicData = new float[nbCoords];
    for (unsigned int i = 0; i < nbCoords; ++i)
    {
        graphicData[i] = a_scoord[i];
    }
    // Graphic Data
    GdcmData::setTagValues<float,0x0070,0x0022>(graphicData, nbCoords,  gDsSCOORD); // Type 1
    delete graphicData;

    // Graphic Type
    GdcmData::setTagValue<0x0070,0x0023>(a_scoord.getGraphicType(),     gDsSCOORD); // Type 1

    // Create a SQ which contains referenced image
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqImage = new ::gdcm::SequenceOfItems();
    gSqImage->SetLengthToUndefined();
    {
        createIMAGE(a_refFrame, a_classUID, a_instanceUID, gSqImage);

        GdcmData::setSQ<0x0040,0xa730>(gSqImage, gDsSCOORD);
    }

    a_sq->AddItem(gItSCOORD);
}

//------------------------------------------------------------------------------

void DicomSR::createSCOORD(const SCoord &      a_scoord,
                           const std::string & a_refFrame,
                           const std::string & a_classUID,
                           const std::string & a_instanceUID,
                           ::gdcm::DataSet &   a_gDs)
{
    // Get or create a content sequence in order to insert a SCOORD
    const ::gdcm::Tag contentSQTag(0x0040,0xa730);  // Content Sequence
    if ( !a_gDs.FindDataElement(contentSQTag) )
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSQ = new ::gdcm::SequenceOfItems();
        GdcmData::setSQ<0x0040,0xa730>(gSQ, a_gDs);
    }
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gContentSQ = a_gDs.GetDataElement(contentSQTag).GetValueAsSQ();

    // Write a SCOORD
    DicomSR::createSCOORD(a_scoord, a_refFrame, a_classUID, a_instanceUID, gContentSQ);
}

//------------------------------------------------------------------------------

void DicomSR::createSCOORD(const SCoord &       a_scoord,
                           const std::string &  a_classUID,
                           const std::string &  a_instanceUID,
                           ::gdcm::DataSet &    a_gDs)
{
    // Get or create a content sequence in order to insert a SCOORD
    const ::gdcm::Tag contentSQTag(0x0040,0xa730);  // Content Sequence
    if ( !a_gDs.FindDataElement(contentSQTag) )
    {
        ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSQ = new ::gdcm::SequenceOfItems();
        GdcmData::setSQ<0x0040,0xa730>(gSQ, a_gDs);
    }
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gContentSQ = a_gDs.GetDataElement(contentSQTag).GetValueAsSQ();

    // Write a SCOORD
    DicomSR::createSCOORD(a_scoord, "", a_classUID, a_instanceUID, gContentSQ);
}

//------------------------------------------------------------------------------

void DicomSR::createIMAGE(const std::string &                               a_refFrame,
                          const std::string &                               a_classUID,
                          const std::string &                               a_instanceUID,
                          ::gdcm::SmartPointer< ::gdcm::SequenceOfItems >   a_sq)
{
    ::gdcm::Item        gItImage;
    gItImage.SetVLToUndefined();
    ::gdcm::DataSet &   gDsImage = gItImage.GetNestedDataSet();

    // Relationship Type
    GdcmData::setTagValue<0x0040,0xa010>(DicomDictionarySR::getRelationshipString(DicomDictionarySR::SELECTED_FROM),gDsImage);

    // Type Value
    GdcmData::setTagValue<0x0040,0xa040>(DicomDictionarySR::getTypeString(DicomDictionarySR::IMAGE),                gDsImage);

    //** Create a SQ which contains referenced frames of an image **//
    ::gdcm::SmartPointer< ::gdcm::SequenceOfItems > gSqRefImage = new ::gdcm::SequenceOfItems();
    gSqRefImage->SetLengthToUndefined();
    {
        //see: PS 3.3 Table 10-3 IMAGE SOP INSTANCE REFERENCE MACRO ATTRIBUTES
        ::gdcm::Item        gItRefImage;
        gItRefImage.SetVLToUndefined();
        ::gdcm::DataSet &   gDsRefImage = gItRefImage.GetNestedDataSet();

        // Referenced SOP Class UID
        GdcmData::setTagValue<0x0008,0x1150>(a_classUID,                gDsRefImage);   // Type 1

        // Referenced SOP Instance UID
        GdcmData::setTagValue<0x0008,0x1155>(a_instanceUID,             gDsRefImage);   // Type 1

        if ( !a_refFrame.empty() )
        {
            const int referencedFrame = atoi(a_refFrame.c_str());

            // Referenced Frame Number
            GdcmData::setTagValues<int,0x0008,0x1160>(&referencedFrame, 1,  gDsRefImage);// Type 1C
        }

        gSqRefImage->AddItem(gItRefImage);

        GdcmData::setSQ<0x0008,0x1199>(gSqRefImage,         gDsImage);  // Referenced SOP Sequence
    } // End container of referenced frames

    a_sq->AddItem(gItImage);
}

//------------------------------------------------------------------------------

unsigned int DicomSR::instanceUIDToFrameNumber(const std::string &                  a_instanceUID,
                                               const std::vector< std::string > &   a_referencedInstanceUIDs) throw(::fwTools::Failed)
{
    unsigned int                        frameNumber                 = 0;
    const unsigned int                  numberOfFrames              = a_referencedInstanceUIDs.size();
    bool                                stop                        = false;
    // Traverse referenced SOP instance UIDs to find the good one
    while (frameNumber < numberOfFrames && !stop)
    {
        if (a_referencedInstanceUIDs[ frameNumber ] == a_instanceUID)
        {
            stop = true;
        }
        else
        {
            ++frameNumber;
        }
    }

    if (frameNumber < numberOfFrames)
    {
        return frameNumber +1; // +1 because frame number start at 1
    }
    else
    {
        throw ::fwTools::Failed("Referenced image not found");
    }

    return 0;
}

//------------------------------------------------------------------------------

SCoord DicomSR::readSCOORD(const ::gdcm::DataSet & a_gDs)
{
    SCoord scoord;

    // Graphic Type
    scoord.setGraphicType( GdcmData::getTagValue<0x0070,0x0023>(a_gDs) );

    // Graphic Data
    ::gdcm::Attribute<0x0070,0x0022>    gAtGD;
    gAtGD.SetFromDataElement( a_gDs.GetDataElement( ::gdcm::Attribute<0x0070,0x0022>::GetTag() ) );
    const unsigned int                  nbCoords = gAtGD.GetNumberOfValues();
    scoord.setGraphicData( gAtGD.GetValues(), nbCoords);

    // Concept Name Code Sequence (Context)
    scoord.setContext( DicomTools::readCodeSequence<0x0040,0xa043>(a_gDs) );

    return scoord;
}

} //namespace helper

} //namespace gdcmIO


