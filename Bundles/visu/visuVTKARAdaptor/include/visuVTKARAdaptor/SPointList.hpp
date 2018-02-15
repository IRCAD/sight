/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __VISUVTKARADAPTOR_SPOINTLIST_HPP__
#define __VISUVTKARADAPTOR_SPOINTLIST_HPP__

#include "visuVTKARAdaptor/config.hpp"

#include <fwCore/base.hpp>

#include <fwData/Color.hpp>

#include <fwRenderVTK/IAdaptor.hpp>

namespace visuVTKARAdaptor
{

/**
 * @brief   Display a 2D point list.
 *
 *  The coordinate system of the point list is [ (0,0);(width,-height) ] whereas the coordinate system of the scene
 *  is [ (-width/2;-height/2,width/2:height/2], so we need to transform the points.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::visuVTKARAdaptor::SPointList" autoConnect="yes">
       <in key="pointlist" uid="..." />
       <config renderer="default" color="#cb1f72" radius="3.0" />
   </service>
   @endcode
 *
 * @subsection Input Input
 * - \b pointlist [::fwData::PointList]: displayed point list.
 *
 * @subsection Configuration Configuration
 * - \b renderer : ID of renderer the adaptor must use.
 * - \b color(optional) (default: white): point color.
 * - \b radius (optional) (default: 3.): point radius.
 */
class VISUVTKARADAPTOR_CLASS_API SPointList : public ::fwRenderVTK::IAdaptor
{

public:
    fwCoreServiceClassDefinitionsMacro( (SPointList)(::fwRenderVTK::IAdaptor) );

    /// Constructor.
    VISUVTKARADAPTOR_API SPointList() noexcept;

    /// Destructor.
    VISUVTKARADAPTOR_API virtual ~SPointList() noexcept;

protected:

    VISUVTKARADAPTOR_API void configuring() override;

    /// Initialize the service and update it.
    VISUVTKARADAPTOR_API void starting() override;

    /// Create/update vtk props.
    VISUVTKARADAPTOR_API void updating() override;

    /// Cleanup vtk props.
    VISUVTKARADAPTOR_API void stopping() override;

    /// Restart the service (stop-start).
    VISUVTKARADAPTOR_API void swapping() override;

private:

    /// Point color.
    ::fwData::Color::sptr m_pointColor;

    /// Point size.
    double m_radius;

};

} //namespace visuVTKARAdaptor

#endif // __VISUVTKARADAPTOR_SPOINTLIST_HPP__
