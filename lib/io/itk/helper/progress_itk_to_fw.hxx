/************************************************************************
 *
 * Copyright (C) 2009-2023 IRCAD France
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

// cspell:ignore NOLINT

#pragma once

#include <core/base.hpp>

#include <itkCommand.h>
#include <itkEventObject.h>
#include <itkLightProcessObject.h>
#include <itkSmartPointer.h>

#include <limits>
#include <sstream>

namespace sight::io::itk
{

class local_command : public ::itk::Command
{
public:

    local_command()
    = default;

    using self_t  = local_command;
    using Pointer = ::itk::SmartPointer<local_command>;
    itkNewMacro(self_t);

    //------------------------------------------------------------------------------

    void Execute(const ::itk::Object* _caller, const ::itk::EventObject& /*event*/) override
    {
        const auto* po = dynamic_cast<const ::itk::LightProcessObject*>(_caller);
        if(po == nullptr)
        {
            return;
        }

        float percent = po->GetProgress();
        m_adviser->notify_progress(percent, m_msg);
    }

    //------------------------------------------------------------------------------

    void Execute(::itk::Object* _caller, const ::itk::EventObject& _event) override
    {
        const ::itk::Object* const_caller = _caller;
        Execute(const_caller, _event);
    }

    std::string m_msg;
    SPTR(core::tools::progress_adviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
progress_itk_to_fw<OBSERVEE>::progress_itk_to_fw(
    OBSERVEE observee,
    SPTR(core::tools::progress_adviser)observer,
    std::string msg
) :
    m_observee(observee),
    m_obs_tag(std::numeric_limits<std::uint64_t>::max())
{
    typename local_command::Pointer itk_call_back;
    itk_call_back            = local_command::New();
    itk_call_back->m_msg     = msg;
    itk_call_back->m_adviser = observer;
    m_obs_tag                = m_observee->AddObserver(::itk::ProgressEvent(), itk_call_back);
    m_initialized            = true; // NOLINT(cppcoreguidelines-prefer-member-initializer)
}

//------------------------------------------------------------------------------

template<typename OBSERVEE>
progress_itk_to_fw<OBSERVEE>::~progress_itk_to_fw()
{
    if(m_initialized)
    {
        m_observee->RemoveObserver(m_obs_tag);
    }
}

} // namespace sight::io::itk
