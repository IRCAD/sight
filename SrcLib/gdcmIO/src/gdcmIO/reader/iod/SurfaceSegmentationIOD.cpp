/* ***** BEGIN LICENSE BLOCK *****
* FW4SPL - Copyright (C) IRCAD, 2009-2013.
* Distributed under the terms of the GNU Lesser General Public License (LGPL) as
* published by the Free Software Foundation.
* ****** END LICENSE BLOCK ****** */

#include <boost/algorithm/clamp.hpp>

#include <gdcmSegmentReader.h>
#include <gdcmSurfaceReader.h>
#include <gdcmSurfaceHelper.h>

#include <fwData/Boolean.hpp>
#include <fwData/Mesh.hpp>
#include <fwData/Reconstruction.hpp>
#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/container/DicomSurface.hpp"
#include "gdcmIO/reader/iod/SurfaceSegmentationIOD.hpp"
#include "gdcmIO/helper/DicomData.hpp"

namespace gdcmIO
{
namespace reader
{
namespace iod
{

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::SurfaceSegmentationIOD(::fwDicomData::DicomSeries::sptr dicomSeries,
        SPTR(::gdcmIO::container::DicomInstance) instance) :
        ::gdcmIO::reader::iod::InformationObjectDefinition(dicomSeries, instance)
{
    // Instantiates a specific reader
    m_gdcmReader = ::boost::shared_ptr< ::gdcm::SurfaceReader >( new ::gdcm::SurfaceReader );
}

//------------------------------------------------------------------------------

SurfaceSegmentationIOD::~SurfaceSegmentationIOD()
{
   SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::read(::fwMedData::Series::sptr series) throw(::gdcmIO::exception::Failed)
{
   ::fwMedData::ModelSeries::sptr modelSeries = ::fwMedData::ModelSeries::dynamicCast(series);
   SLM_ASSERT("ModelSeries should not be null.", modelSeries);

   // Path container
   ::fwDicomData::DicomSeries::DicomPathContainerType pathContainer = m_dicomSeries->getLocalDicomPaths();
   OSLM_WARN_IF("More than one surface segmentation storage was found in series. Only the first one will be read.",
           pathContainer.size() > 1);

   // Retrieve surface reader
   ::boost::shared_ptr< ::gdcm::SurfaceReader > gdcmSurfaceReader =
           ::boost::static_pointer_cast< ::gdcm::SurfaceReader >( m_gdcmReader );

   // Read first file
   std::string filename = pathContainer.begin()->second.string();
   gdcmSurfaceReader->SetFileName( filename.c_str() );
   if (gdcmSurfaceReader->Read())
   {
       try
       {
           // Read all segmentations and then surfaces
           this->readSurfaceMeshes(modelSeries);
       }
       catch (::gdcmIO::exception::Failed & e)
       {
           OSLM_ERROR("Surface mesh reading error : " << e.what());
       }
   }
   else
   {
       OSLM_ERROR("Surface segmentation reading error : " << filename);
   }

   OSLM_TRACE("Number of reconstructions : " << modelSeries->getReconstructionDB().size());

   // Display reconstructions
   series->setField("ShowReconstructions", ::fwData::Boolean::New(true));
}

//------------------------------------------------------------------------------

void SurfaceSegmentationIOD::readSurfaceMeshes(::fwMedData::ModelSeries::sptr modelSeries)
throw (::gdcmIO::exception::Failed)
{
   ::boost::shared_ptr< ::gdcm::SurfaceReader > gdcmSurfaceReader =
           ::boost::static_pointer_cast< ::gdcm::SurfaceReader >( m_gdcmReader );

   unsigned int skippedSegment = 0;    // Number of rejected segmentation during reading.

   // Read each surface segmentation
   const ::gdcm::SegmentReader::SegmentVector &segmentContainer= gdcmSurfaceReader->GetSegments();
    BOOST_FOREACH(::gdcm::SmartPointer< ::gdcm::Segment > segment, segmentContainer)
    {
        try
        {
            // First read surface segmentation module and then surface mesh module
            ::fwData::Reconstruction::sptr reconstruction = this->readSurfaceMesh(segment);

            // Add the Reconstruction in the ModelSeries
            ::fwMedData::ModelSeries::ReconstructionVectorType reconstructionDB = modelSeries->getReconstructionDB();
            reconstructionDB.push_back(reconstruction);
            modelSeries->setReconstructionDB(reconstructionDB);
        }
        catch (::gdcmIO::exception::Failed & e)
        {
            ++skippedSegment;
            SLM_ERROR("Surface mesh reading error : " + e.what());
        }
    }

    OSLM_WARN("Number of skipped segment : " << skippedSegment);

}

//------------------------------------------------------------------------------

::fwData::Reconstruction::sptr SurfaceSegmentationIOD::readSurfaceMesh( ::gdcm::SmartPointer< ::gdcm::Segment > segment)
throw (::gdcmIO::exception::Failed)
{
   ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

    // We only handle segment containing one surface
    if (segment->GetSurfaceCount() != 1)
    {
       throw ::gdcmIO::exception::Failed("Inappropriate surface count for a segment.");
    }

    // Organ Name - TODO : if empty, try to get label with DicomDictionarySegment
    reconstruction->setOrganName(segment->GetSegmentDescription());
    OSLM_TRACE("Organ Name : " << reconstruction->getOrganName());

    // Structure Type
    reconstruction->setStructureType(segment->GetSegmentLabel());
    OSLM_TRACE("Structure type : " << reconstruction->getStructureType());

   // Get the associated surface of the current segmentation
   ::gdcm::SmartPointer< ::gdcm::Surface > surface = segment->GetSurface(0);

   // Create material
   ::fwData::Material::sptr material = fwData::Material::New();

   // Convert CIE Lab to RGBA
   const unsigned short *lab = surface->GetRecommendedDisplayCIELabValue();
   ::gdcm::SurfaceHelper::ColorArray CIELab(3);
   CIELab[0] = lab[0];
   CIELab[1] = lab[1];
   CIELab[2] = lab[2];
   const std::vector<float> RGB = ::gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(CIELab, 1.);

   // Adapt color to material
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
           ::gdcmIO::helper::DicomData::convertToRepresentationMode( surface->GetRecommendedPresentationType() ) );
   OSLM_TRACE("Reconstruction's structure type : " <<
              ::gdcm::Surface::GetVIEWTypeString( surface->GetRecommendedPresentationType() ) );

   // Finite Volume
   // reconstruction->setIsClosed( (surface->GetFiniteVolume() == ::gdcm::Surface::YES)?true:false );
   // OSLM_TRACE("Reconstruction is closed : " << reconstruction->getCRefIsClosed());

   // Manifold
   if (surface->GetManifold() == ::gdcm::Surface::YES)
   {
       throw ::gdcmIO::exception::Failed("Manifold not handled");
   }

   // Point Coordinates Data
   const ::gdcm::DataElement &gdcmPointCoords = surface->GetPointCoordinatesData();
   const ::gdcm::ByteValue *gdcmPointCoordsBV = gdcmPointCoords.GetByteValue();
   const char *coordBuffer = gdcmPointCoordsBV->GetPointer();

   // Check that the surface contains point coordinates
   if(gdcmPointCoordsBV == 0 || gdcmPointCoordsBV->GetPointer() == 0)
   {
       throw ::gdcmIO::exception::Failed("No point coordinates data");
   }

   // Get size of buffer (bytes)
   const unsigned long coordBufferSize = gdcmPointCoordsBV->GetLength();

   // Compute number of coordinates
   const unsigned long coordSize = coordBufferSize / sizeof(float);

   // Check that the buffer has the correct size according to the number of points
   if ( (coordSize / 3) != surface->GetNumberOfSurfacePoints())
   {
       throw ::gdcmIO::exception::Failed("Corrupted point coordinates data");
   }

   // Mesh Primitive
   ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > gdcmMeshPrimitive = surface->GetMeshPrimitive();
   if (gdcmMeshPrimitive->GetPrimitiveType() != ::gdcm::MeshPrimitive::TRIANGLE)
   {
       throw ::gdcmIO::exception::Failed("Primitive type not handled");
   }

   // Surface Points Normals
   const char *normalBuffer = NULL;
   if (!surface->GetVectorCoordinateData().IsEmpty())
   {
       const ::gdcm::DataElement &gdcmNormalCoords = surface->GetVectorCoordinateData();
       const ::gdcm::ByteValue *gdcmNormalCoordsBV = gdcmNormalCoords.GetByteValue();

       // Check that the surface contains normals
       if(gdcmNormalCoordsBV == 0 || gdcmNormalCoordsBV->GetPointer() == 0)
       {
           throw ::gdcmIO::exception::Failed("No normal coordinates data");
       }

       // Set normal buffer
       normalBuffer = gdcmNormalCoordsBV->GetPointer();

       // Get size of buffer (bytes)
       const unsigned long   normalCoordBufferSize = gdcmNormalCoordsBV->GetLength();

       // Compute number of coordinates
       const unsigned long   normalCoordSize       = normalCoordBufferSize / sizeof(float);

       // Check that the buffer has the correct size according to the number of normals
       if ( (normalCoordSize / 3) != surface->GetNumberOfVectors() && normalCoordSize != coordSize)
       {
           throw ::gdcmIO::exception::Failed("Corrupted normal coordinates data");
       }
   }

   // Triangle Point Index List
   const ::gdcm::DataElement & gdcmPointIndex = gdcmMeshPrimitive->GetPrimitiveData();
   const ::gdcm::ByteValue *   gdcmPointIndexBV = gdcmPointIndex.GetByteValue();
   const char * indexBuffer = gdcmPointIndexBV->GetPointer();

   // Check that the surface contains triangle point list
   if (indexBuffer == 0)
   {
       throw ::gdcmIO::exception::Failed("No triangle point index list");
   }

   // Get size of buffer (bytes)
   const unsigned long indexBufferSize = gdcmPointIndexBV->GetLength();

   // Get number of primitives
   const unsigned long indexSize = indexBufferSize / sizeof (uint32_t); 

   OSLM_TRACE("Reconstruction's number of points : " << coordSize / 3 );
   OSLM_TRACE("Reconstruction's number of cells : " << indexSize / 3 );

   // Create a new Mesh
   ::gdcmIO::container::DicomSurface surfaceContainer;
   ::fwData::Mesh::sptr mesh = surfaceContainer.convertToData(static_cast<float*>(coordBuffer), coordSize, static_cast<uint32_t*>(indexBuffer),
           indexSize, static_cast<float*>(normalBuffer));

   // Set the reconstruction
   reconstruction->setMaterial( material );
   reconstruction->setMesh( mesh );

   return reconstruction;
}


}  // namespace iod
}  // namespace reader
}  // namespace gdcmIO
