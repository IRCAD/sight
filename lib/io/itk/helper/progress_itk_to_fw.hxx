/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
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

#include "core/progress/observer.hpp"

#include <core/base.hpp>

#include <itkCommand.h>
#include <itkEventObject.h>
#include <itkLightProcessObject.h>
#include <itkSmartPointer.h>

#include <utility>

namespace sight::io::itk
{

class local_command : public ::itk::Command
{
public:

    using self_t  = local_command;
    using Pointer = ::itk::SmartPointer<self_t>; // NOLINT(readability-identifier-naming)
    itkNewMacro(self_t);

    //------------------------------------------------------------------------------

    void Execute(const ::itk::Object* _caller, const ::itk::EventObject& /*event*/) override
    {
        const auto* po = dynamic_cast<const ::itk::ProcessObject*>(_caller);
        if(po != nullptr)
        {
            auto percent = po->GetProgress();
            m_observer->done_work(static_cast<std::uint64_t>(percent * 100));
        }
    }

    //------------------------------------------------------------------------------

    void Execute(::itk::Object* _caller, const ::itk::EventObject& _event) override
    {
        const ::itk::Object* const_caller = _caller;
        Execute(const_caller, _event);
    }

    SPTR(core::progress::observer) m_observer;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
progress_itk_to_fw<OBSERVEE>::progress_itk_to_fw(
    OBSERVEE _observee,
    SPTR(core::progress::observer)_observer
) :
    m_observee(std::move(_observee)),
    m_obs_tag(std::numeric_limits<std::uint64_t>::max())
{
    local_command::Pointer itk_call_back;
    itk_call_back             = local_command::New();
    itk_call_back->m_observer = _observer;
    m_obs_tag                 = m_observee->AddObserver(::itk::ProgressEvent(), itk_call_back);
}

//------------------------------------------------------------------------------

template<typename OBSERVEE>
progress_itk_to_fw<OBSERVEE>::~progress_itk_to_fw()
{
    m_observee->RemoveObserver(m_obs_tag);
}

} // namespace sight::io::itk
