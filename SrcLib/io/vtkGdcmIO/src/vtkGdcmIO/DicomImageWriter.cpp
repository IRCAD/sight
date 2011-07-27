/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2010.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <boost/filesystem.hpp>
#include <boost/bind.hpp>
#include <boost/lambda/lambda.hpp>

#include <fwTools/ClassRegistrar.hpp>

#include <fwCore/base.hpp>

#include "vtkGdcmIO/DicomImageWriter.hpp"
#include "vtkGdcmIO/DicomPatientWriter.hpp"

REGISTER_BINDING_BYCLASSNAME( ::fwDataIO::writer::IObjectWriter , ::vtkGdcmIO::DicomImageWriter, ::vtkGdcmIO::DicomImageWriter );


namespace vtkGdcmIO
{
//------------------------------------------------------------------------------

DicomImageWriter::DicomImageWriter()
: ::fwData::location::enableFolder< ::fwDataIO::writer::IObjectWriter >(this)
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

DicomImageWriter::~DicomImageWriter()
{
    SLM_TRACE_FUNC();
}

//------------------------------------------------------------------------------

void DicomImageWriter::write()
{
    assert( m_object.use_count() );
    assert( !m_object.expired() );
    assert( m_object.lock() );

    std::string imagePath =  this->getFolder().string();
    ::fwData::Image::sptr pImage = getConcreteObject();
    SLM_ASSERT("pImage not instanced", pImage);

    ::fwData::Patient::NewSptr pPatient;
    pPatient->setCRefName("anonymous");
    pPatient->setCRefFirstname("anonymous");

    ::fwData::Study::NewSptr pStudy;
    pStudy->setCRefHospital("unknown");
    pStudy->setCRefModality("CT");
    pPatient->addStudy(pStudy);
    ::fwData::Acquisition::NewSptr pAcquisition;
    pStudy->addAcquisition(pAcquisition);
    pAcquisition->setImage(pImage);

    ::vtkGdcmIO::DicomPatientWriter::NewSptr myWriter;
    myWriter->setObject(pPatient);
    ::fwData::location::Folder::NewSptr loc;
    loc->setFolder(imagePath);
    myWriter->setLocation(loc);
    // forward event progress to its parents
    ::fwTools::ProgressAdviser::ProgessHandler handler = ::boost::bind( &DicomImageWriter::notifyProgress, this, ::boost::lambda::_1, ::boost::lambda::_2);
    myWriter->addHandler ( handler );

    myWriter->write();
}

//------------------------------------------------------------------------------
std::string  DicomImageWriter::extension()
{
    return "";
}

} // namespace dicomIO
