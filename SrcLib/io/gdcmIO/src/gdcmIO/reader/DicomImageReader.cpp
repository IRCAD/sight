/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/string/trim.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/algorithm/string/classification.hpp>

#include <gdcmImageReader.h>
#include <gdcmIPPSorter.h>

#include <fwTools/dateAndTime.hpp>
#include <fwTools/fromIsoExtendedString.hpp>

#include "gdcmIO/reader/DicomImageReader.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{

namespace reader
{

//------------------------------------------------------------------------------

DicomImageReader::DicomImageReader():
        m_zSpacing(0)
{
    SLM_TRACE_FUNC();

    // Instantiates a specific reader
    this->setReader( ::boost::shared_ptr< ::gdcm::ImageReader >( new ::gdcm::ImageReader ) );
}

//------------------------------------------------------------------------------

DicomImageReader::~DicomImageReader()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------
// sortInstanceNumber : Operator which compare instance number (see tag(0020,0013))
//                      of two data sets taken from different files.
//------------------------------------------------------------------------------
bool sortInstanceNumber(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2 )
{
  gdcm::Attribute<0x0020,0x0013> at1;
  at1.Set( ds1 );
  gdcm::Attribute<0x0020,0x0013> at2;
  at2.Set( ds2 );

  return at1 < at2;
}

//------------------------------------------------------------------------------
// sortSliceLocation : Operator which compare slice location (see tag(0020,1041))
//                     of two data sets taken from different files.
//------------------------------------------------------------------------------
bool sortSliceLocation(gdcm::DataSet const & ds1, gdcm::DataSet const & ds2 )
{
  gdcm::Attribute<0x0020,0x1041> at1;
  at1.Set( ds1 );
  gdcm::Attribute<0x0020,0x1041> at2;
  at2.Set( ds2 );

  return at1 < at2;
}

//------------------------------------------------------------------------------

void DicomImageReader::read() throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    //*****     Get all file names      *****//
    std::vector< std::string > & imageFiles = this->getRefFileNames(); // files which define one image (2D or 3D)
    OSLM_TRACE("Number of files for an image : " << imageFiles.size());

    //*****     Sort file names      *****//
    // In case of volume image reading from several files, their names have to be sorted
    if ( imageFiles.size() > 1 )
    {
        // Sort files
        ::gdcm::IPPSorter gIPPSorter;           // Sort from Image Position Patient (IPP) and Image Orientation Patient (IOP)
        gIPPSorter.SetComputeZSpacing( true );  // In the same time, compute the spacing between each frame with the IPP
        gIPPSorter.SetZSpacingTolerance( 1e-3 );
        if ( gIPPSorter.Sort( imageFiles ) )
        {
            SLM_TRACE ( "Success to IPPSort" );

            // Get depth spacing
            m_zSpacing = gIPPSorter.GetZSpacing();
            if (m_zSpacing != 0)
            {
                OSLM_TRACE ( "Found z-spacing : " << m_zSpacing);
            }

            // Get IPP sorted file names
            this->setFileNames( gIPPSorter.GetFilenames() );
        }
        else
        {
            SLM_TRACE ( "Failed to IPPSort" );

            // Try to sort from instance number and slice location.
            // Because, some SOP Class UIDs (like CT image) have not IPP

            gdcm::Sorter gSorter;
            gSorter.SetSortFunction( sortInstanceNumber );  // Try instance number sort
            if ( !gSorter.StableSort( imageFiles ) )
            {
                SLM_TRACE ( "Failed to Instance Number sort" );

                // Try slice location sort
                gSorter.SetSortFunction( sortSliceLocation );
                if ( !gSorter.StableSort( imageFiles ) )
                {
                    // Give up
                    throw ::fwTools::Failed("Files sort failed");
                }
            }
            // Get sorted file names
            this->setFileNames( gSorter.GetFilenames() );
        }
    }

    //*****     Read image      *****//
    // Get the gdcm reader (instantiate in the constructor)
    ::gdcm::ImageReader & gImageReader = *( ::boost::static_pointer_cast< ::gdcm::ImageReader >( this->getReader() ) );

    // Read the first file and others if necessary
    gImageReader.SetFileName( this->getCRefFileNames()[0].c_str() );
    if (gImageReader.Read())
    {
        ::fwData::Image::NewSptr img;
        try
        {
            // Set the fwData::image
            this->readImage(img);

            // Add a complete image to the series
            this->getConcreteObject()->setImage(img);
        }
        catch (::fwTools::Failed &e)
        {
            std::stringstream ss;   ss.str("");
            ss << "Error found in DICOM image : " << e.what();
            throw ::fwTools::Failed(ss.str());
        }
    }
    else
    {
        throw ::fwTools::Failed("DICOM image reading failed");
    }
}

//------------------------------------------------------------------------------

void DicomImageReader::readImage( ::fwData::Image::sptr img) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    // List of tag use in this method
//    const ::gdcm::Tag iSBSTag(0x0018,0x0088); // Spacing Between Slices
////    const ::gdcm::Tag iDimTag(0x0028,0x0005); // Image Dimensions
////    const ::gdcm::Tag iNbFTag(0x0028,0x0008); // Number of Frames
////    const ::gdcm::Tag iRowTag(0x0028,0x0010); // Rows
////    const ::gdcm::Tag iColTag(0x0028,0x0011); // Columns
//    const ::gdcm::Tag iCenTag(0x0028,0x1050); // Window Center
//    const ::gdcm::Tag iWidTag(0x0028,0x1051); // Window Width
////    const ::gdcm::Tag iResTag(0x0028,0x1052); // Rescale Intercept

    // Init
    ::fwData::Acquisition::sptr series  = this->getConcreteObject();
    ::gdcm::Image &             gImg    = ::boost::static_pointer_cast< ::gdcm::ImageReader >( this->getReader() )->GetImage();
    const ::gdcm::DataSet &     a_ds    = this->getDataSet();

    // DicomInstance is necessary for SR reading
    ::boost::shared_ptr< DicomInstance > dicomInstance = this->getDicomInstance();
    SLM_ASSERT("gdcmIO::DicomInstance not set", dicomInstance);

    // Reference the first SOP Instance UID in dicomInstance for SR reading
    const std::string refSOPInstanceUID = helper::GdcmData::getTagValue<0x0008,0x0018>(a_ds);   // SOP Instance UID
    if ( !refSOPInstanceUID.empty() )
        dicomInstance->addReferencedSOPInstanceUID(refSOPInstanceUID);
    else
        OSLM_WARN("Empty SOP instance UID found");

    // Image's origin
    //WARNING: NOT IMPLEMENTED IN VTK. So, if origin != (0,0,0) then there is a bad display.
    const double * gOrigin = gImg.GetOrigin();
    if ( gOrigin != 0 )
    {
        std::copy( gOrigin, gOrigin+3, img->getRefOrigin().begin() );
        OSLM_TRACE("Image's origin : "<<img->getRefOrigin()[0]<<"x"<<img->getRefOrigin()[1]<<"x"<<img->getRefOrigin()[2]);
    }

    // Image's pixel data
    // WARNING : idea : image is multiframe if there is more than one file names here
    void *      gdcmBuffer;
    const bool  isMultiFrame = (this->getFileNames().size() > 1 ? false : true);
    dicomInstance->setIsMultiFrame( isMultiFrame );
    if ( !isMultiFrame )
    {// Read one image from several files
        // TODO : test if numberOfFrames == number of files
        // (some files could be erased by an over series)

        // Get raw buffer of 3D image even if it is encoded (JPEG, ...)
        gdcmBuffer = this->read2DImages();  // NOTE : Can modify gdcm::Image attributes

        // Complete fwData::Image
        img->setDimension(3);
        img->getRefSize()[2] = this->getFileNames().size();
    }
    else
    {// Read 2D or 3D image from one file
        // Allocate raw buffer
        const unsigned long sizeOfImage = gImg.GetBufferLength();
        try
        {
            gdcmBuffer = new char[sizeOfImage];
            OSLM_TRACE("Global buffer size : " << sizeOfImage);
        }
        catch (...)
        {
            throw ::fwTools::Failed("Need more memory");
        }

        // Get raw buffer of image even if it is encoded (JPEG, ...)
          if ( !gImg.GetBuffer( (char*) gdcmBuffer ) )
              throw ::fwTools::Failed("Failed to get image buffer");
    }

    // Convert color image into grayscale one and set the fwData::Image buffer
    try
    {
        helper::GdcmData::convertGdcmToDataBuffer(gImg, img, gdcmBuffer);
    }
    catch (::fwTools::Failed &e)
    {
        throw;
    }

    // Image's dimension
    const unsigned int dim = gImg.GetNumberOfDimensions();
    img->setDimension(dim);
    OSLM_TRACE("Image's dim : "<<dim);

    // Image's size
    const unsigned int * gDim = gImg.GetDimensions();
    if ( gDim != 0 )
    {
        std::copy( gDim, gDim+dim, img->getRefSize().begin() );
    }
    OSLM_TRACE("Image's size : "<<img->getRefSize()[0]<<"x"<<img->getRefSize()[1]<<"x"<<img->getRefSize()[2]);


    // Image's spacing
    const double * gSpacing = gImg.GetSpacing();
    if ( gSpacing != 0 )
    {
        std::copy( gSpacing, gSpacing+dim, img->getRefSpacing().begin() );
    }

    // Slice Thickness
    std::string sliceThicknessStr = helper::GdcmData::getTagValue<0x0018,0x0050>(a_ds);

    if (dim < 3)                // 2D image
    {
        img->getRefSpacing()[2] = 1;
        img->getRefSize()[2]    = 1;
    }
    else if ( !isMultiFrame )   // 3D image from several files
    {
        // Try to get z spacing

        //** From IPP **//
        if (m_zSpacing != 0)
        {
            img->getRefSpacing()[2] = m_zSpacing;
        }
        else
        {
            //** From Spacing Between Slices **//
            // Spacing Between Slices
            std::string spacingOnZstr = helper::GdcmData::getTagValue<0x0018,0x0088>(a_ds);
            if ( !spacingOnZstr.empty() )
            {
                const float spacingOnZ = atof( spacingOnZstr.c_str() );
                if (spacingOnZ != img->getCRefSpacing()[2])
                    img->getRefSpacing()[2] = spacingOnZ;
            }
            else
            {
                //** From Slice Thickness **//
                // WARNING: slice thickness could be = zSpacing or = 2 * zSpacing, I don't know why, DICOM magic?
                if ( !sliceThicknessStr.empty() )
                {
                    const float sliceThickness = atof( sliceThicknessStr.c_str() );
                    if (sliceThickness != img->getCRefSpacing()[2])
                        img->getRefSpacing()[2] = sliceThickness;
                }
                else
                {
                    // Let's give up
                    throw ::fwTools::Failed("Failed to get depth spacing");
                }
            }
        }
    }
    OSLM_TRACE("Image's spacing : "<<img->getCRefSpacing()[0]<<"x"<<img->getCRefSpacing()[1]<<"x"<<img->getCRefSpacing()[2]);

    // Slice Thickness
    if (sliceThicknessStr.empty())
    {
        // Try to get slice thickness from z spacing
        // WARNING: could be wrong (See C.7.6.2.1.1 and C.7.6.16.2.3.1)
        sliceThicknessStr = ::fwTools::getString( img->getCRefSpacing()[2] );
    }
    series->setSliceThickness( atof( sliceThicknessStr.c_str() ) );
    OSLM_TRACE("Slice thickness : " << sliceThicknessStr);

    // Image's pixel type
    img->setPixelType( helper::GdcmData::getPixelType(gImg) );
    OSLM_TRACE("Image's pixel type: "<<img->getCRefPixelType().string());

    //Image's window center (double)
    std::string                 windowCenter = helper::GdcmData::getTagValue<0x0028,0x1050>(a_ds);
    std::vector<std::string>    splitedWindows;
    if ( !windowCenter.empty() )
    {
        ::boost::split( splitedWindows,       windowCenter,      ::boost::is_any_of( "\\" ) );  // Get each windowCenter
        img->setCRefWindowCenter( atof( splitedWindows[0].c_str() ) );                          // Assign first one
    }
    OSLM_TRACE("Image's window center : "<<img->getWindowCenter());

    //Image's window width (double)
    std::string windowWidth = helper::GdcmData::getTagValue<0x0028,0x1051>(a_ds);
    if ( !windowWidth.empty() )
    {
        ::boost::split( splitedWindows,       windowWidth,      ::boost::is_any_of( "\\" ) );  // Get each windowWidth
        img->setCRefWindowWidth( atof( splitedWindows[0].c_str() ) );                          // Assign first one
    }
    OSLM_TRACE("Image's window width : "<<img->getWindowWidth());

    // Image's rescale intercept (double)    // NOT IMPLEMENTED IN VTK
    img->setCRefRescaleIntercept(gImg.GetIntercept());
    OSLM_TRACE("Image's rescale intercept : "<<img->getRescaleIntercept());

    // Image's path
    std::string path = this->getFileNames()[0];
    size_t      pos = path.find_last_of("/");
    path = path.substr(0, pos);
    img->setCRefFilename( path );
    OSLM_TRACE("Image path : " << path);
}

//------------------------------------------------------------------------------

void * DicomImageReader::read2DImages() throw(::fwTools::Failed)
{
    ::boost::shared_ptr< ::gdcm::ImageReader >  gImageReader    = ::boost::static_pointer_cast< ::gdcm::ImageReader >( this->getReader() );
    ::gdcm::Image &                             gImg            = gImageReader->GetImage();

    char *                                      gdcmGlobalBuffer;                           // Raw buffer for all frames
    const unsigned long                         sizeOfFrame     = gImg.GetBufferLength();   // all frames have the same size as the first one
    const std::vector< std::string > &          fileNames       = this->getCRefFileNames();
    const unsigned int                          nbFrames        = fileNames.size();

    // Allocate raw buffer
    try
    {
        gdcmGlobalBuffer    = new char[sizeOfFrame * nbFrames];
        OSLM_TRACE("Global buffer size : " << sizeOfFrame * nbFrames);
    }
    catch (...)
    {
        throw ::fwTools::Failed("Need more memory");
    }

    // Get first frame buffer
      if ( !gImg.GetBuffer( gdcmGlobalBuffer ) )
          throw ::fwTools::Failed("Failed to get first frame buffer");

    // Get other frame buffers
    ::boost::shared_ptr< DicomInstance >    dicomInstance   = this->getDicomInstance();
    ::gdcm::DataSet &                       gHeRoot         = gImageReader->GetFile().GetHeader();
    ::gdcm::DataSet &                       gDsRoot         = gImageReader->GetFile().GetDataSet();
    for (unsigned int i = 1; i < nbFrames; ++i) // First file already read
    {
        // Clear data because GDCM reader does not read twice the same tag
        gHeRoot.Clear();
        gDsRoot.Clear();

        // Read a frame
        gImageReader->SetFileName( fileNames[i].c_str() );
        if ( gImageReader->Read() )
        {
            // Get raw buffer and set it in the image buffer
            if ( !gImg.GetBuffer( gdcmGlobalBuffer + i * sizeOfFrame ) )
                throw ::fwTools::Failed("Failed to get a frame buffer");

            // Add a new referenced SOP Instance UID
            dicomInstance->addReferencedSOPInstanceUID( helper::GdcmData::getTagValue<0x0008,0x0018>(gDsRoot) );
        }
        else
        {
            std::stringstream ss;
            ss.str(""); ss<<"Reading error on frame : "<<i;
            throw ::fwTools::Failed(ss.str());
        }
    }

    // Adapt gdcm::Image to call helper::GdcmData::convertGdcmToDataBuffer(gImg, img)
    gImg.SetNumberOfDimensions(3);
    gImg.SetDimension(2, nbFrames);

    return (void*)gdcmGlobalBuffer;
}

}  // namespace reader

}  // namespace gdcmIO
