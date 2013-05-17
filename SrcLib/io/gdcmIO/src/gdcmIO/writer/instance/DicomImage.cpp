/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <gdcmImageWriter.h>
#include <gdcmUIDGenerator.h>

#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <fwTools/dateAndTime.hpp>

#include <fwComEd/helper/Image.hpp>

#include "gdcmIO/writer/instance/DicomImage.hpp"
#include "gdcmIO/writer/module/DicomEquipment.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace writer
{
namespace instance
{

//------------------------------------------------------------------------------

DicomImage::DicomImage():
        m_isMultiFrame( false ),
        m_imageID("1")
{
    // Instantiate a specific GDCM writer
    this->setWriter( ::boost::shared_ptr< ::gdcm::ImageWriter >( new ::gdcm::ImageWriter ) );
}

//------------------------------------------------------------------------------

DicomImage::~DicomImage()
{}

//------------------------------------------------------------------------------

void DicomImage::write() throw(::gdcmIO::exception::Failed)
{
    ::fwData::Image::csptr image  = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    // Get typed writer
    ::gdcm::DataSet& gDsRoot = this->getDataSet();

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();

    this->writeImagePixel();

    unsigned int dim = (unsigned int) image->getNumberOfDimensions();
    if (!dicomInstance->getIsMultiFrame())  // if non multi-frame, each file will have 2D image
    {
        dim = 2;
    }

    // Define SOP Class UID from the modality
    const std::string &     modality    = dicomInstance->getCRefModality();
    ::gdcm::MediaStorage    gMs;
    gMs.GuessFromModality(modality.c_str(), dim);
    std::string             SOPClassUID;    // Identify the content of file
    if ( gMs != ::gdcm::MediaStorage::MS_END
      && gMs.GetString() != 0 )
    {// Get the SOP Class UID from a guess
        SOPClassUID = gMs.GetString();
    }
    else
    {// Force modality and corresponding SOP Class UID to be "CT"
        dicomInstance->setModality("CT");   // Replace modality to comply with the SOP Class UID
        if (dim == 2)
        {
            SOPClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::CTImageStorage);
        }
        else
        {
            SOPClassUID = ::gdcm::MediaStorage::GetMSString(::gdcm::MediaStorage::EnhancedCTImageStorage);
        }
    }
    dicomInstance->setSOPClassUID(SOPClassUID);

    // Frame of reference UID
    ::gdcm::UIDGenerator    gUID;
    gUID.SetRoot( SOPClassUID.c_str() );
    const std::string framOfRefUID = gUID.Generate(); // UID of the first frame of the 2D/3D image
    dicomInstance->setFrameOfRefUID( framOfRefUID );
    helper::GdcmData::setTagValue<0x0020,0x0052>(framOfRefUID, gDsRoot);
    OSLM_TRACE("Frame of reference UID : " << framOfRefUID);

    // SOP Instance UID
    dicomInstance->setSOPInstanceUID( framOfRefUID );       // The first instance is the frame of reference, so they have the same UID
    dicomInstance->addReferencedSOPInstanceUID( framOfRefUID );

    if (dim > 2)
    {
        // Spacing between slice
        helper::GdcmData::setTagValue<double,0x0018,0x0088>(image->getSpacing()[2], gDsRoot);
        OSLM_TRACE("Spacing between slice : "<<image->getSpacing()[2] );
    }

    // Image's windows center
    const double winCenter = image->getWindowCenter();
    helper::GdcmData::setTagValues<double,0x0028,0x1050>(&winCenter, 1, gDsRoot);
    OSLM_TRACE("Image's window center : "<<image->getWindowCenter());

    // Image's windows width
    const double winWidth = image->getWindowWidth();
    helper::GdcmData::setTagValues<double,0x0028,0x1051>(&winWidth, 1, gDsRoot);
    OSLM_TRACE("Image's window width : "<<image->getWindowWidth());

    // TODO Acquisition Number
    const unsigned int acqNumber = 1;
    helper::GdcmData::setTagValue<unsigned int,0x0020,0x0012>(acqNumber, gDsRoot);

    // TODO Image Type (Type 1)
    const ::gdcm::String<92, 16> imageType = "ORIGINAL\\PRIMARY\\AXIAL";
    helper::GdcmData::setTagValues< ::gdcm::String<92, 16>, 0x0008,0x0008>(&imageType, 1, gDsRoot);

    // TODO Rescale Type (Type 1C)
    const ::gdcm::String<> rescaleType = "HU";
    helper::GdcmData::setTagValue<0x0028,0x1054>(rescaleType, gDsRoot);

    // TODO KVP : (Type 2)
    helper::GdcmData::setTagValue<double, 0x0018,0x0060>(0. , gDsRoot);

    // Patient Position : (Type 2C)
    ::gdcm::Attribute<0x0018,0x5100> gAtPatientPos;
    gDsRoot.Insert(gAtPatientPos.GetAsDataElement());

    // Position Reference Indicator (Type 2)
    ::gdcm::Attribute<0x0020,0x1040> gAtPosRefInd;
    gDsRoot.Insert(gAtPosRefInd.GetAsDataElement());

    // Laterality (Type 2C)
    ::gdcm::Attribute<0x0020,0x0060> gAtLaterality;
    gDsRoot.Insert(gAtLaterality.GetAsDataElement());


    //*****     Complete file and write it      *****//
    if (this->isMultiFrame())
    {// Write multi frame image in one file
        this->writeMultiFrame();

        this->write3DImage();
    }
    else
    {// Write image in several files if necessary
        this->writeImagePlane();

        this->write2DImage();
    }
}

//------------------------------------------------------------------------------

void DicomImage::write2DImage()
{
    // Get typed writer
    ::gdcm::ImageWriter *gImageWriter = ::boost::static_pointer_cast< ::gdcm::ImageWriter >( this->getWriter() ).get();
    ::gdcm::DataSet &gDsRoot = this->getDataSet();

    // Get images
    ::gdcm::Image & gImg = gImageWriter->GetImage();
    ::fwData::Image::sptr image = this->getConcreteObject();

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();

    // Remove completed tag by previous module writer (eg : General Image Module)
    // to be redefined after.
    if (gDsRoot.FindDataElement( ::gdcm::Tag(0x0020,0x0032) ))
    {
        gDsRoot.Remove(::gdcm::Tag(0x0020,0x0032));
    }
    if (gDsRoot.FindDataElement( ::gdcm::Tag(0x0020,0x1041) ))
    {
        gDsRoot.Remove(::gdcm::Tag(0x0020,0x1041));
    }

    //*****     Complete the file      *****//
    // Image's pixel data
    const ::fwData::Image::SizeType & size            = image->getSize();
    const unsigned int                  bufferLength    = size[0]*size[1] * gImg.GetPixelFormat().GetPixelSize();
    OSLM_TRACE("Frame's buffer size : "<<bufferLength);

    ::fwComEd::helper::Image imgHelper(image);
    const char *                        imageBuffer     = static_cast<char*>( imgHelper.getBuffer() );
    const unsigned int                  nbFrame         = size[2];
    const std::string &                 imagesPath      = this->getFile().string();
    const std::string                   fileName        = imagesPath + "/image_";

    // Create a copy to avoid writing conflict with GDCM
    const ::gdcm::DataSet       gDsRootCopy = gImageWriter->GetFile().GetDataSet();

    // Write the first frame
    {
        SLM_ASSERT("Frame without UID", !dicomInstance->getCRefSOPInstanceUID().empty());

        dicomInstance->setInstanceNumber(::fwTools::getString(1));   // Type 2

        // Slice origin
        const std::vector< double > & origin = image->getOrigin();
        gImg.SetOrigin(2, origin[2]);

        // Slice Location
        ::gdcmIO::helper::GdcmData::setTagValue<double,0x0020,0x1041>(origin[2], gDsRoot);    // Type 3
        OSLM_TRACE("Slice location : " << origin[2]);

        // Pixel Data
        ::gdcm::DataElement pixeldata( ::gdcm::Tag(0x7fe0,0x0010) );
        pixeldata.SetByteValue( imageBuffer, bufferLength );
        gImg.SetDataElement( pixeldata );

        // Set file names without extension
        this->setFile( fileName + "0" );

        //*****     Write one file      *****//
        DicomInstance< ::fwData::Image >::write();
    }

    // Write other frame
    for (unsigned int i = 1; i < nbFrame; ++i)  // Start to one to handle first instance
    {
        gImageWriter->GetFile().SetDataSet( gDsRootCopy );

        this->getDicomInstance()->setInstanceNumber(::fwTools::getString(i+1));   // Type 2
        OSLM_TRACE("Instance number : " << i+1);

        // Compute the slice location on Z
        const std::vector< double > &   origin  = image->getOrigin();
        const double                    posOnZ  = i * image->getSpacing()[2] + origin[2];
        gImg.SetOrigin(2, posOnZ);  // Set IPP z coordinate

        // Slice Location
        ::gdcmIO::helper::GdcmData::setTagValue<double,0x0020,0x1041>(posOnZ, gDsRoot);    // Type 3
        OSLM_TRACE("Slice location : " << posOnZ);

        // Pixel Data
        ::gdcm::DataElement pixeldata( ::gdcm::Tag(0x7fe0,0x0010) );
        pixeldata.SetByteValue( imageBuffer + i * bufferLength, bufferLength ); // Split the fwData::Image buffer
        gImg.SetDataElement( pixeldata );

        //*****     Complete the file      *****//
        // Define a SOP Instance UID for each file
        ::gdcm::UIDGenerator    gUIDgen;
        gUIDgen.SetRoot( dicomInstance->getSOPClassUID().c_str() );
        const std::string SOPInstanceUID  = gUIDgen.Generate();
        dicomInstance->setSOPInstanceUID( SOPInstanceUID );
        // Add the UID of this frame in dicomInstance
        dicomInstance->addReferencedSOPInstanceUID( SOPInstanceUID );   // IMPORTANT for document SR writing

        // Set file names without extension
        this->setFile( fileName + ::fwTools::getString(i) );

        //*****     Write one file      *****//
        DicomInstance< ::fwData::Image >::write();
    }
}

//------------------------------------------------------------------------------

void DicomImage::write3DImage()
{
    //*****     Initialize     *****//
    // Get typed writer
    ::gdcm::ImageWriter *gImageWriter = ::boost::static_pointer_cast< ::gdcm::ImageWriter >( this->getWriter() ).get();

    // Get images
    ::gdcm::Image &gImg = gImageWriter->GetImage();
    ::fwData::Image::sptr image = this->getConcreteObject();

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance = this->getDicomInstance();

    const ::fwData::Image::SizeType & size = image->getSize();
    const unsigned int bufferLength = size[0]*size[1]*size[2] * gImg.GetPixelFormat().GetPixelSize();
    OSLM_TRACE("Image's buffer size : "<<bufferLength);

    //*****     Set buffer     *****//
    // Pixel Data
    ::gdcm::DataElement pixeldata( ::gdcm::Tag(0x7fe0,0x0010) );
    ::fwComEd::helper::Image imgHelper(image);
    pixeldata.SetByteValue( static_cast<char*>(imgHelper.getBuffer()), bufferLength );
    gImg.SetDataElement( pixeldata );

    //*****     Complete the file      *****//
    // Define a unique SOP Instance UID
    ::gdcm::UIDGenerator gUIDgen;
    gUIDgen.SetRoot( dicomInstance->getSOPClassUID().c_str() );
    dicomInstance->setSOPInstanceUID( gUIDgen.Generate() );

    // Set file name without extension
    this->setFile( this->getFile().string() + "/image" );

    //*****     Write the file      *****//
    DicomInstance< ::fwData::Image >::write();
}

//------------------------------------------------------------------------------

void DicomImage::writeImagePlane()
{
    //*****     Initialize     *****//
    ::fwData::Image::csptr image = this->getConcreteObject();

    // Get typed writer
    ::gdcm::ImageWriter *gImageWriter = ::boost::static_pointer_cast< ::gdcm::ImageWriter >( this->getWriter() ).get();
    ::gdcm::DataSet & gDsRoot = this->getDataSet();

    // Get images
    ::gdcm::Image & gImg = gImageWriter->GetImage();

    //Image's number of dimension
    const unsigned int dim = (unsigned int) image->getNumberOfDimensions();

    // Image's spacing
    // WARNING : some DICOM image have not any spacing (NOT SUPPORTED BY FW4SPL), but stuff like "Pixel Aspect Ratio"
    const std::vector<double> & spacing = image->getSpacing();
    for (unsigned int i = 0; i < dim; ++i)
        gImg.SetSpacing(i, spacing[i]);
    OSLM_TRACE("Image's spacing : "<<spacing[0]<<"x"<<spacing[1]<<"x"<<spacing[2]);

    // Slice Thickness
    helper::GdcmData::setTagValue<double,0x0018,0x0050>(spacing[2], gDsRoot);    // Type 2

    //NOTE : gdcm::ImageWriter replace DataElement(0x0020,0x0032) values from image origins.
    //       But, it depends on the modality. So, because GDCM is a good boy, we write nothing and
    //       let him do its cooking.

    // Image Position (Patient)   (See C.7.6.2.1.1)   // Will be modified for 2D images
    // It handles by GDCM ImageWriter throw origin
    const std::vector< double > & origin = image->getOrigin();
    gImg.SetOrigin(0, origin[0]);   gImg.SetOrigin(1, origin[1]);   gImg.SetOrigin(2, origin[2]);

    // Image Orientation (Patient)  (See C.7.6.2.1.1)
    // It handles by GDCM ImageWriter

//    // Slice Location                                   // Will be modified for 2D images
//    ::gdcmIO::helper::GdcmData::setTagValue<0x0020,0x1041>( ::fwTools::getString( image->getOrigin()[2] ), gDsRoot);    // Type 3
//    OSLM_TRACE("Slice location : " << "");
}
//
////------------------------------------------------------------------------------
//
void DicomImage::writeMultiFrame()
{
    // Multi-Frame module (see PS 3.3 C.7.6.6)

    // Get typed writer
    ::gdcm::DataSet & gDsRoot = this->getDataSet();

    ::fwData::Image::csptr  image = this->getConcreteObject();

    // Image's number of frames
    const ::fwData::Image::SizeType & size = image->getSize();
    helper::GdcmData::setTagValue< ::boost::int32_t ,0x0028,0x0008>(size[2], gDsRoot);
    OSLM_TRACE("Image's number of frames : "<< ::fwTools::getString(size[2]));

    // Frame Increment Pointer (0028,0009) //TODO
    helper::GdcmData::setTagValue<int,0x0028,0x0009>(1, gDsRoot);   // It is right?
    OSLM_TRACE("Frame Increment Pointer : " << "1");
}

//------------------------------------------------------------------------------

void DicomImage::writeImagePixel()
{
    ::fwData::Image::csptr image = this->getConcreteObject();

    // Get typed writer
    ::gdcm::ImageWriter * gImageWriter = ::boost::static_pointer_cast< ::gdcm::ImageWriter >( this->getWriter() ).get();

    // Get images
    ::gdcm::Image &gImg = gImageWriter->GetImage();

    // Image's photometric interpretation
    gImg.SetPhotometricInterpretation( helper::GdcmData::getPhotometricInterpretation( *image ) );
    OSLM_TRACE("Image's photometric interpretation : " << helper::GdcmData::getPhotometricInterpretation( *image ) );

    // Image's pixel type
    gImg.SetPixelFormat( helper::GdcmData::getPixelType(*image) );
    OSLM_TRACE("Image's pixel type : "<<helper::GdcmData::getPixelType(*image));

    //Image's number of dimension
    unsigned int dim = (unsigned int) image->getNumberOfDimensions();
    if ( !this->isMultiFrame() )
    {
        dim = 2;
    }
    gImg.SetNumberOfDimensions(dim);
    OSLM_TRACE("Image's number of dimensions : "<<dim);

    // Image's dimension
    const ::fwData::Image::SizeType & size = image->getSize();
    for (unsigned int i = 0; i < dim; i++)
    {
        gImg.SetDimension(i, size[i]);
    }
    OSLM_TRACE("Image's dimensions : "<<size[0]<<"x"<<size[1]<<"x"<<size[2]);

//    if ( color image )    // NOT SUPPORTED BY FW4SPL
//    {
//        // Image's planar configuration
//        gImg.SetPlanarConfiguration(0); // FIXED BY FW4SPL
//        OSLM_TRACE("Image's planar configuration : 0");
//    }

    // Pixel Data (moved to write2DImage() or write3DImage())
}

//------------------------------------------------------------------------------

bool DicomImage::isMultiFrame() const
{
    return this->m_isMultiFrame;
}

//------------------------------------------------------------------------------

void DicomImage::setMultiFrame(const bool a_state)
{
    this->m_isMultiFrame = a_state;
}

//------------------------------------------------------------------------------

void DicomImage::setImageID(const unsigned int a_imageID)
{
    this->m_imageID = ::fwTools::getString(a_imageID);
}

} // namespace instance
} // namespace writer
} // namespace gdcmIO
