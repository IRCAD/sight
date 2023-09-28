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

class LocalCommand : public ::itk::Command
{
public:

    LocalCommand()
    = default;

    typedef LocalCommand Self;
    typedef ::itk::SmartPointer<LocalCommand> Pointer;
    itkNewMacro(Self);

    //------------------------------------------------------------------------------

    void Execute(const ::itk::Object* caller, const ::itk::EventObject& /*event*/) override
    {
        const auto* po = dynamic_cast<const ::itk::LightProcessObject*>(caller);
        if(po == nullptr)
        {
            return;
        }

        float percent = po->GetProgress();
        m_adviser->notify_progress(percent, m_msg);
    }

    //------------------------------------------------------------------------------

    void Execute(::itk::Object* caller, const ::itk::EventObject& event) override
    {
        const ::itk::Object* constCaller = caller;
        Execute(constCaller, event);
    }

    std::string m_msg;
    SPTR(core::tools::progress_adviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
ProgressItkToFw<OBSERVEE>::ProgressItkToFw(
    OBSERVEE observee,
    SPTR(core::tools::progress_adviser)observer,
    std::string msg
) :
    m_observee(observee),
    m_obsTag(std::numeric_limits<std::uint64_t>::max())
{
    typename LocalCommand::Pointer itkCallBack;
    itkCallBack            = LocalCommand::New();
    itkCallBack->m_msg     = msg;
    itkCallBack->m_adviser = observer;
    m_obsTag               = m_observee->AddObserver(::itk::ProgressEvent(), itkCallBack);
    m_initialized          = true; // NOLINT(cppcoreguidelines-prefer-member-initializer)
}

//------------------------------------------------------------------------------

template<typename OBSERVEE>
ProgressItkToFw<OBSERVEE>::~ProgressItkToFw()
{
    if(m_initialized)
    {
        m_observee->RemoveObserver(m_obsTag);
    }
}

} // namespace sight::io::itk
