/* ***** BEGIN LICENSE BLOCK *****
 * FW4SPL - Copyright (C) IRCAD, 2009-2018.
 * Distributed under the terms of the GNU Lesser General Public License (LGPL) as
 * published by the Free Software Foundation.
 * ****** END LICENSE BLOCK ****** */

#pragma once

#include "uiVisuQt/config.hpp"

#include <fwData/Point.hpp>

#include <fwDataTools/PickingInfo.hpp>

#include <fwGui/editor/IEditor.hpp>

#include <fwTools/Failed.hpp>

#include <QLineEdit>
#include <QObject>
#include <QPointer>

namespace uiVisuQt
{

/**
 * @brief   PointEditor service allows to display point information.
 */
class UIVISUQT_CLASS_API PointEditor : public QObject,
                                       public ::fwGui::editor::IEditor
{
Q_OBJECT

public:

    fwCoreServiceClassDefinitionsMacro( (PointEditor)(::fwGui::editor::IEditor) );

    /// Constructor. Do nothing.
    UIVISUQT_API PointEditor() noexcept;

    /// Destructor. Do nothing.
    UIVISUQT_API virtual ~PointEditor() noexcept;

protected:

    typedef ::fwRuntime::ConfigurationElement::sptr Configuration;

    ///This method launches the IEditor::starting method.
    virtual void starting() override;

    ///This method launches the IEditor::stopping method.
    virtual void stopping() override;

    void updating() override;

    void swapping() override;

    void configuring() override;

    /// Overrides
    virtual void info( std::ostream& _sstream ) override;

private:

    /// Slot: get the interaction information
    void getInteraction(::fwDataTools::PickingInfo info);

    QPointer< QLineEdit >   m_textCtrl_x;
    QPointer< QLineEdit >   m_textCtrl_y;
    QPointer< QLineEdit >   m_textCtrl_z;

};

} // uiData
