/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#include "Series.hpp"

#include <data/DicomSeries.hpp>
#include <data/ImageSeries.hpp>
#include <data/ModelSeries.hpp>

namespace sight::data::dicom
{

// ----------------------------------------------------------------------------

data::ImageSeries::sptr Series::convertToImageSeries(const data::DicomSeries::csptr& series)
{
    data::ImageSeries::sptr result = data::ImageSeries::New();

    // Copy values of patient, study & equipment, since it returns const pointer.
    data::Patient::sptr patient     = data::Patient::New();
    data::Study::sptr study         = data::Study::New();
    data::Equipment::sptr equipment = data::Equipment::New();

    patient->deepCopy(series->getPatient());
    study->deepCopy(series->getStudy());
    equipment->deepCopy(series->getEquipment());

    result->setPatient(patient);
    result->setStudy(study);
    result->setEquipment(equipment);
    result->setInstanceUID(series->getInstanceUID());
    result->setModality(series->getModality());
    result->setDate(series->getDate());
    result->setTime(series->getTime());
    result->setDescription(series->getDescription());
    result->setPerformingPhysiciansName(series->getPerformingPhysiciansName());
    return result;
}

// ----------------------------------------------------------------------------

data::ModelSeries::sptr Series::convertToModelSeries(const data::DicomSeries::csptr& series)
{
    data::ModelSeries::sptr result = data::ModelSeries::New();

    // Copy values of patient, study & equipment, since it returns const pointer.
    data::Patient::sptr patient     = data::Patient::New();
    data::Study::sptr study         = data::Study::New();
    data::Equipment::sptr equipment = data::Equipment::New();

    patient->deepCopy(series->getPatient());
    study->deepCopy(series->getStudy());
    equipment->deepCopy(series->getEquipment());

    result->setPatient(patient);
    result->setStudy(study);
    result->setEquipment(equipment);
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
