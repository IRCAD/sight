/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ECHOEDSIMU_SIMAGEMANAGER_HPP__
#define __ECHOEDSIMU_SIMAGEMANAGER_HPP__

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>

#include <arServices/ISimulator.hpp>

#include "echoEdSimu/config.hpp"

namespace echoEdSimu
{

class ECHOEDSIMU_CLASS_API SImageManager : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro ( (SImageManager)(::arServices::ISimulator) );

    /// Constructor. Do nothing.
    ECHOEDSIMU_API SImageManager() throw();

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SImageManager() throw();

    /**
     * @name Slots API
     * @{
     */
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_CRANIOCAUDAL_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_RADIAL_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_BOTH_SLOT;
    /** @} */

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring() throw(fwTools::Failed);

    /// Grab the input echographic and CT images
    ECHOEDSIMU_API virtual void starting() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void updating() throw(::fwTools::Failed);

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping() throw(::fwTools::Failed);

private:

    /// Slot: update the image in the craniocaudal direction
    void updateCranioCaudalImage(int i);

    /// Slot: update the image in the radial direction
    void updateRadialImage(int j);

    /// Slot: Common function used to update the image
    void updateBoth(int i, int j);

    typedef std::vector< std::vector< ::fwData::Image::sptr > > ImagesVecType;

    /// Load an image from the disk
    void load(::boost::filesystem::path dir, ImagesVecType & images);

    ImagesVecType m_ctImages;
    ImagesVecType m_echoImages;

    ::fwData::Image::sptr m_ctImage;
    ::fwData::Image::sptr m_echoImage;

    /// Current craniocaudal index
    int m_cranioCaudalIndex;
    /// Current radial index
    int m_radialIndex;
};

} // namespace echoEdSimu

#endif /*__ECHOEDSIMU_SIMAGEMANAGER_HPP__*/


