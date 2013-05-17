/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwMedData/ModelSeries.hpp>

#include "gdcmIO/writer/module/DicomRTManager.hpp"
#include "gdcmIO/writer/instance/DicomSurface.hpp"


namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomRTManager::DicomRTManager()
{}

//------------------------------------------------------------------------------

DicomRTManager::~DicomRTManager()
{}

//------------------------------------------------------------------------------

void DicomRTManager::write(::gdcm::DataSet & a_gDs,
                           const ::boost::filesystem::path &  a_path) throw(::gdcmIO::exception::Failed)
{
    ::fwMedData::ModelSeries::sptr series = this->getConcreteObject();
    SLM_ASSERT(":fwMedData::ModelSeries not instanced", series);

    SLM_ASSERT("gdcmIO::DicomInstance not set",this->getDicomInstance().get());

    //*****     Handle RT document      *****//
    // First surface segmentation for 3D reconstruction
    if( !series->getReconstructionDB().empty() )
    {
        ::gdcmIO::writer::instance::DicomSurface surfaceWriter;   // Surface segmentation writer
        surfaceWriter.setObject( series );
        surfaceWriter.setFile( a_path.string() + "/surfaces" );

        // Set instance and the referenced one
        surfaceWriter.setDicomInstance( this->getDicomInstance() );

        // Write 3D reconstruction
        surfaceWriter.setDataSet(a_gDs);
        try
        {
            surfaceWriter.write();
        }
        catch(::gdcmIO::exception::Failed &)
        {
            throw;
        }
    }
}

//------------------------------------------------------------------------------

} // namespace module
} // namespace writer
} // namespace gdcmIO
