/************************************************************************
 *
 * Copyright (C) 2024 IRCAD France
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

// cspell:ignore asmt bdus ctprotocol ivoct ivus optbsv optenf rtdose rtimage rtintent rtrad rtrecord rtsegann rtstruct
// cspell:ignore rtplan texturemap xaprotocol

#pragma once

#include <sight/data/config.hpp>

#include <cstdint>
#include <optional>
#include <stdexcept>
#include <string_view>

namespace sight::data::dicom
{

/// Enum that defines modalities
enum class modality_t : std::uint8_t
{
    unknown = 0,
    ann,
    ar,
    asmt,
    au,
    bdus,
    bi,
    bmd,
    cfm,
    cr,
    ct,
    ctprotocol,
    dms,
    dg,
    doc,
    dx,
    ecg,
    eeg,
    emg,
    eog,
    eps,
    es,
    fid,
    gm,
    hc,
    hd,
    io,
    iol,
    ivoct,
    ivus,
    ker,
    ko,
    len,
    ls,
    mg,
    mr,
    m3d,
    nm,
    oam,
    oct,
    op,
    opm,
    opt,
    optbsv,
    optenf,
    opv,
    oss,
    ot,
    pa,
    plan,
    pos,
    pr,
    pt,
    px,
    reg,
    resp,
    rf,
    rg,
    rtdose,
    rtimage,
    rtintent,
    rtplan,
    rtrad,
    rtrecord,
    rtsegann,
    rtstruct,
    rwv,
    seg,
    sm,
    smr,
    sr,
    srf,
    stain,
    texturemap,
    tg,
    us,
    va,
    xa,
    xaprotocol,
    xc
};

/// Convenience function to convert from modality_t enum value to string
/// @param[in] _modality the modality_t enum value to convert
static constexpr std::optional<std::string_view> to_string(modality_t _modality)
{
    /// @see @link https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.7.3.html#sect_C.7.3.1.1.1
    switch(_modality)
    {
        case modality_t::ann:
            return "ANN";

        case modality_t::ar:
            return "AR";

        case modality_t::asmt:
            return "ASMT";

        case modality_t::au:
            return "AU";

        case modality_t::bdus:
            return "BDUS";

        case modality_t::bi:
            return "BI";

        case modality_t::bmd:
            return "BMD";

        case modality_t::cfm:
            return "CFM";

        case modality_t::cr:
            return "CR";

        case modality_t::ct:
            return "CT";

        case modality_t::ctprotocol:
            return "CTPROTOCOL";

        case modality_t::dms:
            return "DMS";

        case modality_t::dg:
            return "DG";

        case modality_t::doc:
            return "DOC";

        case modality_t::dx:
            return "DX";

        case modality_t::ecg:
            return "ECG";

        case modality_t::eeg:
            return "EEG";

        case modality_t::emg:
            return "EMG";

        case modality_t::eog:
            return "EOG";

        case modality_t::eps:
            return "EPS";

        case modality_t::es:
            return "ES";

        case modality_t::fid:
            return "FID";

        case modality_t::gm:
            return "GM";

        case modality_t::hc:
            return "HC";

        case modality_t::hd:
            return "HD";

        case modality_t::io:
            return "IO";

        case modality_t::iol:
            return "IOL";

        case modality_t::ivoct:
            return "IVOCT";

        case modality_t::ivus:
            return "IVUS";

        case modality_t::ker:
            return "KER";

        case modality_t::ko:
            return "KO";

        case modality_t::len:
            return "LEN";

        case modality_t::ls:
            return "LS";

        case modality_t::mg:
            return "MG";

        case modality_t::mr:
            return "MR";

        case modality_t::m3d:
            return "M3D";

        case modality_t::nm:
            return "NM";

        case modality_t::oam:
            return "OAM";

        case modality_t::oct:
            return "OCT";

        case modality_t::op:
            return "OP";

        case modality_t::opm:
            return "OPM";

        case modality_t::opt:
            return "OPT";

        case modality_t::optbsv:
            return "OPTBSV";

        case modality_t::optenf:
            return "OPTENF";

        case modality_t::opv:
            return "OPV";

        case modality_t::oss:
            return "OSS";

        case modality_t::ot:
            return "OT";

        case modality_t::pa:
            return "PA";

        case modality_t::plan:
            return "PLAN";

        case modality_t::pos:
            return "POS";

        case modality_t::pr:
            return "PR";

        case modality_t::pt:
            return "PT";

        case modality_t::px:
            return "PX";

        case modality_t::reg:
            return "REG";

        case modality_t::resp:
            return "RESP";

        case modality_t::rf:
            return "RF";

        case modality_t::rg:
            return "RG";

        case modality_t::rtdose:
            return "RTDOSE";

        case modality_t::rtimage:
            return "RTIMAGE";

        case modality_t::rtintent:
            return "RTINTENT";

        case modality_t::rtplan:
            return "RTPLAN";

        case modality_t::rtrad:
            return "RTRAD";

        case modality_t::rtrecord:
            return "RTRECORD";

        case modality_t::rtsegann:
            return "RTSEGANN";

        case modality_t::rtstruct:
            return "RTSTRUCT";

        case modality_t::rwv:
            return "RWV";

        case modality_t::seg:
            return "SEG";

        case modality_t::sm:
            return "SM";

        case modality_t::smr:
            return "SMR";

        case modality_t::sr:
            return "SR";

        case modality_t::srf:
            return "SRF";

        case modality_t::stain:
            return "STAIN";

        case modality_t::texturemap:
            return "TEXTUREMAP";

        case modality_t::tg:
            return "TG";

        case modality_t::us:
            return "US";

        case modality_t::va:
            return "VA";

        case modality_t::xa:
            return "XA";

        case modality_t::xaprotocol:
            return "XAPROTOCOL";

        case modality_t::xc:
            return "XC";

        case modality_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid modality");
    }
}

SIGHT_DATA_API modality_t to_modality(const std::string_view& _modality) noexcept;

enum class ultrasound_acquisition_geometry_t : std::uint8_t
{
    unknown = 0,
    apex,
    patient
};

//------------------------------------------------------------------------------

static constexpr std::optional<std::string_view> to_string(
    ultrasound_acquisition_geometry_t _ultrasound_acquisition_geometry
)
{
    // APEX: there exists an apex of the scan lines from which the volume data was acquired.
    // PATIENT: the ultrasound acquisition geometry is patient relative
    switch(_ultrasound_acquisition_geometry)
    {
        case ultrasound_acquisition_geometry_t::apex:
            return "APEX";

        case ultrasound_acquisition_geometry_t::patient:
            return "PATIENT";

        case ultrasound_acquisition_geometry_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid ultrasound acquisition geometry");
    }
}

//------------------------------------------------------------------------------

static constexpr ultrasound_acquisition_geometry_t to_ultrasound_acquisition_geometry(
    const std::string_view& _ultrasound_acquisition_geometry
) noexcept
{
    // APEX: there exists an apex of the scan lines from which the volume data was acquired.
    // PATIENT: the ultrasound acquisition geometry is patient relative
    if(constexpr auto apex = to_string(ultrasound_acquisition_geometry_t::apex);
       _ultrasound_acquisition_geometry == apex)
    {
        return ultrasound_acquisition_geometry_t::apex;
    }

    if(constexpr auto patient = to_string(ultrasound_acquisition_geometry_t::patient);
       _ultrasound_acquisition_geometry == patient)
    {
        return ultrasound_acquisition_geometry_t::patient;
    }

    return ultrasound_acquisition_geometry_t::unknown;
}

enum class patient_frame_of_reference_source_t : std::uint8_t
{
    unknown = 0,
    table,
    estimated,
    registration
};

//------------------------------------------------------------------------------

static constexpr std::optional<std::string_view> to_string(
    patient_frame_of_reference_source_t _patient_frame_of_reference_source
)
{
    // TABLE: A positioning device, such as a gantry, was used to generate these values.
    // ESTIMATED: Estimated patient position / orientation (e.g., estimated by the user), or if reliable
    //            information is not available.
    // REGISTRATION: Acquisition has been spatially registered to a prior image set.
    switch(_patient_frame_of_reference_source)
    {
        case patient_frame_of_reference_source_t::table:
            return "TABLE";

        case patient_frame_of_reference_source_t::estimated:
            return "ESTIMATED";

        case patient_frame_of_reference_source_t::registration:
            return "REGISTRATION";

        case patient_frame_of_reference_source_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid patient frame of reference source");
    }
}

//------------------------------------------------------------------------------

static constexpr patient_frame_of_reference_source_t to_patient_frame_of_reference_source(
    const std::string_view& _patient_frame_of_reference_source
) noexcept
{
    // TABLE: A positioning device, such as a gantry, was used to generate these values.
    // ESTIMATED: Estimated patient position / orientation (e.g., estimated by the user), or if reliable
    //            information is not available.
    // REGISTRATION: Acquisition has been spatially registered to a prior image set.
    if(constexpr auto table = to_string(patient_frame_of_reference_source_t::table);
       _patient_frame_of_reference_source == table)
    {
        return patient_frame_of_reference_source_t::table;
    }

    if(constexpr auto estimated = to_string(patient_frame_of_reference_source_t::estimated);
       _patient_frame_of_reference_source == estimated)
    {
        return patient_frame_of_reference_source_t::estimated;
    }

    if(constexpr auto registration = to_string(patient_frame_of_reference_source_t::registration);
       _patient_frame_of_reference_source == registration)
    {
        return patient_frame_of_reference_source_t::registration;
    }

    return patient_frame_of_reference_source_t::unknown;
}

/// Enum that defines the kind of multi-frame dimension we are
enum class dimension_organization_t : std::uint8_t
{
    unknown = 0,
    volume,          // "3D in DICOM",
    volume_temporal, // "3D_TEMPORAL in DICOM",
    tiled_full,
    tiled_sparse
};

//------------------------------------------------------------------------------

static constexpr std::optional<std::string_view> to_string(
    dimension_organization_t _dimension_organization_type
)
{
    // 3D: Spatial Multi-frame image of equally spaced parallel planes (3D volume set)
    // 3D_TEMPORAL: Temporal loop of equally spaced parallel-plane 3D volume sets.
    // TILED_FULL: Tiled image in which each frame represents a single tile and the positions of the tiles are
    //             implicitly defined as per Section C.7.6.17.3.
    // TILED_SPARSE: Tiled image in which each frame represents a single tile and the positions of tiles are
    //               explicitly defined by per-frame Functional Group Macro entries.

    switch(_dimension_organization_type)
    {
        case dimension_organization_t::volume:
            return "3D";

        case dimension_organization_t::volume_temporal:
            return "3D_TEMPORAL";

        case dimension_organization_t::tiled_full:
            return "TILED_FULL";

        case dimension_organization_t::tiled_sparse:
            return "TILED_SPARSE";

        case dimension_organization_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid dimension organization type");
    }
}

//------------------------------------------------------------------------------

static constexpr dimension_organization_t to_dimension_organization_type(
    const std::string_view& _dimension_organization_type
) noexcept
{
    // 3D: Spatial Multi-frame image of equally spaced parallel planes (3D volume set)
    // 3D_TEMPORAL: Temporal loop of equally spaced parallel-plane 3D volume sets.
    // TILED_FULL: Tiled image in which each frame represents a single tile and the positions of the tiles are
    //             implicitly defined as per Section C.7.6.17.3.
    // TILED_SPARSE: Tiled image in which each frame represents a single tile and the positions of tiles are
    //               explicitly defined by per-frame Functional Group Macro entries.
    if(constexpr auto volume = to_string(dimension_organization_t::volume);
       _dimension_organization_type == volume)
    {
        return dimension_organization_t::volume;
    }

    if(constexpr auto volume_temporal = to_string(dimension_organization_t::volume_temporal);
       _dimension_organization_type == volume_temporal)
    {
        return dimension_organization_t::volume_temporal;
    }

    if(constexpr auto tiled_full = to_string(dimension_organization_t::tiled_full);
       _dimension_organization_type == tiled_full)
    {
        return dimension_organization_t::volume_temporal;
    }

    if(constexpr auto tiled_sparse = to_string(dimension_organization_t::tiled_sparse);
       _dimension_organization_type == tiled_sparse)
    {
        return dimension_organization_t::tiled_sparse;
    }

    return dimension_organization_t::unknown;
}

/// Enum that defines the relationship between the volume and the transducer
enum class volume_to_transducer_relationship_t : std::uint8_t
{
    unknown = 0,
    fixed,
    position_var,
    orientation_var,
    variable
};

//------------------------------------------------------------------------------

static constexpr std::optional<std::string_view> to_string(
    volume_to_transducer_relationship_t _volume_to_transducer_relationship
)
{
    // FIXED: The transducer position and orientation relative to the volume is constant and specified by Volume to
    //        Transducer Mapping Matrix (0020,9309).
    // POSITION_VAR: The transducer position relative to the volume varies during acquisition and the position specified
    //               by Volume to Transducer Mapping Matrix (0020,9309) is a nominal value. The transducer orientation
    //               relative to the volume is constant and specified by Volume to Transducer Mapping Matrix
    //               (0020,9309).
    // ORIENTATION_VAR: The transducer orientation relative to the volume varies during acquisition and the orientation
    //                  specified by Volume to Transducer Mapping Matrix (0020,9309) is a nominal value. The transducer
    //                  position relative to the volume is constant and specified by Volume to Transducer Mapping Matrix
    //                  (0020,9309).
    // VARIABLE: The transducer position and orientation relative to the volume varies during acquisition and the
    //           position and orientation specified by Volume to Transducer Mapping Matrix (0020,9309) are nominal.

    switch(_volume_to_transducer_relationship)
    {
        case volume_to_transducer_relationship_t::fixed:
            return "FIXED";

        case volume_to_transducer_relationship_t::position_var:
            return "POSITION_VAR";

        case volume_to_transducer_relationship_t::orientation_var:
            return "ORIENTATION_VAR";

        case volume_to_transducer_relationship_t::variable:
            return "VARIABLE";

        case volume_to_transducer_relationship_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid volume to transducer relationship");
    }
}

//------------------------------------------------------------------------------

static constexpr volume_to_transducer_relationship_t to_volume_to_transducer_relationship(
    const std::string_view& _volume_to_transducer_relationship
) noexcept
{
    // FIXED: The transducer position and orientation relative to the volume is constant and specified by Volume to
    //        Transducer Mapping Matrix (0020,9309).
    // POSITION_VAR: The transducer position relative to the volume varies during acquisition and the position specified
    //               by Volume to Transducer Mapping Matrix (0020,9309) is a nominal value. The transducer orientation
    //               relative to the volume is constant and specified by Volume to Transducer Mapping Matrix
    //               (0020,9309).
    // ORIENTATION_VAR: The transducer orientation relative to the volume varies during acquisition and the orientation
    //                  specified by Volume to Transducer Mapping Matrix (0020,9309) is a nominal value. The transducer
    //                  position relative to the volume is constant and specified by Volume to Transducer Mapping Matrix
    //                  (0020,9309).
    // VARIABLE: The transducer position and orientation relative to the volume varies during acquisition and the
    //           position and orientation specified by Volume to Transducer Mapping Matrix (0020,9309) are nominal.
    if(constexpr auto fixed = to_string(volume_to_transducer_relationship_t::fixed);
       _volume_to_transducer_relationship == fixed)
    {
        return volume_to_transducer_relationship_t::fixed;
    }

    if(constexpr auto position_var = to_string(volume_to_transducer_relationship_t::position_var);
       _volume_to_transducer_relationship == position_var)
    {
        return volume_to_transducer_relationship_t::position_var;
    }

    if(constexpr auto orientation_var = to_string(volume_to_transducer_relationship_t::orientation_var);
       _volume_to_transducer_relationship == orientation_var)
    {
        return volume_to_transducer_relationship_t::orientation_var;
    }

    if(constexpr auto variable = to_string(volume_to_transducer_relationship_t::variable);
       _volume_to_transducer_relationship == variable)
    {
        return volume_to_transducer_relationship_t::variable;
    }

    return volume_to_transducer_relationship_t::unknown;
}

/// Enum that defines the position measuring device used
enum class position_measuring_device_used_t : std::uint8_t
{
    unknown = 0,
    rigid,
    tracked,
    freehand
};

//------------------------------------------------------------------------------

static constexpr std::optional<std::string_view> to_string(
    position_measuring_device_used_t _position_measuring_device_used
)
{
    // See: https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.8.24.3.html#table_C.8.24.3-1
    switch(_position_measuring_device_used)
    {
        case position_measuring_device_used_t::rigid:
            return "RIGID";

        case position_measuring_device_used_t::tracked:
            return "TRACKED";

        case position_measuring_device_used_t::freehand:
            return "FREEHAND";

        case position_measuring_device_used_t::unknown:
            return std::nullopt;

        default:
            throw std::invalid_argument("Invalid dimension organization type");
    }
}

//------------------------------------------------------------------------------

static constexpr std::optional<position_measuring_device_used_t> to_position_measuring_device_used(
    const std::string_view& _position_measuring_device_used
) noexcept
{
    // See: https://dicom.nema.org/medical/dicom/current/output/chtml/part03/sect_C.8.24.3.html#table_C.8.24.3-1
    if(_position_measuring_device_used == to_string(position_measuring_device_used_t::rigid))
    {
        return position_measuring_device_used_t::rigid;
    }

    if(_position_measuring_device_used == to_string(position_measuring_device_used_t::tracked))
    {
        return position_measuring_device_used_t::tracked;
    }

    if(_position_measuring_device_used == to_string(position_measuring_device_used_t::freehand))
    {
        return position_measuring_device_used_t::freehand;
    }

    return std::nullopt;
}

} // namespace sight::data::dicom
