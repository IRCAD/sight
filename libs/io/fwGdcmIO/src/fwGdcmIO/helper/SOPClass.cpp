/************************************************************************
 *
 * Copyright (C) 2017-2019 IRCAD France
 * Copyright (C) 2017-2019 IHU Strasbourg
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

#include <map>
#include <string>
#include <utility>

namespace fwGdcmIO
{
namespace helper
{

//------------------------------------------------------------------------------

const SOPClass::SOPClassContainerType SOPClass::s_SOP_CLASS_LIST = {

    // Media Storage Directory Storage (DICOMDIR)
    { "1.2.840.10008.1.3.10", { "Media Storage Directory Storage", false} },

    // Standard SOP Classes{ see DICOM PS3.4 B.5){ Update
    // 2016c)
    { "1.2.840.10008.5.1.4.1.1.1", { "Computed Radiography Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.1.1", { "Digital X-Ray Image Storage - For Presentation", false } },
    { "1.2.840.10008.5.1.4.1.1.1.1.1", { "Digital X-Ray Image Storage - For Processing", false } },
    { "1.2.840.10008.5.1.4.1.1.1.2", { "Digital Mammography X-Ray Image Storage - For Presentation", false } },
    { "1.2.840.10008.5.1.4.1.1.1.2.1", { "Digital Mammography X-Ray Image Storage - For Processing", false } },
    { "1.2.840.10008.5.1.4.1.1.1.3", { "Digital Intra-Oral X-Ray Image Storage - For Presentation", false } },
    { "1.2.840.10008.5.1.4.1.1.1.3.1", { "Digital Intra-Oral X-Ray Image Storage - For Processing", false } },
    { "1.2.840.10008.5.1.4.1.1.2", { "CT Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.2.1", { "Enhanced CT Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.2.2", { "Legacy Converted Enhanced CT Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.3.1", { "Ultrasound Multi-frame Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.4", { "MR Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.4.1", { "Enhanced MR Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.4.2", { "MR Spectroscopy Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.4.3", { "Enhanced MR Color Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.4.4", { "Legacy Converted Enhanced MR Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.6.1", { "Ultrasound Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.6.2", { "Enhanced US Volume Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.7", { "Secondary Capture Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.7.1", { "Multi-frame Single Bit Secondary Capture Image Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.7.2", { "Multi-frame Grayscale Byte Secondary Capture Image Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.7.3", { "Multi-frame Grayscale Word Secondary Capture Image Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.7.4", { "Multi-frame True Color Secondary Capture Image Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.9.1.1", { "12-lead ECG Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.9.1.2", { "General ECG Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.9.1.3", { "Ambulatory ECG Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.9.2.1", { "Hemodynamic Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.9.3.1", { "Cardiac Electrophysiology Waveform Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.9.4.1", { "Basic Voice Audio Waveform Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.9.4.2", { "General Audio Waveform Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.9.5.1", { "Arterial Pulse Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.9.6.1", { "Respiratory Waveform Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.11.1", { "Grayscale Softcopy Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.2", { "Color Softcopy Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.3", { "Pseudo-Color Softcopy Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.4", { "Blending Softcopy Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.5", { "XA/XRF Grayscale Softcopy Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.6", { "Grayscale Planar MPR Volumetric Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.11.7", { "Compositing Planar MPR Volumetric Presentation State Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.12.1", { "X-Ray Angiographic Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.12.1.1", { "Enhanced XA Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.12.2", { "X-Ray Radiofluoroscopic Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.12.2.1", { "Enhanced XRF Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.13.1.1", { "X-Ray 3D Angiographic Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.13.1.2", { "X-Ray 3D Craniofacial Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.13.1.3", { "Breast Tomosynthesis Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.13.1.4", { "Breast Projection X-Ray Image Storage - For Presentation", false } },
    { "1.2.840.10008.5.1.4.1.1.13.1.5", { "Breast Projection X-Ray Image Storage - For Processing", false } },
    { "1.2.840.10008.5.1.4.1.1.14.1",
      { "Intravascular Optical Coherence Tomography Image Storage - For Presentation", false } },
    { "1.2.840.10008.5.1.4.1.1.14.2",
      { "Intravascular Optical Coherence Tomography Image Storage - For Processing", false } },
    { "1.2.840.10008.5.1.4.1.1.20", { "Nuclear Medicine Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.30", { "Parametric Map Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.66", { "Raw Data Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.66.1", { "Spatial Registration Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.66.2", { "Spatial Fiducials Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.66.3", { "Deformable Spatial Registration Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.66.4", { "Segmentation Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.66.5", { "Surface Segmentation Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.66.6", { "Tractography Results Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.67", { "Real World Value Mapping Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.68.1", { "Surface Scan Mesh Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.68.2", { "Surface Scan Point Cloud Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.77.1.1", { "VL Endoscopic Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.77.1.1.1", { "Video Endoscopic Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.77.1.2", { "VL Microscopic Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.77.1.2.1", { "Video Microscopic Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.77.1.3", { "VL Slide-Coordinates Microscopic Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.4", { "VL Photographic Image Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.77.1.4.1", { "Video Photographic Image Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.1", { "Ophthalmic Photography 8 Bit Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.2", { "Ophthalmic Photography 16 Bit Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.3", { "Stereometric Relationship Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.4", { "Ophthalmic Tomography Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.5",
      { "Wide Field Ophthalmic Photography Stereographic Projection Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.5.6", { "Wide Field Ophthalmic Photography 3D Coordinates Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.77.1.6", { "VL Whole Slide Microscopy Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.78.1", { "Lensometry Measurements Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.78.2", { "Autorefraction Measurements Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.78.3", { "Keratometry Measurements Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.78.4", { "Subjective Refraction Measurements Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.78.5", { "Visual Acuity Storage Measurements Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.78.6", { "Spectacle Prescription Report Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.78.7", { "Ophthalmic Axial Measurements Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.78.8", { "Intraocular Lens Calculations Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.79.1", { "Macular Grid Thickness and Volume Report", true } },
    { "1.2.840.10008.5.1.4.1.1.80.1", { "Ophthalmic Visual Field Static Perimetry Measurements Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.81.1", { "Ophthalmic Thickness Map Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.82.1", { "Corneal Topography Map Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.88.11", { "Basic Text SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.22", { "Enhanced SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.33", { "Comprehensive SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.34", { "Comprehensive 3D SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.35", { "Extensible SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.40", { "Procedure Log Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.50", { "Mammography CAD SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.59", { "Key Object Selection Document Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.88.65", { "Chest CAD SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.67", { "X-Ray Radiation Dose SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.68", { "Radiopharmaceutical Radiation Dose SR Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.88.69", { "Colon CAD SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.88.70", { "Implantation Plan SR Document Storage", true } },
    { "1.2.840.10008.5.1.4.1.1.88.71", { "Acquisition Context SR Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.90.1", { "Content Assessment Results Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.104.1", { "Encapsulated PDF Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.104.2", { "Encapsulated CDA Storage", true} },
    { "1.2.840.10008.5.1.4.1.1.128", { "Positron Emission Tomography Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.130", { "Enhanced PET Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.128.1", { "Legacy Converted Enhanced PET Image Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.131", { "Basic Structured Display Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.1", { "RT Image Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.2", { "RT Dose Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.3", { "RT Structure Set Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.4", { "RT Beams Treatment Record Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.481.5", { "RT Plan Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.6", { "RT Brachy Treatment Record Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.481.7", { "RT Treatment Summary Record Storage", false } },
    { "1.2.840.10008.5.1.4.1.1.481.8", { "RT Ion Plan Storage", false} },
    { "1.2.840.10008.5.1.4.1.1.481.9", { "RT Ion Beams Treatment Record Storage", false } },
    { "1.2.840.10008.5.1.4.34.7", { "RT Beams Delivery Instruction Storage", false } },
    { "1.2.840.10008.5.1.4.34.10", { "RT Brachy Application Setup Delivery Instruction Storage", false } },

    // Non-Patient Object Storage Service Classes
    { "1.2.840.10008.5.1.4.38.1", { "Hanging Protocol Storage", false} },
    { "1.2.840.10008.5.1.4.39.1", { "Color Palette Storage", false} },
    { "1.2.840.10008.5.1.4.43.1", { "Generic Implant Template Storage", false} },
    { "1.2.840.10008.5.1.4.44.1", { "Implant Assembly Template Storage", false } },
    { "1.2.840.10008.5.1.4.45.1", { "Implant Template Group Storage", false} }
};

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
