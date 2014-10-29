/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2013.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#ifndef __IODICOMEXT_COMMON_EDITOR_SPACSCONFIGURATIONEDITOR_HPP__
#define __IODICOMEXT_COMMON_EDITOR_SPACSCONFIGURATIONEDITOR_HPP__

#include <boost/filesystem/path.hpp>
#include <QObject>
#include <QWidget>
#include <QPointer>
#include <QLineEdit>
#include <QPushButton>
#include <QSpinBox>
#include <QComboBox>

#include <gui/editor/IEditor.hpp>
#include <fwDicomIOExt/data/PacsConfiguration.hpp>
#include <fwDicomIOExt/dcmtk/SeriesEnquirer.hpp>

#include "ioDicomExt/config.hpp"

namespace ioDicomExt
{

namespace common
{

namespace editor
{

/**
 * @brief   This editor service is used to edit a pacs configuration
 * @class   SPacsConfigurationEditor
 * @date    2013.
 */
class IODICOMEXT_CLASS_API SPacsConfigurationEditor : public QObject, public ::gui::editor::IEditor
{
Q_OBJECT;

public :

    fwCoreServiceClassDefinitionsMacro ( (SPacsConfigurationEditor)( ::gui::editor::IEditor ) ) ;
    /**
     * @brief Constructor
     */
    IODICOMEXT_API SPacsConfigurationEditor() throw();

    /**
     * @brief Destructor
     */
    IODICOMEXT_API virtual ~SPacsConfigurationEditor() throw();

private Q_SLOTS:
    /// Slot called for pinging the pacs
    IODICOMEXT_API void pingPacs();

    /// Slot called for changing the local application title
    IODICOMEXT_API void localApplicationTitleChanged();

    /// Slot called for changing the pacs host name
    IODICOMEXT_API void pacsHostNameChanged();

    /// Slot called for changing the pacs application title
    IODICOMEXT_API void pacsApplicationTitleChanged();

    /**
     * @brief Slot called for changing the pacs application port
     * @param[in] value Pacs application port
     */
    IODICOMEXT_API void pacsApplicationPortChanged(int value);

    /// Slot called for changing the move application title
    IODICOMEXT_API void moveApplicationTitleChanged();

    /**
     * @brief Slot called for changing the move application port
     * @param[in] value Move application port
     */
    IODICOMEXT_API void moveApplicationPortChanged(int value);

    /**
     * @brief Slot called for changing the retrieve method
     * @param[in] index Retrieve method index
     */
    IODICOMEXT_API void retrieveMethodChanged(int index);

protected:

    /**
     * @brief Configuring method. This method is used to configure the service.
     *
     * XML configuration sample:
     @verbatim
     <service uid="pacsConfigurationEditor" impl="::ioDicomExt::common::editor::SPacsConfigurationEditor"
         autoConnect="yes">
     </service>
     @endverbatim
    */
    IODICOMEXT_API virtual void configuring() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void starting() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void stopping() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void updating() throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API virtual void receiving( CSPTR(::fwServices::ObjectMsg) _msg ) throw(::fwTools::Failed);

    /// Override
    IODICOMEXT_API void info(std::ostream &_sstream ) ;

    /// Local application title
    QPointer< QLineEdit > m_localApplicationTitleWidget;

    /// Pacs host name
    QPointer< QLineEdit > m_pacsHostNameWidget;

    /// Pacs application title
    QPointer< QLineEdit > m_pacsApplicationTitleWidget;

    /// Pacs application port
    QPointer< QSpinBox > m_pacsApplicationPortWidget;

    /// Move application title
    QPointer< QLineEdit > m_moveApplicationTitleWidget;

    /// Move application port
    QPointer< QSpinBox > m_moveApplicationPortWidget;

    /// Retrieve method
    QPointer< QComboBox > m_retrieveMethodWidget;

    /// Test button
    QPointer< QPushButton > m_pingPacsButtonWidget;

};

} // namespace editor
} // namespace common
} // namespace ioDicomExt

#endif // __IODICOMEXT_COMMON_EDITOR_SPACSCONFIGURATIONEDITOR_HPP__
