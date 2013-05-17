/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/clamp.hpp>

#include <gdcmSurfaceReader.h>
#include <gdcmSurfaceHelper.h>

#include <fwData/Boolean.hpp>
#include <fwData/Mesh.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/reader/dicom/DicomSurface.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

namespace gdcmIO
{
namespace reader
{
namespace dicom
{

//------------------------------------------------------------------------------

DicomSurface::DicomSurface()
{
   SLM_TRACE_FUNC();

   // Instantiate the appropriated reader
   this->setReader( ::boost::shared_ptr< ::gdcm::SurfaceReader >( new ::gdcm::SurfaceReader ) );

   // Init the tool to convert DICOM surface into FW4SPL one
   this->m_surfaces = ::boost::shared_ptr< ::gdcmIO::container::DicomSurface>( new ::gdcmIO::container::DicomSurface );
}

//------------------------------------------------------------------------------

DicomSurface::~DicomSurface()
{
   SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSurface::read() throw (::gdcmIO::exception::Failed)
{
   SLM_TRACE_FUNC();

   ::fwMedData::ModelSeries::sptr series = this->getConcreteObject();
   SLM_ASSERT("fwMedData::ModelSeries not instanced", series);

   //*****     Get all file names      *****//
   const std::vector< std::string > &  surfaceFiles   = this->getCRefFileNames();
   const unsigned int                  nbSurfaceFiles = surfaceFiles.size();
   OSLM_TRACE("Number of surface segmentation files : " << nbSurfaceFiles);
   if (nbSurfaceFiles > 1)
   {
       OSLM_WARN("More than one surface segmentation storage was found in series. Just the first one will be read.");
   }

   //*****     Read first file      *****//
   ::boost::shared_ptr< ::gdcm::SurfaceReader > gSurfaceReader = ::boost::static_pointer_cast< ::gdcm::SurfaceReader >( this->getReader() );

   gSurfaceReader->SetFileName( surfaceFiles[0].c_str() );
   if (gSurfaceReader->Read())
   {
       OSLM_TRACE("Surface file reading : "<<surfaceFiles[0].c_str());

       try
       {// Read all segmentations and then surfaces
           this->readSurfaceMeshs();
       }
       catch (::gdcmIO::exception::Failed & e)
       {
           OSLM_ERROR("Surface mesh reading error : " << e.what());
       }
   }
   else
   {
       OSLM_ERROR("Surface segmentation reading error : "<<surfaceFiles[0]);
   }

   OSLM_TRACE("series->getNumberOfReconstructions() : " << series->getReconstructionDB().size());

   series->setField("ShowReconstructions", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------

void DicomSurface::readSurfaceMeshs() throw (::gdcmIO::exception::Failed)
{
   ::boost::shared_ptr< ::gdcm::SurfaceReader > gSurfaceReader = ::boost::static_pointer_cast< ::gdcm::SurfaceReader >( this->getReader() );

   unsigned int skippedSegment = 0;    // Number of rejected segmentation during reading.

   // Read each surface segmentation
   const ::gdcm::SegmentReader::SegmentVector &           segments= gSurfaceReader->GetSegments(); // Get all segmentations with their associated surface
   ::gdcm::SegmentReader::SegmentVector::const_iterator   it      = segments.begin();
   ::gdcm::SegmentReader::SegmentVector::const_iterator   itEnd   = segments.end();
   for (; it != itEnd; ++it)
   {
       ::gdcm::SmartPointer< ::gdcm::Segment > segment = *it;

       try
       {
           // First read surface segmentation module and then surface mesh module
           this->readSurfaceMesh(segment);
       }
       catch (::gdcmIO::exception::Failed & e)
       {
           ++skippedSegment;
           OSLM_ERROR("Surface mesh reading error : " << e.what());
       }
   }
}

//------------------------------------------------------------------------------

void DicomSurface::readSurfaceMesh( ::gdcm::SmartPointer< ::gdcm::Segment > a_segment) throw (::gdcmIO::exception::Failed)
{
   ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

   try
   {
       // Read segmentation
       DicomSegment::readSurfaceSegmentation(reconstruction, a_segment);
   }
   catch (::gdcmIO::exception::Failed & e)
   {
       throw;
   }

//    if (segment->GetSurfaceCount() != 1)      // It already Checked in DicomSegment::readSurfaceSegmentation( segment )
//        throw ::gdcmIO::exception::Failed("A segment was skipped : unappropriated surface count");
   // Get the associated surface of the current segmentation
   ::gdcm::SmartPointer< ::gdcm::Surface > surface = a_segment->GetSurface(0);

   //*****     Set the reconstruction      *****//
   fwData::Material::sptr material = fwData::Material::New();

   // Convert CIE Lab to RGBA
   const unsigned short * lab = surface->GetRecommendedDisplayCIELabValue();
   ::gdcm::SurfaceHelper::ColorArray   CIELab(3);
   CIELab[0] = lab[0]; CIELab[1] = lab[1]; CIELab[2] = lab[2];
   const std::vector<float> RGB = ::gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(CIELab, 1.);

   // Adapt to material
   ::boost::array<float, 4> rgba;
   ::boost::algorithm::clamp_range(RGB.begin(), RGB.end(), rgba.begin(), 0.f, 1.f);

   // Recommended Presentation Opacity
   const float opacity = ::boost::algorithm::clamp(surface->GetRecommendedPresentationOpacity(), 0.f, 1.f);
   rgba[3] = opacity; // set alpha component

   // Set reconstruction's visibility
   reconstruction->setIsVisible(opacity > 1e-3);
   OSLM_TRACE("Reconstruction is visible : " << reconstruction->getIsVisible());

   // Set reconstruction's color
   material->ambient()->setRGBA( rgba );
   OSLM_TRACE("RGBA color : " << rgba[0]<<" "<< rgba[1]<<" "<< rgba[2]<<" "<< rgba[3]);

   // Recommended Presentation Type
   material->setRepresentationMode(
           helper::GdcmData::convertToRepresentationMode( surface->GetRecommendedPresentationType() ) );
   OSLM_TRACE("Reconstruction's structure type : " <<
              ::gdcm::Surface::GetVIEWTypeString( surface->GetRecommendedPresentationType() ) );

   // Finite Volume
   // reconstruction->setIsClosed( (surface->GetFiniteVolume() == ::gdcm::Surface::YES)?true:false );
   // OSLM_TRACE("Reconstruction is closed : " << reconstruction->getCRefIsClosed());

   // Manifold
   if (surface->GetManifold() == ::gdcm::Surface::YES)
       throw ::gdcmIO::exception::Failed("Manifold not handled");
   // Let's try for a Manifold == UNKNOWN

   // Point Coordinates Data
   const ::gdcm::DataElement & gPointCoords    = surface->GetPointCoordinatesData();
   const ::gdcm::ByteValue *   gpointCoordsBV  = gPointCoords.GetByteValue();

   if(gpointCoordsBV == 0 || gpointCoordsBV->GetPointer() == 0)
       throw ::gdcmIO::exception::Failed("No point coordinates data");

   const char * coordBuffer     = gpointCoordsBV->GetPointer();

   const unsigned long         coordBufferSize = gpointCoordsBV->GetLength();      // Get size of buffer (bytes)
   const unsigned long         coordSize       = coordBufferSize / sizeof(float);  // Compute number of coordinates
   if ( (coordSize / 3) != surface->GetNumberOfSurfacePoints())
       throw ::gdcmIO::exception::Failed("Corrupted point coordinates data");
   OSLM_TRACE("Reconstruction's number of points : " << coordSize / 3 );

   // Mesh Primitive
   ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > gMeshPrimitive = surface->GetMeshPrimitive();
   if (gMeshPrimitive->GetPrimitiveType() != ::gdcm::MeshPrimitive::TRIANGLE)
       throw ::gdcmIO::exception::Failed("Primitive type not handled");

   // Surface Points Normals
   const char * normalCoordBuffer = NULL;
   if (!surface->GetVectorCoordinateData().IsEmpty())
   {
       const ::gdcm::DataElement & gNormalCoords    = surface->GetVectorCoordinateData();
       const ::gdcm::ByteValue *   gNormalCoordsBV  = gNormalCoords.GetByteValue();

       if(gNormalCoordsBV == 0 || gNormalCoordsBV->GetPointer() == 0)
           throw ::gdcmIO::exception::Failed("No normal coordinates data");

       normalCoordBuffer = gNormalCoordsBV->GetPointer();

       const unsigned long   normalCoordBufferSize = gNormalCoordsBV->GetLength();          // Get size of buffer (bytes)
       const unsigned long   normalCoordSize       = normalCoordBufferSize / sizeof(float); // Compute number of coordinates
       if ( (normalCoordSize / 3) != surface->GetNumberOfVectors() && normalCoordSize != coordSize)
           throw ::gdcmIO::exception::Failed("Corrupted normal coordinates data");
   }

   // Triangle Point Index List
   const ::gdcm::DataElement & gPointIndex     = gMeshPrimitive->GetPrimitiveData();
   const ::gdcm::ByteValue *   gpointIndexBV   = gPointIndex.GetByteValue();
   const char *                indexBuffer     = gpointIndexBV->GetPointer();
   if (indexBuffer == 0)
       throw ::gdcmIO::exception::Failed("No triangle point index list");
   const unsigned long         indexBufferSize = gpointIndexBV->GetLength();           // Get size of buffer (bytes)
   const unsigned long         indexSize       = indexBufferSize / sizeof (uint32_t);  // Get number of primitives
   OSLM_TRACE("Reconstruction's number of cells : " << indexSize / 3 );

   // Create a new Mesh
   ::fwData::Mesh::sptr mesh = m_surfaces->convertToData((float*)coordBuffer,
                                                         coordSize,
                                                         (uint32_t*) indexBuffer,
                                                         indexSize,
                                                         (float*)normalCoordBuffer);

   // Set the reconstruction
   reconstruction->setMaterial( material );
   reconstruction->setMesh( mesh );

   // Add the reconstruction in the acquisition
   ::fwMedData::ModelSeries::sptr series = this->getConcreteObject();
   // reconstruction->setImage( series->getImage() );
   ::fwMedData::ModelSeries::ReconstructionVectorType recs = series->getReconstructionDB();
   recs.push_back(reconstruction);
   series->setReconstructionDB( recs );
}

}  // namespace dicom
}  // namespace reader
}  // namespace gdcmIO
