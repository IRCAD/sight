/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2014-2015.
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


namespace extData
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
 * @brief   SMatrixTLSynchronizer service synchronizes video frame and tracking matrix.
 * @class   SMatrixTLSynchronizer
 */
class SYNCTIMELINE_CLASS_API SMatrixTLSynchronizer : public ::arServices::ISynchronizer
{

public:

    fwCoreServiceClassDefinitionsMacro((SMatrixTLSynchronizer)(::arServices::ISynchronizer));

    /**
     * @brief Constructor.
     */
    SYNCTIMELINE_API SMatrixTLSynchronizer() throw ();

    /**
     * @brief Destructor.
     */
    virtual ~SMatrixTLSynchronizer() throw ()
    {
    }

    typedef std::map< unsigned long, std::string > MatrixIndexNameType;

protected:
    /**
     * @brief This method is used to configure the service.
     *
     * @verbatim
       <service impl="::syncTimeline::SMatrixTLSynchronizer" type="::arServices::ISynchronizer">
           <matrices>
               <matrix index="0" to="matrix0" />
               <matrix index="1" to="matrix1" />
               <matrix index="2" to="matrix2" />
            </matrices>
       </service>
       @endverbatim
     * - \b matrices defines the matrixTL to synchronize.
     *   - \b index: index of the matrix in the timeline.
     *   - \b to: uid of the TransformationMatrix3D where to extract the matrix.
     */
    SYNCTIMELINE_API void configuring() throw (::fwTools::Failed);

    /// This method is used to initialize the service.
    SYNCTIMELINE_API void starting() throw (::fwTools::Failed);

    /// Does nothing.
    SYNCTIMELINE_API void stopping() throw (::fwTools::Failed);

    /// Does nothing.
    SYNCTIMELINE_API void updating() throw (::fwTools::Failed);

    /// Synchronize
    SYNCTIMELINE_API void synchronize();

    SYNCTIMELINE_API ::fwServices::IService::KeyConnectionsType getObjSrvConnections() const;

private:

    MatrixIndexNameType m_matrixIndexName;
};

} //namespace syncTimeline
#endif  // __SYNCTIMELINE_SMATRIXTLSYNCHRONIZER_HPP__
