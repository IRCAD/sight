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

#include "data/dicom/coded_string.hpp"

#include <core/exceptionmacros.hpp>

#include <unordered_map>

namespace sight::data::dicom
{

//------------------------------------------------------------------------------

modality_t to_modality(const std::string_view& _modality) noexcept
{
    static const std::unordered_map<std::string_view, modality_t> s_MODALITY {
        {*to_string(modality_t::ann), modality_t::ann},
        {*to_string(modality_t::ar), modality_t::ar},
        {*to_string(modality_t::asmt), modality_t::asmt},
        {*to_string(modality_t::au), modality_t::au},
        {*to_string(modality_t::bdus), modality_t::bdus},
        {*to_string(modality_t::bi), modality_t::bi},
        {*to_string(modality_t::bmd), modality_t::bmd},
        {*to_string(modality_t::cfm), modality_t::cfm},
        {*to_string(modality_t::cr), modality_t::cr},
        {*to_string(modality_t::ct), modality_t::ct},
        {*to_string(modality_t::ctprotocol), modality_t::ctprotocol},
        {*to_string(modality_t::dms), modality_t::dms},
        {*to_string(modality_t::dg), modality_t::dg},
        {*to_string(modality_t::doc), modality_t::doc},
        {*to_string(modality_t::dx), modality_t::dx},
        {*to_string(modality_t::ecg), modality_t::ecg},
        {*to_string(modality_t::eeg), modality_t::eeg},
        {*to_string(modality_t::emg), modality_t::emg},
        {*to_string(modality_t::eog), modality_t::eog},
        {*to_string(modality_t::eps), modality_t::eps},
        {*to_string(modality_t::es), modality_t::es},
        {*to_string(modality_t::fid), modality_t::fid},
        {*to_string(modality_t::gm), modality_t::gm},
        {*to_string(modality_t::hc), modality_t::hc},
        {*to_string(modality_t::hd), modality_t::hd},
        {*to_string(modality_t::io), modality_t::io},
        {*to_string(modality_t::iol), modality_t::iol},
        {*to_string(modality_t::ivoct), modality_t::ivoct},
        {*to_string(modality_t::ivus), modality_t::ivus},
        {*to_string(modality_t::ker), modality_t::ker},
        {*to_string(modality_t::ko), modality_t::ko},
        {*to_string(modality_t::len), modality_t::len},
        {*to_string(modality_t::ls), modality_t::ls},
        {*to_string(modality_t::mg), modality_t::mg},
        {*to_string(modality_t::mr), modality_t::mr},
        {*to_string(modality_t::m3d), modality_t::m3d},
        {*to_string(modality_t::nm), modality_t::nm},
        {*to_string(modality_t::oam), modality_t::oam},
        {*to_string(modality_t::oct), modality_t::oct},
        {*to_string(modality_t::op), modality_t::op},
        {*to_string(modality_t::opm), modality_t::opm},
        {*to_string(modality_t::opt), modality_t::opt},
        {*to_string(modality_t::optbsv), modality_t::optbsv},
        {*to_string(modality_t::optenf), modality_t::optenf},
        {*to_string(modality_t::opv), modality_t::opv},
        {*to_string(modality_t::oss), modality_t::oss},
        {*to_string(modality_t::ot), modality_t::ot},
        {*to_string(modality_t::pa), modality_t::pa},
        {*to_string(modality_t::plan), modality_t::plan},
        {*to_string(modality_t::pos), modality_t::pos},
        {*to_string(modality_t::pr), modality_t::pr},
        {*to_string(modality_t::pt), modality_t::pt},
        {*to_string(modality_t::px), modality_t::px},
        {*to_string(modality_t::reg), modality_t::reg},
        {*to_string(modality_t::resp), modality_t::resp},
        {*to_string(modality_t::rf), modality_t::rf},
        {*to_string(modality_t::rg), modality_t::rg},
        {*to_string(modality_t::rtdose), modality_t::rtdose},
        {*to_string(modality_t::rtimage), modality_t::rtimage},
        {*to_string(modality_t::rtintent), modality_t::rtintent},
        {*to_string(modality_t::rtplan), modality_t::rtplan},
        {*to_string(modality_t::rtrad), modality_t::rtrad},
        {*to_string(modality_t::rtrecord), modality_t::rtrecord},
        {*to_string(modality_t::rtsegann), modality_t::rtsegann},
        {*to_string(modality_t::rtstruct), modality_t::rtstruct},
        {*to_string(modality_t::rwv), modality_t::rwv},
        {*to_string(modality_t::seg), modality_t::seg},
        {*to_string(modality_t::sm), modality_t::sm},
        {*to_string(modality_t::smr), modality_t::smr},
        {*to_string(modality_t::sr), modality_t::sr},
        {*to_string(modality_t::srf), modality_t::srf},
        {*to_string(modality_t::stain), modality_t::stain},
        {*to_string(modality_t::texturemap), modality_t::texturemap},
        {*to_string(modality_t::tg), modality_t::tg},
        {*to_string(modality_t::us), modality_t::us},
        {*to_string(modality_t::va), modality_t::va},
        {*to_string(modality_t::xa), modality_t::xa},
        {*to_string(modality_t::xaprotocol), modality_t::xaprotocol},
        {*to_string(modality_t::xc), modality_t::xc},
    };

    if(const auto& it = s_MODALITY.find(_modality); it != s_MODALITY.cend())
    {
        return it->second;
    }

    return modality_t::unknown;
}

} // namespace sight::data::dicom
