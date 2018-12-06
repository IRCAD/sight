/************************************************************************
 *
 * Copyright (C) 2009-2018 IRCAD France
 * Copyright (C) 2012-2018 IHU Strasbourg
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

#include "fwRenderVTK/config.hpp"

namespace fwRenderVTK
{

/**
 * @brief   IInteractorStyle is an interface dedicated to hold some flags for class inherited from vtkInteractorStyle.
 */

class FWRENDERVTK_CLASS_API IInteractorStyle
{
public:

    /// Constructor. Do nothing.
    IInteractorStyle();

    /// Destructor. Do nothing.
    virtual ~IInteractorStyle();

    /// Get the autorender flag
    bool getAutoRender() const;

    /// Set the autorender flag
    void setAutoRender(bool _autoRender);

private:

    /// Does the scene update automatically when something changes ?
    bool m_autoRender;
};

//------------------------------------------------------------------------------

inline IInteractorStyle::IInteractorStyle() :
    m_autoRender(true)
{
}

//------------------------------------------------------------------------------

inline IInteractorStyle::~IInteractorStyle()
{
}

//------------------------------------------------------------------------------

inline bool IInteractorStyle::getAutoRender() const
{
    return m_autoRender;
}

//------------------------------------------------------------------------------

inline void IInteractorStyle::setAutoRender(bool _autoRender)
{
    m_autoRender = _autoRender;
}

//------------------------------------------------------------------------------

}
