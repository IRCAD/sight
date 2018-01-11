/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2017.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IOTIMELINE_SMATRIXWRITER_HPP__
#define __IOTIMELINE_SMATRIXWRITER_HPP__

#include "ioTimeline/config.hpp"

#include <arData/MatrixTL.hpp>

#include <io/IWriter.hpp>

namespace ioTimeline
{

/**
 * @brief This service allows the user to save the timeline matrices in a csv file.
 *
 * @note The method 'updating' allows to save the timeline matrix with the current timestamp. If you want to save all
 * the
 *       matrices when they are pushed in the timeline, you must use the slots 'startRecord' and 'stopRecord'
 *
 * @section Slots Slots
 * - \b startRecord() : start recording
 * - \b stopRecord() : stop recording
 * - \b write(::fwCore::HiResClock::HiResClockType) : write matrix in a csv file
 * - \b saveMatrix(::fwCore::HiResClock::HiResClockType) : save current matrices
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
   <service type="::ioTimeline::SMatrixWriter">
       <in key="data" uid="..." autoConnect="yes" />
       <windowTitle>Select the file to save the matrix timeline to</windowTitle>
   </service>
   @endcode
 * @subsection Input Input
 * - \b data [::arData::MatrixTL]: timeline containing the matrices to save.
 *
 * @subsection Configuration Configuration
 * - \b windowTitle: allow overriding the default title of the modal file selection window. \see io::IWriter
 */
class IOTIMELINE_CLASS_API SMatrixWriter : public ::io::IWriter
{

public:

    fwCoreServiceClassDefinitionsMacro( (SMatrixWriter)(::io::IWriter) );

    /// Constructor.
    IOTIMELINE_API SMatrixWriter() noexcept;

    /// Destructor. Does nothing
    IOTIMELINE_API virtual ~SMatrixWriter() noexcept;

    /// Defines auto connection for this service (saveFrame()) to the frame timeline (objectPushed)
    IOTIMELINE_API virtual ::fwServices::IService::KeyConnectionsMap getAutoConnections() const override;

    /// Display a location dialog allowing to select the video file to save
    IOTIMELINE_API virtual void configureWithIHM() override;

    /// Return file type (::io::FOLDER)
    IOTIMELINE_API virtual ::io::IOPathType getIOPathType() const override;

protected:

    /// Does nothing
    IOTIMELINE_API virtual void configuring() override;

    /// Does nothing
    IOTIMELINE_API virtual void starting() override;

    /// Does nothing
    IOTIMELINE_API virtual void stopping() override;

    /// Does nothing
    IOTIMELINE_API virtual void updating() override;

private:

    /// SLOT: Save current matrices.
    void saveMatrix(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Write matrices in csv file
    void write(::fwCore::HiResClock::HiResClockType timestamp);

    /// SLOT: Start recording
    void startRecord();

    /// SLOT: Stop recording
    void stopRecord();

    bool m_isRecording; ///< flag if the service is recording.

    std::ofstream* m_filestream;
};

} // ioTimeline

#endif // __IOTIMELINE_SMATRIXWRITER_HPP__
