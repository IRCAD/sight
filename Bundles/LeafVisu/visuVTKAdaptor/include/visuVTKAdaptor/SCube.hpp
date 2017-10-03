/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKADAPTOR_SCUBE_HPP__
#define __VISUVTKADAPTOR_SCUBE_HPP__

#include "visuVTKAdaptor/config.hpp"

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKAdaptor
{

/**
 * @brief Render a cube on the generic scene
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service uid="cube" type="::visuVTKAdaptor::SCube">
        <config renderer="default" />
    </service>
   @endcode
 * @subsection Configuration Configuration:
 * - \b config(mandatory) : contains the adaptor configuration
 *    - \b renderer (mandatory): defines the renderer to show the cube.
 */
class VISUVTKADAPTOR_CLASS_API SCube : public ::fwRenderVTK::IAdaptor
{

public:

    fwCoreServiceClassDefinitionsMacro( (SCube)(::fwRenderVTK::IAdaptor) );

    VISUVTKADAPTOR_API SCube() noexcept;

    VISUVTKADAPTOR_API virtual ~SCube() noexcept;

protected:

    VISUVTKADAPTOR_API void configuring() override;
    VISUVTKADAPTOR_API void starting() override;
    VISUVTKADAPTOR_API void updating() override;
    VISUVTKADAPTOR_API void stopping() override;
};

#endif //__VISUVTKADAPTOR_SCUBE_HPP__

} //namespace visuVTKAdaptor
