/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2016.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __SYNCTIMELINE_SMATRIXTLSYNCHRONIZER_HPP__
#define __SYNCTIMELINE_SMATRIXTLSYNCHRONIZER_HPP__

#include "syncTimeline/config.hpp"

#include <arServices/ISynchronizer.hpp>
#include <fwCore/base.hpp>
#include <fwCore/HiResClock.hpp>
#include <fwData/Composite.hpp>
#include <fwServices/IController.hpp>
#include <fwThread/Timer.hpp>


namespace arData
{
class FrameTL;
class MatrixTL;
}

namespace fwData
{
class Image;
class TransformationMatrix3D;
}

namespace syncTimeline
{
/**
 * @brief   SMatrixTLSynchronizer service synchronizes tracking matrices.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
     <service uid="matrixToolsSynchronizer" type="::syncTimeline::SMatrixTLSynchronizer">
         <in key="matrixTL" uid="matrixToolsTL" autoConnect="yes" />
         <inout group="matrices">
             <key uid="markerEndoMX" />
             <key uid="tipEndoMX" />
             <key uid="markerTool1MX" />
             <key uid="tipTool1MX" />
             <key uid="markerTool2MX" />
             <key uid="tipTool2MX" />
         </inout>
     </service>
   @endcode
 * @subsection Input Input
 * - \b matrixTL [::arData::MatrixTL]: matrix timeline used to extract matrices.
 * @subsection In-Out In-Out
 * - \b matrices [::fwData::TransformationMatrix3D]: list of TransformationMatrix3D used to store extracted matrices.
 */
class SYNCTIMELINE_CLASS_API SMatrixTLSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixTLSynchronizer)(::arServices::ISynchronizer));

    /**
     * @brief Constructor.
     */
    SYNCTIMELINE_API SMatrixTLSynchronizer() noexcept;

    /**
     * @brief Destructor.
     */
    virtual ~SMatrixTLSynchronizer() noexcept
    {
    }

    typedef std::map< unsigned long, std::string > MatrixIndexNameType;

protected:

    /// Does nothing
    SYNCTIMELINE_API void configuring();

    /// This method is used to initialize the service.
    SYNCTIMELINE_API void starting();

    /// Does nothing.
    SYNCTIMELINE_API void stopping();

    /// Does nothing.
    SYNCTIMELINE_API void updating();

    /// Synchronize
    SYNCTIMELINE_API void synchronize();

    SYNCTIMELINE_API ::fwServices::IService::KeyConnectionsMap getAutoConnections() const;

private:

    MatrixIndexNameType m_matrixIndexName;
};

} //namespace syncTimeline
#endif  // __SYNCTIMELINE_SMATRIXTLSYNCHRONIZER_HPP__
