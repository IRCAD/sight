/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SPOINTLABEL_HPP__
#define __VISUVTKADAPTOR_SPOINTLABEL_HPP__

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
    fwCoreServiceClassDefinitionsMacro( (SPointLabel)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SPointLabel();

    static const ::fwServices::IService::KeyType s_POINT_INPUT;

protected:

    VISUVTKADAPTOR_API void starting();
    VISUVTKADAPTOR_API void updating();
    VISUVTKADAPTOR_API void stopping();

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Point::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsMap getAutoConnections() const;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SPOINTLABEL_HPP__

