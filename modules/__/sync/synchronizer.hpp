/************************************************************************
 *
 * Copyright (C) 2022-2023 IRCAD France
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

#include "modules/sync/config.hpp"

#include <core/thread/timer.hpp>

#include <data/frame_tl.hpp>
#include <data/image.hpp>
#include <data/matrix4.hpp>
#include <data/matrix_tl.hpp>

#include <service/synchronizer.hpp>

namespace sight::module::sync
{

/**
 * @brief The synchronizer service synchronizes video frames and/or transform matrices.
 * This service takes a set of frames and matrices timelines.
 * The timelines contents are synchronized when some data are added, and when the synchronization is required,
 * which can be automated or through slot.
 * Once synchronized, the timeline data are pushed in output variables, which are associated through the configuration.
 *
 * @section Signals Signals
 * - \b synchronization_done(core::clock::type): Emitted when a synchronization has been done, and forward
 * the synchronization timestamp.
 * - \b synchronization_skipped(): Emitted when the synchronization is not done, either because the timelines are all
 * empty, or because the synchronization timestamp has not changed.
 * - \b frameSynchronized(int): Emitted when a frame with sendStatus at true is synchronized while it wasn't previously.
 * - \b frameUnsynchronized(int): Emitted when a frame with sendStatus at true is unsynchronized while it wasn't
 * previously.
 * - \b matrix_synchronized(int): Emitted when a matrix with sendStatus at true is synchronized while it wasn't
 * previously.
 * - \b matrix_unsynchronized(int): Emitted when a matrix with sendStatus at true is unsynchronized while it wasn't
 * previously.
 *
 * @section Slots Slots
 * - \b synchronize() : synchronizes the timelines.
 * - \b reset() : reset the previous synchronization timestamp to 0.
 * - \b setFrameBinding( std::size_t, unsigned int, std::size_t) : change the association of a frame output var
 * with a TL and the element index in the timeline.
 * - \b setMatrixBinding( std::size_t, unsigned int, std::size_t) : change the association of a matrix output var
 * with a TL and the element index in the timeline.
 * - \b set_delay(int, std::string): Apply delay between frames and matrices
 *  The key needs to be either frameDelay_i or matrixDelay_i where i is the index of the timeline in the input vector
 *  The value should be a positive value.
 *  The delay will be applied to the corresponding timeline. Once synchronized, the effective frame/matrix
 *  picked by the algorithm will be in the past with a temporal offset of delay.
 *
 * @section XML XML Configuration
 *
 * @code{.xml}
    <service type="sight::module::sync::synchronizer" auto_connect="true">
        <in group="frameTL">
            <key uid="frameTL1" />
            <key uid="frameTL4" delay="36" />
            <key uid="frameTL6" />
        </in>
        <inout group="frames">
            <key uid="frame1" sendStatus="true" />
            <key uid="frame6" tl="2" />
            <key uid="frame4" tl="1" sendStatus="false"/>
            <key uid="frame11" tl="0"  sendStatus="true" />
        </inout>
        <in group="matrixTL">
            <key uid="matrixTL1" />
            <key uid="matrixTL2" delay="56" />
        </in>
        <inout group="matrix">
            <key uid="matrix0" index="1" sendStatus="true" >
            <key uid="matrix1" />
            <key uid="matrix2" tl="1" index="0" sendStatus="false"/>
            <key uid="matrix3" tl="1" index="1"/>
            <key uid="matrix4" tl="0" index2"/>
        </inout>
        <tolerance>500</tolerance>
    </service>
   @endcode
 *
 * For more configurations samples, please have a look at synchronizerTest.
 *
 * @subsection Input Input
 * - \b frameTL [sight::data::frame_tl]: defines the frameTL to synchronize.
 *  each frameTL can have an optional attribute, which is the delay to apply to the timeline.
 * - \b matrixTL [sight::data::matrix_tl]: defines the frameTL to synchronize.
 *  each matrixTL can have an optional attribute, which is the delay to apply to the timeline.
 *
 * @subsection In-Out In-Out
 * - \b frames [sight::data::image]: defines the images where to extract the image.
 *  each frame can have an optional attribute:
 *    - tl : the index of the tl from which the data are taken to populate the frame variable (default: 0).
 *    - index: the element index, in the tl, from which the data are taken to populate the frame variable (default: 0).
 *    - sendStatus: a boolean to specify if a signal should be send when the variable synchronization state changes
 *(default: false).
 * - \b matrix [sight::data::matrix4]: defines the matrix where to extract the image.
 *  each frame can have an optional attribute:
 *    - tl : the index of the tl from which the data are taken to populate the frame variable (default: 0).
 *    - index: the element index, in the tl, from which the data are taken to populate the frame variable (default: 0).
 *    - sendStatus: a boolean to specify if a signal should be sent when the variable synchronization state changes
 *(default: false).
 *
 * @subsection Configuration Configuration
 * - \b legacyAutoSync : defines if the service should run the synchronization automatically (every 15 ms) or if it is
 * handled through slots. This option is only to support previous configurations, and
 * should not be set to "true" in new configurations (default: false).
 * - \b tolerance : defines the maximum distance between two frames (default: 500).
 *      If a timeline exceeds this tolerance it will not be synchronized (default: true).
 */
class MODULE_SYNC_CLASS_API synchronizer final : public service::synchronizer
{
public:

    SIGHT_DECLARE_SERVICE(synchronizer, service::synchronizer);

    /// Internal wrapper holding signals keys.
    struct signals
    {
        using key_t = sight::core::com::signals::key_t;
        static inline const key_t SYNCHRONIZATION_DONE_SIG    = "synchronization_done";
        static inline const key_t SYNCHRONIZATION_SKIPPED_SIG = "synchronization_skipped";
        static inline const key_t FRAME_SYNCHRONIZED_SIG      = "frameSynchronized";
        static inline const key_t FRAME_UNSYNCHRONIZED_SIG    = "frameUnsynchronized";
        static inline const key_t MATRIX_SYNCHRONIZED_SIG     = "matrix_synchronized";
        static inline const key_t MATRIX_UNSYNCHRONIZED_SIG   = "matrix_unsynchronized";

        using timestamp_signal_t = sight::core::com::signal<void (core::clock::type _timestamp)>;
        using void_signal_t      = sight::core::com::signal<void ()>;
        using int_signal_t       = sight::core::com::signal<void (int)>;
    };

    /// Internal wrapper holding slots keys.
    struct slots
    {
        using key_t = sight::core::com::slots::key_t;
        static inline const key_t RESET_TIMELINE_SLOT         = "reset";
        static inline const key_t SYNCHRONIZE_SLOT            = "synchronize";
        static inline const key_t SET_FRAME_BINDING_SLOT      = "setFrameBinding";
        static inline const key_t SET_MATRIX_BINDING_SLOT     = "setMatrixBinding";
        static inline const key_t SET_DELAY_SLOT              = "set_delay";
        static constexpr std::string_view FRAME_DELAY_PREFIX  = "frameDelay_";
        static constexpr std::string_view MATRIX_DELAY_PREFIX = "matrixDelay_";
    };

    /// Internal wrapper holding configuration keys.
    struct config_key
    {
        static inline const std::string OUTVAR_TL_INDEX      = "<xmlattr>.tl";
        static inline const std::string OUTVAR_ELEMENT_INDEX = "<xmlattr>.index";
        static inline const std::string OUTVAR_SEND_STATUS   = "<xmlattr>.sendStatus";
        static inline const std::string TL_DELAY             = "<xmlattr>.delay";
        static inline const std::string KEY                  = "key";

        static inline const std::string FRAMETL_INPUT     = "frameTL";
        static inline const std::string FRAME_INOUT       = "frames";
        static inline const std::string MATRIXTL_INPUT    = "matrixTL";
        static inline const std::string MATRIX_INOUT      = "matrix";
        static inline const std::string TOLERANCE         = "tolerance";
        static inline const std::string LEGACY_AUTO_SYNCH = "legacyAutoSync";
    };

    /// Internal wrapper used for out variable association with TLs
    struct out_var_parameter
    {
        std::size_t out_var_index {0};
        std::size_t tl_index {0};
        unsigned int tl_element_index {0};
        bool is_synchronized {false};
        bool signal_synchronization {false};
        int delay {0};
    };

    /**
     * @brief Constructor.
     */
    MODULE_SYNC_API synchronizer();

    /**
     * @brief Destructor.
     */
    MODULE_SYNC_API ~synchronizer() final = default;

    /**
     * @brief Return proposals to connect service slots to associated object signals,
     * this method is used for obj/srv auto connections
     *
     * Connect data::timeline::signals::CLEARED to RESET_TIMELINE_SLOT
     */
    MODULE_SYNC_API service::connections_t auto_connections() const final;

protected:

    /**
     * @brief This method is used to configure the service.
     */
    MODULE_SYNC_API void configuring() final;

    /**
     * @brief This method is used to start the service.
     */
    MODULE_SYNC_API void starting() final;

    /**
     * @brief This method is used to stop the service.
     */
    MODULE_SYNC_API void stopping() final;

    /**
     * @brief Called when new objects are pushed into the timeline.
     */
    MODULE_SYNC_API void updating() final;

    /**
     * @brief SLOT: Synchronizes the TLs, fill the output variables, and send notifications
     */
    MODULE_SYNC_API void synchronize();

    /**
     * @brief SLOT: Resets the last timestamp stored
     */
    MODULE_SYNC_API void reset_timeline();

    /**
     * @brief SLOT: Changes the association between a frame timeline, its element index and the outputVar
     *
     * @param _tlIndex : the new timeline index in the configuration
     * @param _elementIndex : the element index in the configuration
     * @param _outputVarIndex : the output var to update
     */
    MODULE_SYNC_API void set_frame_binding(
        std::size_t _tl_index,
        unsigned int _element_index,
        std::size_t _output_var_index
    );

    /**
     * @brief SLOT: Changes the association between a matrix timeline, its element index and the outputVar
     *
     * @param _tlIndex : the new timeline index in the configuration
     * @param _elementIndex : the element index in the configuration
     * @param _outputVarIndex : the output var to update
     */
    MODULE_SYNC_API void set_matrix_binding(
        std::size_t _tl_index,
        unsigned int _element_index,
        std::size_t _output_var_index
    );

    /** Set a delay between frames and matrices.
     *  The key needs to be either frameDelay_i or matrixDelay_i where i is the index of the timeline in the input
     * vector
     *  The value should be a positive value.
     *  The delay will be applied to the corresponding timeline. Once synchronized, the effective frame/matrix
     *  picked by the algorithm will be in the past with a temporal offset of delay.
     */

    /**
     * @brief SLOT: Set a delay between frames and matrices.
     *  The delay will be applied to the corresponding timeline. Once synchronized, the effective frame/matrix
     *  picked by the algorithm will be in the past with a temporal offset of delay.
     *
     * @param val : value to set. Should be a positive value.
     * @param key : needs to be either frameDelay_i or matrixDelay_i
     * where i is the index of the timeline in the input vector
     */
    MODULE_SYNC_API void set_delay(int _val, std::string _key);

private:

    /**
     * @brief Get the index of the frame output vars, associated to the given TL
     *
     * @param frameTLIndex : the index of the frameTL to treat
     */
    std::vector<out_var_parameter> get_frame_tl_output_var_index(std::size_t _frame_tl_index);

    /**
     * @brief Copy the synchronized frame from the tl toward the corresponding output variables.
     *
     * @param frameTLIndex : the index of the frameTL to treat
     * @param synchronizationTimestamp : the synchronization timestamp
     */
    void copy_frame_from_t_lto_output(
        std::size_t _frame_tl_index,
        core::clock::type _synchronization_timestamp
    );

    /**
     * @brief Get the index of the matrix output vars, associated to the given TL
     *
     * @param matrixTLIndex : the index of the matrixTL to treat
     */
    std::vector<out_var_parameter> get_matrix_tl_output_var_index(std::size_t _matrix_tl_index);

    /**
     * @brief Copy the synchronized matrix from the tl toward the corresponding output variables.
     *
     * @param matrixTLIndex : the index of the matrixTL to treat
     * @param synchronizationTimestamp : the synchronization timestamp
     */
    void copy_matrix_from_t_lto_output(
        std::size_t _matrix_tl_index,
        core::clock::type _synchronization_timestamp
    );

    /**
     * @brief send signals for the frame variables which required it, each time that the synchronization state changes
     *
     * @param synchFrameTLIndex : vector of index of the frameTl which have just been synchronized
     */
    void send_frame_var_status(const std::vector<std::size_t>& _synch_frame_tl_index);

    /**
     * @brief send signals for the frame variables which required it, each time that the synchronization state changes
     *
     * @param synchFrameTLIndex : vector of index of the matrixTl which have just been synchronized
     */
    void send_matrix_var_status(const std::vector<std::size_t>& _synch_matrix_tl_index);

    /// @brief vector which store the input/output variable associations for frames and matrix
    std::vector<out_var_parameter> m_frame_out_var_parameters {};
    std::vector<out_var_parameter> m_matrix_out_var_parameters {};

    /// @brief vector which store the delay to apply to each timeline
    std::vector<int> m_matrix_tl_delay;
    std::vector<int> m_frame_tl_delay;

    ///Synchronisation timing mechanism has two possible behaviour differentiated through m_legacyAutoSync
    // => m_legacyAutoSync = true
    //    The synchronisation is expected to be called automatically.
    //    The duration between two synchronizations is stored in m_timeStep.
    //    As this is an automated internal mechanism, and to prevent the confusion with grabbing
    //    framerate, this value should not be exposed.
    //    To do this, a timer mechanism is started to synchronize at the given frequency.
    //
    //    THIS SHOULD NOT BE USED IN NEW CONFIGURATION.
    //
    //=> m_legacyAutoSync = false
    //    The synchronisation is done when it is called through a slot
    //    This allows to have a stronger control on the frame/matrix dispatching rate
    //
    // For both cases, the synchronization is done when the synch method is called,
    // and when some data have been updated.
    // The coordination with the data arrival is handled through an updateMask.
    // The mask changes value when a data is updated (through the updating and autoConnection )
    // And it changes back when the synchronisation method is called.

    // specify if the synchronization is called automatically or only when the slot synchronize is called.
    bool m_legacy_auto_sync {false};

    // enum values for the update/synchronize mask
    enum : std::uint8_t
    {
        object_received = 0x1,
        sync_requested  = 0x2
    };
    std::uint8_t m_update_mask {sync_requested};

    /// Timer used for the update
    core::thread::timer::sptr m_timer;

    /// Timer worker thread
    core::thread::worker::sptr m_worker;

    /// Time step used for the update, in ms
    unsigned int m_time_step {15};

    /// Tolerance to take into account matrix
    core::clock::type m_tolerance {500.};

    core::clock::type m_last_time_stamp {0.};

    /// Contains the input video timelines.
    data::ptr_vector<data::frame_tl, data::access::in> m_frame_t_ls {this, config_key::FRAMETL_INPUT, true};

    /// Contains the output images.
    data::ptr_vector<data::image, data::access::inout> m_frames {this, config_key::FRAME_INOUT, false};

    /// Contains the input matrix timelines.
    data::ptr_vector<data::matrix_tl, data::access::in> m_matrix_t_ls {this, config_key::MATRIXTL_INPUT, true};

    /// Contains the output matrices.
    data::ptr_vector<data::matrix4, data::access::inout> m_matrix {this, config_key::MATRIX_INOUT, false};
};

} // namespace sight::module
// namespace sight::module::sync
