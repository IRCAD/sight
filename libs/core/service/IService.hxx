/************************************************************************
 *
 * Copyright (C) 2009-2022 IRCAD France
 * Copyright (C) 2012-2020 IHU Strasbourg
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

#pragma once

#include "service/IService.hpp"

namespace sight::service
{

//------------------------------------------------------------------------------

template<class DATATYPE, typename CONST_DATATYPE>
inline data::mt::weak_ptr<CONST_DATATYPE> IService::getInput(std::string_view key, std::size_t index) const
{
    data::mt::weak_ptr<CONST_DATATYPE> input;

    if(auto iterator = m_inputsMap.find({std::string(key), index}); iterator != m_inputsMap.end())
    {
        auto shared_input = std::dynamic_pointer_cast<const DATATYPE>(iterator->second.getShared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", shared_input);
        input = shared_input;
    }

    return input;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> IService::getInOut(std::string_view key, std::size_t index) const
{
    data::mt::weak_ptr<DATATYPE> inout;

    if(auto iterator = m_inOutsMap.find({std::string(key), index}); iterator != m_inOutsMap.end())
    {
        auto shared_inout = std::dynamic_pointer_cast<DATATYPE>(iterator->second.getShared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", shared_inout);
        inout = shared_inout;
    }

    return inout;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> IService::getOutput(std::string_view key, std::size_t index) const
{
    data::mt::weak_ptr<DATATYPE> out;

    if(auto iterator = m_outputsMap.find({std::string(key), index}); iterator != m_outputsMap.end())
    {
        auto shared_out = std::dynamic_pointer_cast<DATATYPE>(iterator->second.get_shared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", shared_out);
        out = shared_out;
    }

    return out;
}

//------------------------------------------------------------------------------

inline const IService::InputMapType& IService::getInputs() const
{
    return m_inputsMap;
}

//------------------------------------------------------------------------------

inline const IService::InOutMapType& IService::getInOuts() const
{
    return m_inOutsMap;
}

//------------------------------------------------------------------------------

inline const IService::OutputMapType& IService::getOutputs() const
{
    return m_outputsMap;
}

//------------------------------------------------------------------------------

} // namespace sight::service
