/************************************************************************
 *
 * Copyright (C) 2009-2015 IRCAD France
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

#ifndef __FWRENDERVTK_VTK_INSTANTIATOR_HPP__
#define __FWRENDERVTK_VTK_INSTANTIATOR_HPP__

#include "fwRenderVTK/config.hpp"

namespace fwRenderVTK
{

namespace vtk
{

class FWRENDERVTK_CLASS_API Instantiator
{
public:
    FWRENDERVTK_API static void ClassInitialize();
    FWRENDERVTK_API static void ClassFinalize();
};

} //vtk
} //fwRenderVTK

#endif //__FWRENDERVTK_VTK_INSTANTIATOR_HPP__
