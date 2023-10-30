/************************************************************************
 *
 * Copyright (C) 2014-2023 IRCAD France
 * Copyright (C) 2014-2019 IHU Strasbourg
 *
 * This file is part of Sight.
 *
 * Sight is free software: you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Sight is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with Sight. If not, see <https://www.gnu.org/licenses/>.
 *
 ***********************************************************************/

#pragma once

#include "modules/ui/viz/config.hpp"

#include <ui/__/editor.hpp>

#include <viz/scene3d/layer.hpp>

#include <QLabel>
#include <QObject>
#include <QPointer>

class QAbstractButton;
class QButtonGroup;
class QComboBox;
class QRadioButton;
class QLineEdit;
class QSlider;
class QCheckBox;
class QComboBox;
// needed for the radius
class QDoubleSpinBox;
// needed for the samples number
class QSpinBox;

namespace sight::module::ui::viz
{

/**
 * @brief   Allows to select an Ogre Compositor and apply it to a layer
 */
class MODULE_UI_VIZ_CLASS_API core_compositor_editor : public QObject,
                                                       public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(core_compositor_editor, sight::ui::editor);

    /// Constructor.
    MODULE_UI_VIZ_API core_compositor_editor() noexcept;

    /// Destructor. Does nothing
    MODULE_UI_VIZ_API ~core_compositor_editor() noexcept override;

protected:

    /**
     * @brief method description:
     * @code{.xml}
        <service uid="core_compositor_editorInstance" type="sight::module::ui::viz::core_compositor_editor">
             <parameter>param</parameter>
        </service>
       @endcode
     * - \b Parameter : parameter description.
     */
    MODULE_UI_VIZ_API void configuring() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void starting() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void stopping() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void updating() override;

    /// FILL ME.
    MODULE_UI_VIZ_API void refresh_renderers();

    std::vector<sight::viz::scene3d::layer::wptr> m_layers;
    sight::viz::scene3d::layer::wptr m_current_layer;

protected Q_SLOTS:

    void on_selected_layer_item(int _index);
    void on_edit_transparency_depth(int _depth);
    void on_edit_transparency(int _index);

private:

    QPointer<QButtonGroup> m_transparency_button_group;
    QPointer<QButtonGroup> m_transparency_button_group_shading;
    QPointer<QButtonGroup> m_normals_radio_box;
    QPointer<QComboBox> m_layers_box;
    QPointer<QSlider> m_transparency_depth_slider;
    QPointer<QComboBox> m_combo_box;
    QRadioButton* m_button_default {};
    QRadioButton* m_button_depth_peeling {};
    QRadioButton* m_button_dual_depth_peeling {};
    QRadioButton* m_button_weighted_blended_oit {};
    QRadioButton* m_button_hybrid_transparency {};
    QRadioButton* m_button_cel_shading_depth_peeling {};
    QLabel* m_label_default {};
    QLabel* m_label_depth_peeling {};
    QLabel* m_label_dual_depth_peeling {};
    QLabel* m_label_weighted_blended_oit {};
    QLabel* m_label_hybrid_transparency {};
    QLabel* m_label_cel_shading_depth_peeling {};
    bool m_is_layer_selected {};
};

} // namespace sight::module::ui::viz
