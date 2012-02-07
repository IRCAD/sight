/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2012-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// gdcmIO
#include "gdcmIO/writer/DicomSRWriterManager.hpp"
#include "gdcmIO/writer/DicomSRWriter.hpp"

namespace gdcmIO
{

namespace writer
{

//------------------------------------------------------------------------------

DicomSRWriterManager::DicomSRWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSRWriterManager::~DicomSRWriterManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSRWriterManager::write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path)
{
    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    SLM_ASSERT("gdcmIO::DicomInstance not set",this->getDicomInstance().get())

    //*****     Handle SR document      *****//
    DicomSRWriter   docSRWriter;
    docSRWriter.setObject( image );
    docSRWriter.setFile( a_path.string() + "/docSR" );

    //*****     Set instance and the referenced instances     *****//
    docSRWriter.setDicomInstance( this->getDicomInstance() );

    docSRWriter.setDataSet(a_gDs);
    docSRWriter.write();
}

}   // namespace writer

}   // namespace gdcmIO
