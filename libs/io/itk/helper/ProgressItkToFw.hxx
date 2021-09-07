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
    {
    }

    typedef LocalCommand Self;
    typedef ::itk::SmartPointer<LocalCommand>      Pointer;
    itkNewMacro(Self);

    //------------------------------------------------------------------------------

    void Execute(::itk::Object* caller, const ::itk::EventObject& event) override
    {
        auto* po = dynamic_cast<::itk::LightProcessObject*>(caller);
        if( !po )
        {
            return;
        }
        float percent = po->GetProgress();
        m_adviser->notifyProgress( percent, m_msg );
    }

    //------------------------------------------------------------------------------

    void Execute(const ::itk::Object* caller, const ::itk::EventObject& event) override
    {
        auto* po = dynamic_cast<::itk::LightProcessObject*>( const_cast<::itk::Object*>(caller));
        Execute(po, event);
    }

    std::string m_msg;
    SPTR(core::tools::ProgressAdviser) m_adviser;
};

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::ProgressItkToFw(OBSERVEE observee, SPTR(core::tools::ProgressAdviser)observer,
                                            std::string msg) :
    m_observee( observee),
    m_obsTag(std::numeric_limits<unsigned long>::max()),
    m_initialized(false)
{
    typename LocalCommand::Pointer itkCallBack;
    itkCallBack            = LocalCommand::New();
    itkCallBack->m_msg     = msg;
    itkCallBack->m_adviser = observer;
    m_obsTag               = m_observee->AddObserver(::itk::ProgressEvent(), itkCallBack );
    m_initialized          = true;
}

//------------------------------------------------------------------------------

template<typename OBSERVEE >
ProgressItkToFw<OBSERVEE >::~ProgressItkToFw()
{
    if( m_initialized)
    {
        m_observee->RemoveObserver(m_obsTag);
    }
}

}
