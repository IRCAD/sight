/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SNAPSHOT_HPP__
#define __VISUVTKADAPTOR_SNAPSHOT_HPP__


#include "visuVTKAdaptor/config.hpp"
#include "visuVTKAdaptor/MeshFactory.hpp"

#include <fwCom/Slot.hpp>
#include <fwCom/Slots.hpp>

#include <fwRenderVTK/IVtkAdaptorService.hpp>

namespace visuVTKAdaptor
{


class VISUVTKADAPTOR_CLASS_API Snapshot : public ::fwRenderVTK::IVtkAdaptorService
{

public:

    fwCoreServiceClassDefinitionsMacro ( (Snapshot)(::fwRenderVTK::IVtkAdaptorService) );

    VISUVTKADAPTOR_API Snapshot() throw();

    VISUVTKADAPTOR_API virtual ~Snapshot() throw();

protected:

    VISUVTKADAPTOR_API void doStart() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doStop() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void configuring() throw(fwTools::Failed);
    VISUVTKADAPTOR_API void doSwap() throw(fwTools::Failed);
    // redraw all (stop then restart sub services)
    VISUVTKADAPTOR_API void doUpdate() throw(fwTools::Failed);

private:

    /**
     * @name Slots
     * @{
     */
    /// Type of slot to snap shot
    static const ::fwCom::Slots::SlotKeyType s_SNAP_SIG;

    /// Slot: snap shot the generic scene.
    void snap(std::string filePath);
    /**
     * @}
     */
};


} //namespace visuVTKAdaptor

#endif // __VISUVTKADAPTOR_SNAPSHOT_HPP__
