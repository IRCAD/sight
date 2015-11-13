/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __CTRLPOINTSELECTION_SUPDATENEGATO_HPP__
#define __CTRLPOINTSELECTION_SUPDATENEGATO_HPP__

#include <fwServices/IController.hpp>

#include <fwData/Image.hpp>
#include <fwData/Point.hpp>

#include "ctrlPointSelection/config.hpp"


namespace ctrlPointSelection
{

/**
 * @brief Updates negato slices so that they cross at a given point.
 * @class SUpdateNegato
 */
class CTRLPOINTSELECTION_CLASS_API SUpdateNegato : public ::fwServices::IController
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SUpdateNegato)(::fwServices::IController) );

    /**
     * @name Constructor/Destructor.
     * @{ */
    CTRLPOINTSELECTION_API SUpdateNegato() throw();
    CTRLPOINTSELECTION_API virtual ~SUpdateNegato() throw();
    /**  @} */

    /**
     * @name Slots.
     * @{ */
    typedef ::fwCom::Slot<void (::fwData::Point::sptr)> SelectNewPointSlotType;
    CTRLPOINTSELECTION_API static const ::fwCom::Slots::SlotKeyType s_SELECT_NEW_POINT_SLOT;
    /**  @} */

protected:

    /**
     * @name Overrides.
     * @{ */
    virtual void starting() throw(::fwTools::Failed);
    virtual void stopping() throw(::fwTools::Failed);
    virtual void configuring() throw(::fwTools::Failed);
    virtual void updating() throw(::fwTools::Failed);
    /**  @} */

    /**
     * @brief Crosses negato slices at the given point.
     *
     * @param selectedPoint point at which negato slices must cross
     */
    void updateSlices(::fwData::Point::sptr selectedPoint);

private:

    /// Slot triggered to retrieve a spline point on a negato.
    SelectNewPointSlotType::sptr m_slotSelectNewPoint;
};

} // namespace ctrlPointSelection

#endif // __CTRLPOINTSELECTION_SUPDATENEGATO_HPP__

