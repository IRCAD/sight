/************************************************************************
 *
 * Copyright (C) 2009-2017 IRCAD France
 * Copyright (C) 2012-2017 IHU Strasbourg
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

#ifndef __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__
#define __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

class vtkInteractorStyle;

namespace visuVTKAdaptor
{

/**
 */

/**
 * @brief Managing vtk interactor style
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SInteractorStyle">
       <config renderer="default" style="..." />
   </service>
   @endcode
 * @subsection Configuration Configuration
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to add the interactor
 *    - \b style (mandatory): defines the vtkInteractorStyle to use (ex : InteractorStyle2DForNegato)
 */
class VISUVTKADAPTOR_CLASS_API SInteractorStyle : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SInteractorStyle)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SInteractorStyle() noexcept;

    VISUVTKADAPTOR_API virtual ~SInteractorStyle() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    void setInteractorStyle(vtkInteractorStyle* interactor);

    std::string m_configuredStyle;
    vtkInteractorStyle* m_interactorStyle;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SINTERACTORSTYLE_HPP__
