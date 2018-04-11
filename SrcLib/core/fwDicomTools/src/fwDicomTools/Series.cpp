/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomTools/Series.hpp"

#include <fwMedData/DicomSeries.hpp>
#include <fwMedData/ImageSeries.hpp>
#include <fwMedData/ModelSeries.hpp>

namespace fwDicomTools
{

// ----------------------------------------------------------------------------

::fwMedData::ImageSeries::sptr Series::convertToImageSeries(const ::fwMedData::DicomSeries::csptr& series)
{
    ::fwMedData::ImageSeries::sptr result = ::fwMedData::ImageSeries::New();
    result->setPatient(series->getPatient());
    result->setStudy(series->getStudy());
    result->setEquipment(series->getEquipment());
    result->setInstanceUID(series->getInstanceUID());
    result->setModality(series->getModality());
    result->setDate(series->getDate());
    result->setTime(series->getTime());
    result->setDescription(series->getDescription());
    result->setPerformingPhysiciansName(series->getPerformingPhysiciansName());
    return result;
}

// ----------------------------------------------------------------------------

::fwMedData::ModelSeries::sptr Series::convertToModelSeries(const ::fwMedData::DicomSeries::csptr& series)
{
    ::fwMedData::ModelSeries::sptr result = ::fwMedData::ModelSeries::New();
    result->setPatient(series->getPatient());
    result->setStudy(series->getStudy());
    result->setEquipment(series->getEquipment());
    result->setInstanceUID(series->getInstanceUID());
    result->setModality(series->getModality());
    result->setDate(series->getDate());
    result->setTime(series->getTime());
    result->setDescription(series->getDescription());
    result->setPerformingPhysiciansName(series->getPerformingPhysiciansName());
    return result;
}

// ----------------------------------------------------------------------------

} //fwDicomTools
