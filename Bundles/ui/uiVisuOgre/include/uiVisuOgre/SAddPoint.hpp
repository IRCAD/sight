/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __UIVISUOGRE_SADDPOINT_HPP__
#define __UIVISUOGRE_SADDPOINT_HPP__

#include <uiVisuOgre/config.hpp>

#include <fwServices/IController.hpp>

fwCorePredeclare( (fwData)(Object) )

namespace uiVisuOgre
{

/**
 * @brief   Add points in a ::fwData::PointList
 *
 * @section Slots Slots
 * - \b addPoint(::fwData::Point) : add a new point in the list.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::uiVisuOgre::SAddPoint">
            <inout key="pointList" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b pointList [::fwData::PointList]: target points list.
 */
class UIVISUOGRE_CLASS_API SAddPoint : public ::fwServices::IController
{
public:

    fwCoreServiceClassDefinitionsMacro( (SAddPoint)(::fwServices::IController) )

    /**
     * @name Slots API
     * @{
     */
    UIVISUOGRE_API static const ::fwCom::Slots::SlotKeyType s_ADDPOINT_SLOT;
    /** @} */

    /// Constructor. Creates slot.
    UIVISUOGRE_API SAddPoint() noexcept;

    /// Destructor. Does nothing
    UIVISUOGRE_API virtual ~SAddPoint() noexcept;

protected:
    UIVISUOGRE_API virtual void configuring() override;

    /// Does nothing
    UIVISUOGRE_API virtual void starting() override;

    /// Does nothing
    UIVISUOGRE_API virtual void stopping() override;

    /// Does nothing
    UIVISUOGRE_API virtual void updating() override;

private:

    /**
     * @name Slots methods
     * @{
     */

    /// SLOT : Sends a signal with the clicked poin coordinates.
    void addPoint(SPTR(::fwData::Object) _pointObject);

    /**
     * @}
     */
};
} // uiVisuOgre

#endif // __UIVISUOGRE_SADDPOINT_HPP__
