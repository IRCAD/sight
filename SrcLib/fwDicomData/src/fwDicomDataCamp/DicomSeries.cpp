/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include "fwDicomDataCamp/DicomSeries.hpp"

#include <fwDicomData/DicomSeries.hpp>
#include <fwCamp/UserObject.hpp>

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwDicomData)(DicomSeries))
{
    builder
    .tag("object_version", "1")
    .tag("lib_name", "fwDicomData")
    .base< ::fwMedData::Series>()
    .property("dicom_availability", &::fwDicomData::DicomSeries::m_dicomAvailability)
    .property("number_of_instances", &::fwDicomData::DicomSeries::m_numberOfInstances)
    .property("local_dicom_paths", &::fwDicomData::DicomSeries::m_localDicomPaths)
    .property("dicom_binaries", &::fwDicomData::DicomSeries::m_dicomBinaries)
    .property("sop_class_uids", &::fwDicomData::DicomSeries::m_SOPClassUIDs)
    .property("computed_tag_values", &::fwDicomData::DicomSeries::m_computedTagValues)
    .property("first_instance_number", &::fwDicomData::DicomSeries::m_firstInstanceNumber)
    ;
}


fwCampImplementEnumMacro((fwDicomData)(DicomSeries)(DICOM_AVAILABILITY))
{
    builder
    .value("NONE",  ::fwDicomData::DicomSeries::NONE)
    .value("PATHS",  ::fwDicomData::DicomSeries::PATHS)
    .value("BINARIES",  ::fwDicomData::DicomSeries::BINARIES)
    .value("BLOB",  ::fwDicomData::DicomSeries::BLOB)
    ;
}
