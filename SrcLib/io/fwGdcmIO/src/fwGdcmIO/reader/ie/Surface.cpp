/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

//To include first because of Windows compilation
#include <gdcmSurfaceHelper.h>
#include <gdcmUIDGenerator.h>

#include "fwGdcmIO/container/DicomSurface.hpp"
#include "fwGdcmIO/helper/DicomData.hpp"
#include "fwGdcmIO/reader/ie/Surface.hpp"


#include <fwData/Color.hpp>
#include <fwData/Reconstruction.hpp>

#include <fwDataIO/reader/DictionaryReader.hpp>

#include <fwDataTools/helper/Mesh.hpp>

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/Series.hpp>
#include <fwMedData/types.hpp>

#include <fwRuntime/profile/Profile.hpp>

#include <fwTools/dateAndTime.hpp>

#include <boost/algorithm/clamp.hpp>
#include <boost/algorithm/string/trim.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

namespace fwGdcmIO
{
namespace reader
{
namespace ie
{

//------------------------------------------------------------------------------

Surface::Surface(::fwMedData::DicomSeries::sptr dicomSeries,
                 SPTR(::gdcm::Reader)reader,
                 SPTR(::fwGdcmIO::container::DicomInstance)instance,
                 ::fwMedData::ModelSeries::sptr series,
                 ::fwLog::Logger::sptr logger,
                 const ProgressCallback& callback,
                 const bool& cancelled) :
    ::fwGdcmIO::reader::ie::InformationEntity< ::fwMedData::ModelSeries >(dicomSeries, reader, instance, series,
                                                                          logger, callback, cancelled)
{
}

//------------------------------------------------------------------------------

Surface::~Surface()
{
}

//------------------------------------------------------------------------------

void Surface::readSurfaceSegmentationModule(::gdcm::SmartPointer< ::gdcm::Segment > segment)
{
    ::fwData::Reconstruction::sptr reconstruction = ::fwData::Reconstruction::New();

    // We only handle segment containing one surface
    if (segment->GetSurfaceCount() != 1)
    {
        throw ::fwGdcmIO::exception::Failed("Inappropriate surface count for a segment.");
    }

    // Organ Name - TODO : if empty, try to get label with DictionarySegment
    std::string organName = segment->GetSegmentDescription();
    ::boost::algorithm::trim(organName);
    reconstruction->setOrganName(organName);
    SLM_TRACE("Organ Name : " + reconstruction->getOrganName());

    // Structure Type
    std::string structureType = segment->GetSegmentLabel();
    ::boost::algorithm::trim(structureType);
    reconstruction->setStructureType(structureType);
    SLM_TRACE("Structure type : " + reconstruction->getStructureType());

    // Get the associated surface of the current segmentation
    ::gdcm::SmartPointer< ::gdcm::Surface > surface = segment->GetSurface(0);

    this->readSurfaceMeshModule(surface, reconstruction);

    // Add the Reconstruction in the ModelSeries
    ::fwMedData::ModelSeries::ReconstructionVectorType reconstructionDB = m_object->getReconstructionDB();
    reconstructionDB.push_back(reconstruction);
    m_object->setReconstructionDB(reconstructionDB);

}

//------------------------------------------------------------------------------

void Surface::readSurfaceMeshModule(::gdcm::SmartPointer< ::gdcm::Surface > surface,
                                    ::fwData::Reconstruction::sptr reconstruction)
{
    // Create material
    ::fwData::Material::sptr material = fwData::Material::New();

    // Convert CIE Lab to RGBA
    const unsigned short* lab = surface->GetRecommendedDisplayCIELabValue();
    ::gdcm::SurfaceHelper::ColorArray CIELab(3);
    CIELab[0] = lab[0];
    CIELab[1] = lab[1];
    CIELab[2] = lab[2];
    const std::vector<float> RGB = ::gdcm::SurfaceHelper::RecommendedDisplayCIELabToRGB(CIELab, 1.);

    // Adapt color to material
    ::fwData::Color::ColorArray rgba;
    ::boost::algorithm::clamp_range(RGB.begin(), RGB.end(), rgba.begin(), 0.f, 1.f);

    // Recommended Presentation Opacity
    const float opacity = ::boost::algorithm::clamp(surface->GetRecommendedPresentationOpacity(), 0.f, 1.f);
    rgba[3] = opacity; // set alpha component

    // Set reconstruction's visibility
    reconstruction->setIsVisible(opacity > 1e-3);
    OSLM_TRACE("Reconstruction is visible : " << reconstruction->getIsVisible());

    // Set reconstruction's color
    ::fwData::Color::sptr color = ::fwData::Color::New();
    color->setRGBA( rgba );
    material->setDiffuse(color);
    OSLM_TRACE("RGBA color : " << rgba[0]<<" "<< rgba[1]<<" "<< rgba[2]<<" "<< rgba[3]);

    // Recommended Presentation Type
    material->setRepresentationMode(
        ::fwGdcmIO::helper::DicomData::convertToRepresentationMode( surface->GetRecommendedPresentationType() ) );
    OSLM_TRACE("Reconstruction's structure type : " <<
               ::gdcm::Surface::GetVIEWTypeString( surface->GetRecommendedPresentationType() ) );

    // Manifold
    if (surface->GetManifold() == ::gdcm::Surface::YES)
    {
        throw ::fwGdcmIO::exception::Failed("Manifold not handled");
    }

    // Point Coordinates Data
    const ::gdcm::DataElement& gdcmPointCoords = surface->GetPointCoordinatesData();
    const ::gdcm::ByteValue* gdcmPointCoordsBV = gdcmPointCoords.GetByteValue();
    const char* coordBuffer                    = gdcmPointCoordsBV->GetPointer();

    // Check that the surface contains point coordinates
    if(gdcmPointCoordsBV == 0 || gdcmPointCoordsBV->GetPointer() == 0)
    {
        throw ::fwGdcmIO::exception::Failed("No point coordinates data");
    }

    // Get size of buffer (bytes)
    const unsigned long coordBufferSize = gdcmPointCoordsBV->GetLength();

    // Compute number of coordinates
    const unsigned long coordSize = coordBufferSize / sizeof(float);

    // Check that the buffer has the correct size according to the number of points
    if ( (coordSize / 3) != surface->GetNumberOfSurfacePoints())
    {
        throw ::fwGdcmIO::exception::Failed("Corrupted point coordinates data");
    }

    // Mesh Primitive
    ::gdcm::SmartPointer< ::gdcm::MeshPrimitive > gdcmMeshPrimitive = surface->GetMeshPrimitive();
    if (gdcmMeshPrimitive->GetPrimitiveType() != ::gdcm::MeshPrimitive::TRIANGLE)
    {
        throw ::fwGdcmIO::exception::Failed("Primitive type not handled");
    }

    // Surface Points Normals
    const char* normalBuffer = NULL;
    if (!surface->GetVectorCoordinateData().IsEmpty())
    {
        const ::gdcm::DataElement& gdcmNormalCoords = surface->GetVectorCoordinateData();
        const ::gdcm::ByteValue* gdcmNormalCoordsBV = gdcmNormalCoords.GetByteValue();

        // Check that the surface contains normals
        if(gdcmNormalCoordsBV == 0 || gdcmNormalCoordsBV->GetPointer() == 0)
        {
            throw ::fwGdcmIO::exception::Failed("No normal coordinates data");
        }

        // Set normal buffer
        normalBuffer = gdcmNormalCoordsBV->GetPointer();

        // Get size of buffer (bytes)
        const unsigned long normalCoordBufferSize = gdcmNormalCoordsBV->GetLength();

        // Compute number of coordinates
        const unsigned long normalCoordSize = normalCoordBufferSize / sizeof(float);

        // Check that the buffer has the correct size according to the number of normals
        if ( (normalCoordSize / 3) != surface->GetNumberOfVectors() && normalCoordSize != coordSize)
        {
            throw ::fwGdcmIO::exception::Failed("Corrupted normal coordinates data");
        }
    }

    // Triangle Point Index List
    const ::gdcm::DataElement& gdcmPointIndex   = gdcmMeshPrimitive->GetPrimitiveData();
    const ::gdcm::ByteValue*   gdcmPointIndexBV = gdcmPointIndex.GetByteValue();
    const char* indexBuffer                     = gdcmPointIndexBV->GetPointer();

    // Check that the surface contains triangle point list
    if (indexBuffer == 0)
    {
        throw ::fwGdcmIO::exception::Failed("No triangle point index list");
    }

    // Get size of buffer (bytes)
    const unsigned long indexBufferSize = gdcmPointIndexBV->GetLength();

    // Get number of primitives
    const unsigned long indexSize = indexBufferSize / sizeof (uint32_t);

    OSLM_TRACE("Reconstruction's number of points : " << coordSize / 3 );
    OSLM_TRACE("Reconstruction's number of cells : " << indexSize / 3 );

    // Create a new Mesh
    ::fwGdcmIO::container::DicomSurface surfaceContainer;
    ::fwData::Mesh::sptr mesh = surfaceContainer.convertToData(reinterpret_cast<const float*>(coordBuffer), coordSize,
                                                               reinterpret_cast<const uint32_t*>(indexBuffer),
                                                               indexSize, reinterpret_cast<const float*>(normalBuffer));

    // Set the reconstruction
    reconstruction->setMaterial( material );
    reconstruction->setMesh( mesh );

}

//------------------------------------------------------------------------------

} // namespace ie
} // namespace reader
} // namespace fwGdcmIO
