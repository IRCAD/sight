/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

// gdcmIO
#include "gdcmIO/writer/module/DicomSRManager.hpp"
#include "gdcmIO/writer/instance/DicomSR.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomSRManager::DicomSRManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomSRManager::~DicomSRManager()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomSRManager::write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path)
{
    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    SLM_ASSERT("gdcmIO::DicomInstance not set",  this->getDicomInstance().get());

    //*****     Handle SR document      *****//
    ::gdcmIO::writer::instance::DicomSR   docSRWriter;
    docSRWriter.setObject( image );
    docSRWriter.setFile( a_path.string() + "/docSR" );

    //*****     Set instance and the referenced instances     *****//
    docSRWriter.setDicomInstance( this->getDicomInstance() );

    docSRWriter.setDataSet(a_gDs);
    docSRWriter.write();
}

} // namespace module
} // namespace writer
} // namespace gdcmIO
