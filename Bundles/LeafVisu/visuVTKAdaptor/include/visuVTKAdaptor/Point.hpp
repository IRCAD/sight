/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_POINT_HPP__

#ifndef __VISUVTKADAPTOR_POINT_HPP__
#define __VISUVTKADAPTOR_POINT_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

class vtkHandleWidget;
class vtkHandleRepresentation;
class vtkCommand;

namespace visuVTKAdaptor
{

/**
 * @brief Adaptor to display a point.
 *
 * @section Signals Signals
 * - interactionStarted : when point interaction is started.
 *
 * @code{.xml}
      <adaptor id="..." class="::visuVTKAdaptor::Point" objectId="self">
        <config renderer="default" picker="..." color="#FFFFFF" selectedColor="#FFFFFF" radius="10" interaction="on"/>
      </adaptor>
     @endcode
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the point.
 * - \b picker : defines the picker of the point.
 * - \b color(#FFFFFF) : color of the point.
 * - \b selectedColor(#FFFFFF) : color when the point is selected.
 * - \b radius(double) : point radius.
 * - \b interaction (optional, default: on): if "on" interactions are enabled
 */

class VISUVTKADAPTOR_CLASS_API Point : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (Point)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API Point() noexcept;

    VISUVTKADAPTOR_API virtual ~Point() noexcept;

    VISUVTKADAPTOR_API void setRadius(const double radius);

    VISUVTKADAPTOR_API void setColor(const float red, const float green, const float blue, const float alpha = 1.0);

    VISUVTKADAPTOR_API void setSelectedColor(const float red, const float green, const float blue,
                                             const float alpha = 1.0);

    VISUVTKADAPTOR_API void setInteraction(const bool interaction = true);

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect Point::s_MODIFIED_SIG to this::s_UPDATE_SLOT
     */
    VISUVTKADAPTOR_API virtual KeyConnectionsType getObjSrvConnections() const;

    /**
     * @Signals
     * @{
     */
    /// Type of signal when point interaction is started
    typedef ::fwCom::Signal< void () > InteractionStartedSignalType;
    VISUVTKADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_INTERACTION_STARTED_SIG;
    /**
     * @}
     */

protected:

    VISUVTKADAPTOR_API void doStart();
    VISUVTKADAPTOR_API void doConfigure();
    VISUVTKADAPTOR_API void doSwap();
    VISUVTKADAPTOR_API void doUpdate();
    VISUVTKADAPTOR_API void doStop();

private:
    vtkHandleWidget* m_handle;
    vtkHandleRepresentation* m_representation;
    vtkCommand* m_pointUpdateCommand;

    ::fwData::Color::sptr m_ptColor;
    ::fwData::Color::sptr m_ptSelectedColor;
    double m_radius;

    /// If true, the interactions are enabled
    bool m_interaction;
};

} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_POINT_HPP__

#endif // __VISUVTKADAPTOR_POINT_HPP__
