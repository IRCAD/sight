/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2012.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <sstream>

#include <fwMedData/types.hpp>

#include "gdcmIO/writer/module/DicomSeries.hpp"
#include "gdcmIO/helper/GdcmHelper.hpp"

using std::stringstream;

namespace gdcmIO
{
namespace writer
{
namespace module
{

//------------------------------------------------------------------------------

DicomSeries::DicomSeries()
{}

//------------------------------------------------------------------------------

DicomSeries::~DicomSeries()
{}

//------------------------------------------------------------------------------

void DicomSeries::write(::gdcm::DataSet & a_gDs)
{
    ::fwMedData::Series::csptr series = this->getConcreteObject();
    SLM_ASSERT("fwMedData::Series not instanced", series);

    // Serie's instance UID
    helper::GdcmData::setTagValue<0x0020,0x000e>(series->getInstanceUID(), a_gDs); // Type 1

    // Series's modality
    helper::GdcmData::setTagValue<0x0008,0x0060>(series->getModality(), a_gDs); // Type 1

    // Serie's date
    helper::GdcmData::setTagValue<0x0008,0x0021>(series->getDate(), a_gDs); // Type 3

    // Serie's time
    helper::GdcmData::setTagValue<0x0008,0x0031>(series->getTime(), a_gDs); // Type 3

    // Serie's description
    helper::GdcmData::setTagValue<0x0008,0x103e>(series->getDescription(), a_gDs);

    // Serie's number
    //TODO
    helper::GdcmData::setTagValue<int,0x0020,0x0011>(0, a_gDs); // Type 2

    ::fwMedData::DicomValuesType performingPhysicians = series->getPerformingPhysiciansName();
    const unsigned int length = static_cast<unsigned int>( performingPhysicians.size() );
    if(length > 0)
    {
        ::gdcm::String<> *physicians = new ::gdcm::String<>[performingPhysicians.size()];
        unsigned int count = 0;
        BOOST_FOREACH(std::string physician, performingPhysicians)
        {
            physicians[count++] = ::gdcm::String<>(physician);
        }
        helper::GdcmData::setTagValues< ::gdcm::String<>, 0x0008, 0x1050 >(physicians, length, a_gDs);
    }
}

} // namespace module
} // namespace writer
} // namespace gdcmIO
