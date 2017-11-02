/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __ECHOEDSIMU_SIMAGEMANAGER_HPP__
#define __ECHOEDSIMU_SIMAGEMANAGER_HPP__

#include "echoEdSimu/config.hpp"

#include <arServices/ISimulator.hpp>

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwData/Image.hpp>

namespace echoEdSimu
{

/**
 * @brief   Update the images according to the "craniocaudal" and the "radial" index
 *
 * @section Slots Slots
 * - \b updateSingle(int, std::string) : called to update the "craniocaudal" or the "radial" index.
 * - \b updateBoth(int,int) : called to update the craniocaudal and the radial indexes.

 * @section XML XML Configuration
 *
 * @code{.xml}
        <service type="::echoEdSimu::SImageManager">
                <inout key="echoImage" uid="..." />
                <inout key="echoCTImage" uid="..." />
       </service>
   @endcode
 * @subsection In-Out In-Out:
 * - \b echoImage [::fwData::Image]: echographic image
 * - \b echoCTImage [::fwData::Image]: CT image represented in a echographic plane
 */
class ECHOEDSIMU_CLASS_API SImageManager : public ::arServices::ISimulator
{

public:

    fwCoreServiceClassDefinitionsMacro( (SImageManager)(::arServices::ISimulator) );

    /// Constructor. Do nothing.
    ECHOEDSIMU_API SImageManager() noexcept;

    /// Destructor. Do nothing.
    ECHOEDSIMU_API virtual ~SImageManager() noexcept;

    /**
     * @name Slots API
     * @{
     */
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_SINGLE_SLOT;
    ECHOEDSIMU_API static const ::fwCom::Slots::SlotKeyType s_UPDATE_BOTH_SLOT;
    /** @} */

protected:

    /// Do nothing
    ECHOEDSIMU_API virtual void configuring() override;

    /// Grab the input echographic and CT images
    ECHOEDSIMU_API virtual void starting() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void stopping() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void updating() override;

    /// Do nothing
    ECHOEDSIMU_API virtual void swapping() override;

private:

    /// Slot: update the image in the craniocaudal direction or in the radial direction
    void updateSingle(int i, std::string key);

    /// Slot: Common function used to update the image
    void updateBoth(int i, int j);

    typedef std::vector< std::vector< ::fwData::Image::sptr > > ImagesVecType;

    /// Load an image from the disk
    void load(::boost::filesystem::path dir, ImagesVecType& images);

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

