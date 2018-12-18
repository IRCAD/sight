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

#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/SText.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

#include <string>

class VISUVTKADAPTOR_CLASS_API vtkTextActor;

namespace visuVTKAdaptor
{

/**
 * @brief Displays a label on a point. The label is given in point field "m_labelId"
 *
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the labelPoint
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKAdaptor::SPointLabel" autoConnect="yes">
        <in key="point" uid="..." />
        <config renderer="default" color="#ff0000" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b point [::fwData::Point]: point where to display the label.
 *
 *  @subsection Configuration Configuration
 * - \b config (mandatory) : contains the adaptor configuration
 *    - \b color (optional) : text color in hexadecimal format : \#rrggbb or \#rrggbbaa (optional, default: white)
 */
class VISUVTKADAPTOR_CLASS_API SPointLabel : public SText
{

public:
    fwCoreServiceClassDefinitionsMacro( (SPointLabel)(::fwRenderVTK::IAdaptor) )

    VISUVTKADAPTOR_API SPointLabel();

    static const ::fwServices::IService::KeyType s_POINT_INPUT;

    /// Slot: update pointLabel visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility(bool isVisible);

protected:

    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Point::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const override;
};

} //namespace visuVTKAdaptor
