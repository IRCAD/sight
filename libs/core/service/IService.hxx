/************************************************************************
 *
 * Copyright (C) 2009-2021 IRCAD France
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

template<class DATATYPE>
inline CSPTR(DATATYPE) IService::getInput(std::string_view key, size_t index) const
{
    CSPTR(DATATYPE) input;
    if(auto iterator = m_inputsMap.find({std::string(key), index}); iterator != m_inputsMap.end())
    {
        input = std::dynamic_pointer_cast<const DATATYPE>(iterator->second.getShared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", input);
    }

    return input;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline SPTR(DATATYPE) IService::getInOut(std::string_view key, size_t index) const
{
    SPTR(DATATYPE) inout;
    if(auto iterator = m_inOutsMap.find({std::string(key), index}); iterator != m_inOutsMap.end())
    {
        inout = std::dynamic_pointer_cast<DATATYPE>(iterator->second.getShared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", inout);
    }

    return inout;
}
//------------------------------------------------------------------------------

template<class DATATYPE>
inline SPTR(DATATYPE) IService::getOutput(std::string_view key, size_t index) const
{
    SPTR(DATATYPE) output;
    if(auto iterator = m_outputsMap.find({std::string(key), index}); iterator != m_outputsMap.end())
    {
        output = std::dynamic_pointer_cast<DATATYPE>(iterator->second.get_shared());
        SIGHT_ASSERT("Dynamic cast " << core::TypeDemangler<DATATYPE>().getClassname() << " failed", output);
    }

    return output;
}

//------------------------------------------------------------------------------

template<class DATATYPE, typename CONST_DATATYPE>
inline data::mt::weak_ptr<CONST_DATATYPE> IService::getWeakInput(std::string_view key, size_t index) const
{
    data::mt::weak_ptr<CONST_DATATYPE> input;
    input = this->getInput<CONST_DATATYPE>(key, index);
    return input;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> IService::getWeakInOut(std::string_view key, size_t index) const
{
    data::mt::weak_ptr<DATATYPE> inout;
    inout = this->getInOut<DATATYPE>(key, index);
    return inout;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::weak_ptr<DATATYPE> IService::getWeakOutput(std::string_view key, size_t index) const
{
    data::mt::weak_ptr<DATATYPE> out;
    out = this->getOutput<DATATYPE>(key, index);
    return out;
}

//------------------------------------------------------------------------------

template<class DATATYPE, typename CONST_DATATYPE>
inline data::mt::locked_ptr<CONST_DATATYPE> IService::getLockedInput(
    std::string_view key,
    size_t index
) const
{
    auto lockedInput = this->getWeakInput<DATATYPE>(key, index).lock();

    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Service with ID '"
            + this->getID()
            + "' cannot acquire a locked input with '"
            + std::string(key)
            + "'["
            + std::to_string(index)
            + "]."
        ),
        !lockedInput
    );

    return lockedInput;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::locked_ptr<DATATYPE> IService::getLockedInOut(std::string_view key, size_t index) const
{
    auto lockedInOut = this->getWeakInOut<DATATYPE>(key, index).lock();

    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Service with ID '"
            + this->getID()
            + "' cannot acquire a locked inout with '"
            + std::string(key)
            + "'["
            + std::to_string(index)
            + "]."
        ),
        !lockedInOut
    );

    return lockedInOut;
}

//------------------------------------------------------------------------------

template<class DATATYPE>
inline data::mt::locked_ptr<DATATYPE> IService::getLockedOutput(std::string_view key, size_t index) const
{
    auto lockedOutput = this->getWeakOutput<DATATYPE>(key, index).lock();

    SIGHT_THROW_EXCEPTION_IF(
        data::Exception(
            "Service with ID '"
            + this->getID()
            + "' cannot acquire a locked output with '"
            + std::string(key)
            + "'["
            + std::to_string(index)
            + "]."
        ),
        !lockedOutput
    );

    return lockedOutput;
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
