/************************************************************************
 *
 * Copyright (C) 2009-2024 IRCAD France
 * Copyright (C) 2012-2019 IHU Strasbourg
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

#include <data/map.hpp>
#include <data/matrix4.hpp>
#include <data/model_series.hpp>
#include <data/reconstruction.hpp>

#include <ui/__/editor.hpp>

#include <QObject>
#include <QPointer>

#include <map>
#include <string>

class QListWidget;
class QPushButton;
class QListWidgetItem;
class QComboBox;
class QCheckBox;

namespace sight::module::ui::qt::model
{

/**
 * @brief Display the organs list and allow an interactive selection to set the corresponding meshes in a map
 * @section XML XML Configuration
 * @code{.xml}
   <service type="sight::module::ui::qt::model::organ_transformation">
       <inout key="modelSeries" uid="..." />
       <inout key="map" uid="..." />
   </service>
   @endcode
 * @subsection InOut InOut
 * - \b modelSeries [sight::data::model_series]: modelSeries to modify.
 * - \b map [sight::data::map]: map.
 */
class organ_transformation : public QObject,
                             public sight::ui::editor
{
Q_OBJECT

public:

    SIGHT_DECLARE_SERVICE(organ_transformation, sight::ui::editor);

    /// constructor
    organ_transformation() noexcept;
    /// destructor
    ~organ_transformation() noexcept override;

protected:

    void configuring() override;
    void starting() override;
    void stopping() override;
    void updating() override;
    void info(std::ostream& _stream) override;

    /**
     * @brief Returns proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connection
     *
     * Connect ModelSeries::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_ADDED_SIG to this::service::slots::UPDATE
     * Connect ModelSeries::RECONSTRUCTIONS_REMOVED_SIG to this::service::slots::UPDATE
     * Connect Map::MODIFIED_SIG to this::service::slots::UPDATE
     * Connect Map::ADDED_OBJECTS_SIG to this::service::slots::UPDATE
     * Connect Map::REMOVED_OBJECTS_SIG to this::service::slots::UPDATE
     * Connect Map::CHANGED_OBJECTS_SIG to this::service::slots::UPDATE
     */
    connections_t auto_connections() const override;

private Q_SLOTS:

    void on_reconstruction_check(QListWidgetItem* _current_item);
    void on_reset_click();
    void on_save_click();
    void on_load_click();

    /// Slot to check all the organs of the list
    void on_select_all_changed(int _state);

private:

    void refresh();
    static void notify_transformation_matrix(data::matrix4::sptr _a_trans_mat);

    /// Create the transformation in mesh field. This field is used in the adaptors to transform the mesh
    void add_mesh_transform();

    // reconstruction_map_t
    using reconstruction_map_t = std::map<std::string, data::reconstruction::sptr>;
    using inner_mat_mapping_t  = std::map<std::string, data::matrix4::sptr>;
    using save_mapping_t       = std::map<std::string, inner_mat_mapping_t>;

    reconstruction_map_t m_reconstruction_map;
    QPointer<QPushButton> m_save_button;
    QPointer<QPushButton> m_load_button;
    QPointer<QPushButton> m_reset_button;
    QPointer<QListWidget> m_reconstruction_list_box;
    QPointer<QComboBox> m_save_selection_combo_box;
    QPointer<QCheckBox> m_select_all_check_box;

    //variables for the functionalities of saving & loading
    save_mapping_t m_save_listing;
    unsigned int m_save_count {0};

    static constexpr std::string_view MODEL_SERIES = "modelSeries";
    static constexpr std::string_view MAP          = "map";
    data::ptr<data::model_series, data::access::inout> m_model_series {this, "modelSeries"};
    data::ptr<data::map, data::access::inout> m_map {this, "map"};
};

} // namespace sight::module::ui::qt::model
