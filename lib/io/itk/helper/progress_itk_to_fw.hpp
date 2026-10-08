/************************************************************************
 *
 * Copyright (C) 2009-2026 IRCAD France
 * Copyright (C) 2012-2015 IHU Strasbourg
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

#include <core/macros.hpp>
#include <core/notification/observer.hpp>
#include <memory>

#include <itkCommand.h>
#include <itkEventObject.h>
#include <itkProcessObject.h>
#include <itkSmartPointer.h>

namespace sight::io::itk
{

//------------------------------------------------------------------------------

class progressor_base
{
public:

    using sptr = std::shared_ptr<progressor_base>;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
class progress_itk_to_fw : public progressor_base
{
public:

    progress_itk_to_fw(OBSERVEE _observee, sight::sptr<core::notification::observer> _observer);
    virtual ~progress_itk_to_fw();

private:

    OBSERVEE m_observee;
    // observertag used by itk
    std::uint64_t m_obs_tag;
};

//------------------------------------------------------------------------------

class progressor
{
public:

    using sptr = std::shared_ptr<progressor>;

    template<typename OBS>
    progressor(OBS _filter, sight::sptr<core::notification::observer> _observer) :
        m_progressor(std::make_shared<progress_itk_to_fw<OBS> >(_filter, _observer))
    {
    }

    progressor_base::sptr m_progressor;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE>
progress_itk_to_fw<OBSERVEE>::progress_itk_to_fw(
    OBSERVEE _observee,
    sight::sptr<core::notification::observer> _observer
) :
    m_observee(std::move(_observee)),
    m_obs_tag(std::numeric_limits<std::uint64_t>::max())
{
    class local_command : public ::itk::Command
    {
    public:

        using self_t  = local_command;
        using Pointer = ::itk::SmartPointer<self_t>; // NOLINT(readability-identifier-naming)
        itkNewMacro(local_command);

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

        sight::sptr<core::notification::observer> m_observer;
    };

    typename local_command::Pointer itk_call_back;
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
