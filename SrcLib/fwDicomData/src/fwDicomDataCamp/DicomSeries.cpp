/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#include <fwDicomData/DicomSeries.hpp>
#include <fwCamp/UserObject.hpp>

#include "fwDicomDataCamp/DicomSeries.hpp"

//------------------------------------------------------------------------------

fwCampImplementDataMacro((fwDicomData)(DicomSeries))
{
    builder
        .tag("object_version", "1")
        .tag("lib_name", "fwDicomData")
        .base< ::fwMedData::Series>()
        .property("dicom_availability", &::fwDicomData::DicomSeries::m_attrDicomAvailability)
        .property("number_of_instances", &::fwDicomData::DicomSeries::m_attrNumberOfInstances)
        .property("local_dicom_paths", &::fwDicomData::DicomSeries::m_attrLocalDicomPaths)
        .property("dicom_binaries", &::fwDicomData::DicomSeries::m_attrDicomBinaries)
        .property("sop_class_uids", &::fwDicomData::DicomSeries::m_attrSOPClassUIDs)
        .property("computed_tag_values", &::fwDicomData::DicomSeries::m_attrComputedTagValues)
        ;
}


fwCampImplementEnumMacro((fwDicomData)(DicomSeries)(DICOM_AVAILABILITY))
{
    builder
        .value("NONE",  ::fwDicomData::DicomSeries::NONE)
        .value("PATHS",  ::fwDicomData::DicomSeries::PATHS)
        .value("BINARIES",  ::fwDicomData::DicomSeries::BINARIES)
        ;
}
