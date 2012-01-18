/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// gdcmIO
#include "gdcmIO/writer/DicomRTWriterManager.hpp"
#include "gdcmIO/writer/DicomSurfaceWriter.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomRTWriterManager::DicomRTWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomRTWriterManager::~DicomRTWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomRTWriterManager::write(::gdcm::DataSet &                  a_gDs,
                                 const ::boost::filesystem::path &  a_path) throw(::fwTools::Failed)
{
    SLM_TRACE_FUNC();

    ::fwData::Acquisition::sptr series = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", series);

    SLM_ASSERT("gdcmIO::DicomInstance not set",this->getDicomInstance().get())

    //*****     Handle RT document      *****//
    // First surface segmentation for 3D reconstruction
    if( hasSurface() )
    {
        DicomSurfaceWriter surfaceWriter;   // Surface segmentation writer
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
        catch(::fwTools::Failed & e)
        {
            throw;
        }
    }
}

//------------------------------------------------------------------------------

bool DicomRTWriterManager::hasSurface()
{
    ::fwData::Acquisition::sptr     series          = this->getConcreteObject();
    ::fwData::Reconstruction::sptr  reconstruction  = *(series->getReconstructions().first);

    if (reconstruction)
        return (reconstruction->getMesh() != 0 ? true : false);

    return false;
}

}   // namespace writer

}   // namespace gdcmIO
