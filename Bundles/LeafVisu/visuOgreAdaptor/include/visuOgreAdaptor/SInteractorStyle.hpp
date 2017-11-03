/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUOGREADAPTOR_SINTERACTORSTYLE_HPP__
#define __VISUOGREADAPTOR_SINTERACTORSTYLE_HPP__

#include <visuOgreAdaptor/config.hpp>

#include <fwCom/Signal.hpp>
#include <fwCom/Signals.hpp>
#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Object.hpp>

#include <fwRenderOgre/IAdaptor.hpp>

namespace visuOgreAdaptor
{

/**
 * @brief   Manage interactor style for Ogre
 *
 * @section Signals Signals
 * - \b pointClickedSignal(int) : Emitted when a point is clicked.
 *
 * @section Slots Slots
 * - \b pointClickedSlot(int) : Transmit the clicked point.

 * @section XML XML Configuration
 * @code{.xml}
        <service type="::visuOgreAdaptor::SInteractorStyle">
            <config render="..." style="InteractorStyle" />
       </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b render: Layer on which the interactions will be done.
 * - \b style: Style of the interactor depending on the type: 'Trackball', 'Fixed', 'Negato2D', 'Mesh', 'Video', or
 * 'VR', default to 'Trackball'.
 */
class VISUOGREADAPTOR_CLASS_API SInteractorStyle : public ::fwRenderOgre::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro((SInteractorStyle)(::fwRenderOgre::IAdaptor))

    /**
     * @name Signals API
     * @{
     */

    typedef ::fwCom::Signal< void ( ::fwData::Object::sptr ) > PointClickedSignalType;
    VISUOGREADAPTOR_API static const ::fwCom::Signals::SignalKeyType s_POINT_CLICKED_SIG;

    /** @} */

    /**
     * @name Slots API
     * @{
     */

    VISUOGREADAPTOR_API static const ::fwCom::Slots::SlotKeyType s_POINT_CLICKED_SLOT;

    /** @} */

    /// Constructor. Creates signals and slots
    VISUOGREADAPTOR_API SInteractorStyle() noexcept;

    /// Destructor. Does nothing
    VISUOGREADAPTOR_API virtual ~SInteractorStyle() noexcept;

protected:

    /// Select the interactor style
    VISUOGREADAPTOR_API void configuring() override;

    /// Starting method
    VISUOGREADAPTOR_API void starting() override;

    /// Update the interactor
    VISUOGREADAPTOR_API void updating() override;

    /// Stopping method
    VISUOGREADAPTOR_API void stopping() override;

private:

    /**
     * @name Slots methods
     * @{
     */

    /// Slot: sends a signal when the interactor has recieved a clicked point signal
    void clickedPoint(fwData::Object::sptr obj);

    /**
     * @}
     */

    /// Set interactor style
    void setInteractorStyle();

    /// Type of the configured style
    std::string m_configuredStyle;

    /**
     * @name Signals attributes
     * @{
     */

    /// Pointer to the generic signal
    PointClickedSignalType::sptr m_sigPointClicked;

    /**
     * @}
     */

    ///Connection service, needed for slot/signal association
    ::fwCom::helper::SigSlotConnection m_connections;
};

} //namespace visuOgreAdaptor

#endif // __VISUOGREADAPTOR_SINTERACTORSTYLE_HPP__
