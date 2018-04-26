/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwMedDataCamp/DicomSeries.hpp"

#include <fwCamp/UserObject.hpp>

#include <fwMedData/DicomSeries.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwMedData)(DicomSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwMedData")
    .base< ::fwMedData::Series>()
    .property("dicom_availability", &::fwMedData::DicomSeries::m_dicomAvailability)
    .property("number_of_instances", &::fwMedData::DicomSeries::m_numberOfInstances)
    .property("local_dicom_paths", &::fwMedData::DicomSeries::m_localDicomPaths)
    .property("dicom_binaries", &::fwMedData::DicomSeries::m_dicomBinaries)
    .property("sop_class_uids", &::fwMedData::DicomSeries::m_SOPClassUIDs)
    .property("computed_tag_values", &::fwMedData::DicomSeries::m_computedTagValues)
    .property("first_instance_number", &::fwMedData::DicomSeries::m_firstInstanceNumber)
    ;
}

fwCampImplementEnumMacro((fwMedData)(DicomSeries)(DicomAvailability))
{
    builder
    .value("NONE",  ::fwMedData::DicomSeries::NONE)
    .value("PATHS",  ::fwMedData::DicomSeries::PATHS)
    .value("BINARIES",  ::fwMedData::DicomSeries::BINARIES)
    ;
}
