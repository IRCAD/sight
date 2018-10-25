/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

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
 * @section Slots Slots
 * - \b updateVisibility(bool) : show/hide the point
 *
 * @code{.xml}
      <service uid="..." type="::visuVTKAdaptor::SPoint" autoConnect="yes">
        <inout key="point" uid="..." />
        <config renderer="default" picker="..." color="#FFFFFF" selectedColor="#FFFFFF" radius="10" interaction="on"/>
      </service>
     @endcode
 *
 *  @subsection In-Out In-Out
 * - \b pointList [::fwData::Point]: point to display
 *
 * @subsection Configuration Configuration
 * - \b renderer : defines the renderer to show the point.
 * - \b picker : defines the picker of the point.
 * - \b color(#FFFFFF) : color of the point.
 * - \b selectedColor(#FFFFFF) : color when the point is selected.
 * - \b radius(double) : point radius.
 * - \b interaction (optional, default: on): if "on" interactions are enabled
 */

class VISUVTKADAPTOR_CLASS_API SPoint : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SPoint)(::fwRenderVTK::IAdaptor) )

    static const ::fwServices::IService::KeyType s_POINT_INOUT;

    VISUVTKADAPTOR_API SPoint() noexcept;

    VISUVTKADAPTOR_API virtual ~SPoint() noexcept;

    VISUVTKADAPTOR_API void setRadius(const double radius);

    VISUVTKADAPTOR_API void setColor(const float red, const float green, const float blue, const float alpha = 1.0);

    VISUVTKADAPTOR_API void setSelectedColor(const float red, const float green, const float blue,
                                             const float alpha = 1.0);

    VISUVTKADAPTOR_API void setInteraction(const bool interaction = true);

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

    /// Slot: update point visibility (true = visible)
    VISUVTKADAPTOR_API void updateVisibility( bool isVisible );

protected:

    VISUVTKADAPTOR_API void configuring() override;
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
