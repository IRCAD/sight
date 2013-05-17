/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <string>

#include "gdcmIO/writer/module/DicomImageManager.hpp"
#include "gdcmIO/writer/instance/DicomImage.hpp"

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomImageManager::DicomImageManager()
{}

//------------------------------------------------------------------------------

DicomImageManager::~DicomImageManager()
{}

//------------------------------------------------------------------------------

void DicomImageManager::write(::gdcm::DataSet & a_gDs, const ::boost::filesystem::path & a_path)
{
    ::fwData::Image::sptr image = this->getConcreteObject();
    SLM_ASSERT("fwData::Image not instanced", image);

    ::boost::shared_ptr< ::gdcmIO::container::DicomInstance > dicomInstance   = this->getDicomInstance();
    SLM_ASSERT("gdcmIO::DicomInstance not set", dicomInstance);

    // Initialization of common variables
    ::boost::shared_ptr< ::gdcmIO::writer::instance::DicomImage > imageWriter =
            ::boost::make_shared< ::gdcmIO::writer::instance::DicomImage >();

    // Configure the image writer
    imageWriter->setObject(image);
    imageWriter->setFile(a_path); // The path will be completed by the image writer
    imageWriter->setDicomInstance(dicomInstance);
    imageWriter->setMultiFrame( dicomInstance->getIsMultiFrame() );

    // Write the image
    imageWriter->setDataSet(a_gDs);
    imageWriter->write();
}

} // namespace module
} // namespace writer
} // namespace gdcmIO
