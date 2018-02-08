/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

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
