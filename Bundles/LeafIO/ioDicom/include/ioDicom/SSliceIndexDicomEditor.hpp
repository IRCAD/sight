/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2015.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOM_SSLICEINDEXDICOMEDITOR_HPP__
#define __IODICOM_SSLICEINDEXDICOMEDITOR_HPP__

#include "ioDicom/config.hpp"

#include <fwCom/Slot.hpp>
#include <gui/editor/IEditor.hpp>
#include <io/IReader.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>
#include <QSlider>
#include <QWidget>

#include <boost/asio/deadline_timer.hpp>
#include <boost/asio/io_service.hpp>
#include <boost/filesystem/path.hpp>

namespace fwData
{
class Composite;
class Integer;
}

namespace fwThread
{
class Timer;
}

namespace fwMedData
{
class SeriesDB;
}

namespace fwRuntime
{
struct ConfigurationElement;
}

namespace ioDicom
{

/**
 * @brief   This editor service is used to select a slice index and read the
 *          corresponding image on the local computer.
 */
class IODICOM_CLASS_API SSliceIndexDicomEditor : public QObject,
                                                 public ::gui::editor::IEditor
{
Q_OBJECT;

public:

    fwCoreServiceClassDefinitionsMacro ( (SSliceIndexDicomEditor)( ::gui::editor::IEditor ) );

    IODICOM_API static const ::fwCom::Slots::SlotKeyType s_READ_IMAGE_SLOT;
    typedef ::fwCom::Slot<void (std::size_t)> ReadImageSlotType;

    IODICOM_API static const ::fwCom::Slots::SlotKeyType s_DISPLAY_MESSAGE_SLOT;
    typedef ::fwCom::Slot<void (const std::string&)> DisplayMessageSlotType;

    /**
     * @brief Constructor
     */
    IODICOM_API SSliceIndexDicomEditor() throw();

    /**
     * @brief Destructor
     */
    IODICOM_API virtual ~SSliceIndexDicomEditor() throw();

private Q_SLOTS:
    /**
     * @brief Slot called when the slider is moved
     * @param[in] value Slider value
     */
    IODICOM_API void changeSliceIndex(int value);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
       @verbatim
       <service uid="sliderIndexDicomEditor" type="::gui::editor::IEditor"
         impl="::ioDicom::dcmtk::editor::SSliceIndexDicomEditor" autoConnect="yes">
         <config compositeUID="previewComposite" imageKey="image" dicomReader="::ioGdcm::SSeriesDBReader"
            delay="500">
            <dicomReaderConfig> <!-- optional -->
                <!-- here goes configuration for dicom reader implementation -->
            </dicomReaderConfig>
         </config>
       </service>
       @endverbatim
     */
    IODICOM_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOM_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOM_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOM_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOM_API void info(std::ostream &_sstream );

    /// Function called when a new slice must be displayed
    IODICOM_API void triggerNewSlice();

    /**
     * @brief Read the selected image
     * @param[in] selectedSliceIndex Selected slice of the image that must be read
     */
    IODICOM_API void readImage(std::size_t selectedSliceIndex);

    /**
     * @brief Display error message
     * @param[in] message Message that must be displayed
     */
    IODICOM_API void displayErrorMessage(const std::string& message) const;

    /// Slot to call readLocalSeries method
    ReadImageSlotType::sptr m_slotReadImage;

    /// Slot to call displayErrorMessage method;
    DisplayMessageSlotType::sptr m_slotDisplayMessage;

    /// Slice index slider
    QPointer< QSlider > m_sliceIndexSlider;

    /// Slice index line edit
    QPointer< QLineEdit > m_sliceIndexLineEdit;

    /// Number of instances
    std::size_t m_numberOfSlices;

    /// IODICOM Reader
    std::string m_dicomReaderType;

    /// Reader
    ::io::IReader::wptr m_dicomReader;

    /// Image Key
    std::string m_imageKey;

    /// Composite UID
    std::string m_compositeUID;

    /// Composite
    SPTR(::fwData::Composite) m_composite;

    /// Temporary SeriesDB
    SPTR(::fwMedData::SeriesDB) m_tempSeriesDB;

    /// Axial slice index
    SPTR(fwData::Integer) m_axialIndex;
    /// Frontal slice index
    SPTR(::fwData::Integer) m_frontalIndex;
    /// Sagittal slice index
    SPTR(::fwData::Integer) m_sagittalIndex;

    /// Timer used to generate the new slice selection delay
    SPTR(::fwThread::Timer) m_delayTimer2;

    /// Delay
    std::size_t m_delay;

    /// Optional configuration to set to reader implementation
    SPTR(::fwRuntime::ConfigurationElement) m_readerConfig;
};

} // namespace ioDicom

#endif // __IODICOM_SSLICEINDEXDICOMEDITOR_HPP__
