/************************************************************************
 *
 * Copyright (C) 2017 IRCAD France
 * Copyright (C) 2017 IHU Strasbourg
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

#include "fwGdcmIO/helper/SOPClass.hpp"

#include <boost/assign/list_of.hpp>

#include <map>
#include <string>
#include <utility>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

const SOPClass::SOPClassContainerType SOPClass::s_SOP_CLASS_LIST = ::boost::assign::map_list_of
                                                                   // Media Storage Directory Storage (DICOMDIR)
                                                                       ("1.2.840.10008.1.3.10",
                                                                       std::make_pair("Media Storage Directory Storage",
                                                                                      false))

                                                                   // Standard SOP Classes (see DICOM PS3.4 B.5) (Update
                                                                   // 2016c)
                                                                       ("1.2.840.10008.5.1.4.1.1.1",
                                                                       std::make_pair(
                                                                           "Computed Radiography Image Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.1",
                                                                       std::make_pair(
                                                                           "Digital X-Ray Image Storage - For Presentation",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.1.1",
                                                                       std::make_pair(
                                                                           "Digital X-Ray Image Storage - For Processing",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.2",
                                                                       std::make_pair(
                                                                           "Digital Mammography X-Ray Image Storage - For Presentation",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.2.1",
                                                                       std::make_pair(
                                                                           "Digital Mammography X-Ray Image Storage - For Processing",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.3",
                                                                       std::make_pair(
                                                                           "Digital Intra-Oral X-Ray Image Storage - For Presentation",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.1.3.1",
                                                                       std::make_pair(
                                                                           "Digital Intra-Oral X-Ray Image Storage - For Processing",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.2",
                                                                       std::make_pair("CT Image Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.2.1",
                                                                       std::make_pair("Enhanced CT Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.2.2",
                                                                       std::make_pair(
                                                                           "Legacy Converted Enhanced CT Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.3.1",
                                                                       std::make_pair(
                                                                           "Ultrasound Multi-frame Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.4",
                                                                       std::make_pair("MR Image Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.4.1",
                                                                       std::make_pair("Enhanced MR Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.4.2",
                                                                       std::make_pair("MR Spectroscopy Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.4.3",
                                                                       std::make_pair("Enhanced MR Color Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.4.4",
                                                                       std::make_pair(
                                                                           "Legacy Converted Enhanced MR Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.6.1",
                                                                       std::make_pair("Ultrasound Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.6.2",
                                                                       std::make_pair("Enhanced US Volume Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.7",
                                                                       std::make_pair("Secondary Capture Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.7.1",
                                                                       std::make_pair(
                                                                           "Multi-frame Single Bit Secondary Capture Image Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.7.2",
                                                                       std::make_pair(
                                                                           "Multi-frame Grayscale Byte Secondary Capture Image Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.7.3",
                                                                       std::make_pair(
                                                                           "Multi-frame Grayscale Word Secondary Capture Image Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.7.4",
                                                                       std::make_pair(
                                                                           "Multi-frame True Color Secondary Capture Image Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.1.1",
                                                                       std::make_pair("12-lead ECG Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.1.2",
                                                                       std::make_pair("General ECG Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.1.3",
                                                                       std::make_pair("Ambulatory ECG Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.2.1",
                                                                       std::make_pair("Hemodynamic Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.3.1",
                                                                       std::make_pair(
                                                                           "Cardiac Electrophysiology Waveform Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.4.1",
                                                                       std::make_pair(
                                                                           "Basic Voice Audio Waveform Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.4.2",
                                                                       std::make_pair("General Audio Waveform Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.5.1",
                                                                       std::make_pair("Arterial Pulse Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.9.6.1",
                                                                       std::make_pair("Respiratory Waveform Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.1",
                                                                       std::make_pair(
                                                                           "Grayscale Softcopy Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.2",
                                                                       std::make_pair(
                                                                           "Color Softcopy Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.3",
                                                                       std::make_pair(
                                                                           "Pseudo-Color Softcopy Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.4",
                                                                       std::make_pair(
                                                                           "Blending Softcopy Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.5",
                                                                       std::make_pair(
                                                                           "XA/XRF Grayscale Softcopy Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.6",
                                                                       std::make_pair(
                                                                           "Grayscale Planar MPR Volumetric Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.11.7",
                                                                       std::make_pair(
                                                                           "Compositing Planar MPR Volumetric Presentation State Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.12.1",
                                                                       std::make_pair("X-Ray Angiographic Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.12.1.1",
                                                                       std::make_pair("Enhanced XA Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.12.2",
                                                                       std::make_pair(
                                                                           "X-Ray Radiofluoroscopic Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.12.2.1",
                                                                       std::make_pair("Enhanced XRF Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.13.1.1",
                                                                       std::make_pair(
                                                                           "X-Ray 3D Angiographic Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.13.1.2",
                                                                       std::make_pair(
                                                                           "X-Ray 3D Craniofacial Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.13.1.3",
                                                                       std::make_pair(
                                                                           "Breast Tomosynthesis Image Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.13.1.4",
                                                                       std::make_pair(
                                                                           "Breast Projection X-Ray Image Storage - For Presentation",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.13.1.5",
                                                                       std::make_pair(
                                                                           "Breast Projection X-Ray Image Storage - For Processing",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.14.1",
                                                                       std::make_pair(
                                                                           "Intravascular Optical Coherence Tomography Image Storage - For Presentation",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.14.2",
                                                                       std::make_pair(
                                                                           "Intravascular Optical Coherence Tomography Image Storage - For Processing",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.20",
                                                                       std::make_pair("Nuclear Medicine Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.30",
                                                                       std::make_pair("Parametric Map Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66",
                                                                       std::make_pair("Raw Data Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.1",
                                                                       std::make_pair("Spatial Registration Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.2",
                                                                       std::make_pair("Spatial Fiducials Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.3",
                                                                       std::make_pair(
                                                                           "Deformable Spatial Registration Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.4",
                                                                       std::make_pair("Segmentation Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.5",
                                                                       std::make_pair("Surface Segmentation Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.66.6",
                                                                       std::make_pair("Tractography Results Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.67",
                                                                       std::make_pair("Real World Value Mapping Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.68.1",
                                                                       std::make_pair("Surface Scan Mesh Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.68.2",
                                                                       std::make_pair("Surface Scan Point Cloud Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.1",
                                                                       std::make_pair("VL Endoscopic Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.1.1",
                                                                       std::make_pair("Video Endoscopic Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.2",
                                                                       std::make_pair("VL Microscopic Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.2.1",
                                                                       std::make_pair("Video Microscopic Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.3",
                                                                       std::make_pair(
                                                                           "VL Slide-Coordinates Microscopic Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.4",
                                                                       std::make_pair("VL Photographic Image Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.4.1",
                                                                       std::make_pair("Video Photographic Image Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.1",
                                                                       std::make_pair(
                                                                           "Ophthalmic Photography 8 Bit Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.2",
                                                                       std::make_pair(
                                                                           "Ophthalmic Photography 16 Bit Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.3",
                                                                       std::make_pair(
                                                                           "Stereometric Relationship Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.4",
                                                                       std::make_pair(
                                                                           "Ophthalmic Tomography Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.5",
                                                                       std::make_pair(
                                                                           "Wide Field Ophthalmic Photography Stereographic Projection Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.5.6",
                                                                       std::make_pair(
                                                                           "Wide Field Ophthalmic Photography 3D Coordinates Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.77.1.6",
                                                                       std::make_pair(
                                                                           "VL Whole Slide Microscopy Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.1",
                                                                       std::make_pair("Lensometry Measurements Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.2",
                                                                       std::make_pair(
                                                                           "Autorefraction Measurements Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.3",
                                                                       std::make_pair("Keratometry Measurements Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.4",
                                                                       std::make_pair(
                                                                           "Subjective Refraction Measurements Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.5",
                                                                       std::make_pair(
                                                                           "Visual Acuity Storage Measurements Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.6",
                                                                       std::make_pair(
                                                                           "Spectacle Prescription Report Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.7",
                                                                       std::make_pair(
                                                                           "Ophthalmic Axial Measurements Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.78.8",
                                                                       std::make_pair(
                                                                           "Intraocular Lens Calculations Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.79.1",
                                                                       std::make_pair(
                                                                           "Macular Grid Thickness and Volume Report",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.80.1",
                                                                       std::make_pair(
                                                                           "Ophthalmic Visual Field Static Perimetry Measurements Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.81.1",
                                                                       std::make_pair("Ophthalmic Thickness Map Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.82.1",
                                                                       std::make_pair("Corneal Topography Map Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.11",
                                                                       std::make_pair("Basic Text SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.22",
                                                                       std::make_pair("Enhanced SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.33",
                                                                       std::make_pair("Comprehensive SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.34",
                                                                       std::make_pair("Comprehensive 3D SR Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.35",
                                                                       std::make_pair("Extensible SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.40",
                                                                       std::make_pair("Procedure Log Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.50",
                                                                       std::make_pair("Mammography CAD SR Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.59",
                                                                       std::make_pair(
                                                                           "Key Object Selection Document Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.65",
                                                                       std::make_pair("Chest CAD SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.67",
                                                                       std::make_pair("X-Ray Radiation Dose SR Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.68",
                                                                       std::make_pair(
                                                                           "Radiopharmaceutical Radiation Dose SR Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.69",
                                                                       std::make_pair("Colon CAD SR Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.70",
                                                                       std::make_pair(
                                                                           "Implantation Plan SR Document Storage",
                                                                           true))
                                                                       ("1.2.840.10008.5.1.4.1.1.88.71",
                                                                       std::make_pair("Acquisition Context SR Storage",
                                                                                      true))
                                                                       ("1.2.840.10008.5.1.4.1.1.90.1",
                                                                       std::make_pair(
                                                                           "Content Assessment Results Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.104.1",
                                                                       std::make_pair("Encapsulated PDF Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.104.2",
                                                                       std::make_pair("Encapsulated CDA Storage", true))
                                                                       ("1.2.840.10008.5.1.4.1.1.128",
                                                                       std::make_pair(
                                                                           "Positron Emission Tomography Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.130",
                                                                       std::make_pair("Enhanced PET Image Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.128.1",
                                                                       std::make_pair(
                                                                           "Legacy Converted Enhanced PET Image Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.131",
                                                                       std::make_pair("Basic Structured Display Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.1",
                                                                       std::make_pair("RT Image Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.2",
                                                                       std::make_pair("RT Dose Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.3",
                                                                       std::make_pair("RT Structure Set Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.4",
                                                                       std::make_pair(
                                                                           "RT Beams Treatment Record Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.5",
                                                                       std::make_pair("RT Plan Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.6",
                                                                       std::make_pair(
                                                                           "RT Brachy Treatment Record Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.7",
                                                                       std::make_pair(
                                                                           "RT Treatment Summary Record Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.8",
                                                                       std::make_pair("RT Ion Plan Storage", false))
                                                                       ("1.2.840.10008.5.1.4.1.1.481.9",
                                                                       std::make_pair(
                                                                           "RT Ion Beams Treatment Record Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.34.7",
                                                                       std::make_pair(
                                                                           "RT Beams Delivery Instruction Storage",
                                                                           false))
                                                                       ("1.2.840.10008.5.1.4.34.10",
                                                                       std::make_pair(
                                                                           "RT Brachy Application Setup Delivery Instruction Storage",
                                                                           false))

                                                                   // Non-Patient Object Storage Service Classes
                                                                       ("1.2.840.10008.5.1.4.38.1",
                                                                       std::make_pair("Hanging Protocol Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.39.1",
                                                                       std::make_pair("Color Palette Storage", false))
                                                                       ("1.2.840.10008.5.1.4.43.1",
                                                                       std::make_pair("Generic Implant Template Storage",
                                                                                      false))
                                                                       ("1.2.840.10008.5.1.4.44.1",
                                                                       std::make_pair(
                                                                           "Implant Assembly Template Storage", false))
                                                                       ("1.2.840.10008.5.1.4.45.1",
                                                                       std::make_pair("Implant Template Group Storage",
                                                                                      false))

;

//------------------------------------------------------------------------------

std::string SOPClass::getSOPClassName(const std::string& SOPClassUID)
{
    auto it = s_SOP_CLASS_LIST.find(SOPClassUID);
    return it != s_SOP_CLASS_LIST.end() ? it->second.first : SOPClassUID;
}

//------------------------------------------------------------------------------

bool SOPClass::isHazardousSOPClass(const std::string& SOPClassUID)
{
    auto it = s_SOP_CLASS_LIST.find(SOPClassUID);
    return it != s_SOP_CLASS_LIST.end() ? it->second.second : true;
}

//------------------------------------------------------------------------------

} // namespace helper
} // namespace fwGdcmIO

