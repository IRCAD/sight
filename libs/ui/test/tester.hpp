/************************************************************************
 *
 * Copyright (C) 2021-2025 IRCAD France
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

/// This header defines the GUI tester class and other helper classes to ease the creating of GUI tests.

#pragma once

#include <QTest>
#include <QWidget>

#include <thread>
#include <filesystem>

#include <sight/ui/test/config.hpp>

#include "interaction.hpp"

namespace sight::ui::test
{

/// An event which has a function as a payload. Useful to execute arbitrary code on the main GUI thread.
class test_event : public QEvent
{
public:

    SIGHT_UI_TEST_API test_event(std::function<void()> _f);
    std::function<void()> function();

private:

    std::function<void()> m_function;
};

/// An object which filters the previous TestEvent on an object to execute its payload.
class test_event_filter : public QObject
{
Q_OBJECT

protected:

    bool eventFilter(QObject* _obj, QEvent* _event) override;
};

/// The exception which is thrown when a GUI test fails.
class tester_assertion_failed : public std::runtime_error
{
public:

    tester_assertion_failed(const std::string& _msg);
};

/// A function which always return true, ignoring its parameter.
SIGHT_UI_TEST_API bool always_true(QObject* /*unused*/);

/// The main class of the GUI test library, which allows creating GUI tests.
class SIGHT_UI_TEST_CLASS_API tester
{
public:

    class SIGHT_UI_TEST_CLASS_API backtrace_lock
    {
    public:

        SIGHT_UI_TEST_API backtrace_lock(tester& _tester);
        SIGHT_UI_TEST_API ~backtrace_lock();

    private:

        tester& m_tester;
    };

    /// Constructor. Initializes the test name.
    SIGHT_UI_TEST_API tester(std::string _test_name, bool _verbose_mode = false);

    /// Destructor. Joins the thread.
    SIGHT_UI_TEST_API ~tester();

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component A function which returns the new graphic component.
     * @param _condition The condition the new graphic component must respect.
     * @param _timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void take(
        const std::string& _component_description,
        std::function<T()> _graphic_component,
        std::function<bool(T)> _condition = always_true,
        int _timeout                      = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void take(
        const std::string& _component_description,
        std::function<QObject* ()> _graphic_component,
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component The new graphic component.
     */
    SIGHT_UI_TEST_API void take(const std::string& _component_description, QObject* _graphic_component);

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _parent The parent of the new current graphic component, typically a window. Default is the main window.
     * @param _object_name The objectName of the new current graphic component to be found.
     *
     * @{
     */
    SIGHT_UI_TEST_API void take(
        const std::string& _component_description,
        QObject* _parent,
        const std::string& _object_name,
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void take(
        const std::string& _component_description,
        const std::string& _object_name,
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Interacts with the current graphic component.
     *
     * @param _interaction The interaction that will be used on the current graphic component.
     *
     * @warning The interaction must be allocated on the heap. Guitester will take care of freeing the interaction, it
     * is unsafe to try to access the interaction after this method is called.
     */
    SIGHT_UI_TEST_API void interact(std::unique_ptr<interaction>&& _interaction);

    /**
     * @brief Check if the assertion is true.
     *
     * @param _result_description The description of the expected result. Used in failure messages.
     * @param _result The assertion, which must be true for the test to pass.
     * @param _timeout The max time to wait for the assertion to become true before the test fails.
     *
     * @note If the template is used, the current component will be casted before being used in result.
     *
     * @{
     */
    template<typename T>
    void doubt(
        const std::string& _result_description,
        std::function<bool(T)> _result,
        int _timeout = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void doubt(
        const std::string& _result_description,
        std::function<bool(QObject*)> _result,
        int _timeout = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the graphicComponent function get access to the old graphic component. It also
     * slightly changes the failure messages.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component A function which returns the new graphic component and takes the old graphic component
     * @param _condition The condition the new graphic component must respect.
     * @param _timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void yields(
        const std::string& _component_description,
        std::function<T(QObject*)> _graphic_component,
        std::function<bool(T)> _condition = always_true,
        int _timeout                      = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void yields(
        const std::string& _component_description,
        std::function<QObject* (QObject*)> _graphic_component,
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the graphicComponent is found inside the current graphic component tree. It also
     * slightly changes the failure messages.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _object_name The objectName of the new graphic component to be found.
     *
     * @note The template version narrows the search to the components compatible with that type.
     *
     * @{
     */
    template<typename T>
    void yields(
        const std::string& _component_description,
        const std::string& _object_name          = "",
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void yields(
        const std::string& _component_description,
        const std::string& _object_name          = "",
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the test doesn't fail if the component doesn't appear, instead the following commands
     * will be ignored, until the next @ref take.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component A function which returns the new graphic component and takes the old graphic component
     * @param _condition The condition the new graphic component must respect.
     * @param _timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void maybe_take(
        const std::string& _component_description,
        std::function<T()> _graphic_component,
        std::function<bool(T)> _condition = always_true,
        int _timeout                      = DEFAULT_TIMEOUT
    );
    SIGHT_UI_TEST_API void maybe_take(
        const std::string& _component_description,
        std::function<QObject* ()> _graphic_component,
        std::function<bool(QObject*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Calls the function with the current component, in order to cause side-effects.
     *
     * @param _f The function to be called, which will get the current component.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in f.
     *
     * @{
     */
    template<typename T>
    void do_something(std::function<void(T)> _f);
    SIGHT_UI_TEST_API void do_something(std::function<void(QObject*)> _f);
    /// @}

    /**
     * @brief Like @ref do_something, but the function is called asynchronously, on the main GUI thread.
     *
     * @param _f The function to be called asynchronously, which will get the current component.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in f.
     *
     * @{
     */
    template<typename T>
    void do_something_asynchronously(std::function<void(T)> _f);
    SIGHT_UI_TEST_API void do_something_asynchronously(std::function<void(QObject*)> _f);
    /// @}

    /**
     * @brief Takes a screenshot of the current graphic component
     * @param _path Where to save the screenshot
     */
    SIGHT_UI_TEST_API void take_screenshot(const std::filesystem::path& _path);

    /**
     * @brief Starts the test.
     * @details It will create a thread which will wait for the main window to appear, call f, then try to close the
     * main window, and finally wait for the main window to disappear. Code in the thread is surrounded with a
     * try/catch, as to not catch exceptions in a thread causes process termination in C++.
     *
     * @param _f The function to be called.
     *
     * @pre @ref init was called.
     */
    SIGHT_UI_TEST_API void start(std::function<void()> _f);

    /**
     * @brief Check if the specified component doesn't exist or exists but is hidden, the test fails else.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component A function which returns the new graphic component and takes the old graphic component
     * @param _condition An additional condition the new graphic component must respect.
     * @param _timeout The max time to wait for the component.
     *
     * @return Is the component is hidden?
     *
     * @warning The current graphic component becomes undefined after the call of this method.
     */
    SIGHT_UI_TEST_API void should_be_hidden(
        const std::string& _component_description,
        std::function<QWidget* ()> _graphic_component,
        std::function<bool(QWidget*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );

    /**
     * @brief Check if the specified component exist and is visible, the test fails else. The current graphic component
     * becomes this one after the call.
     *
     * @param _component_description The description of the new graphic component. Used in failure messages.
     * @param _graphic_component A function which returns the new graphic component and takes the old graphic component
     * @param _condition An additional condition the new graphic component must respect.
     * @param _timeout The max time to wait for the component.
     *
     * @return Is the component present?
     */
    SIGHT_UI_TEST_API void should_be_present(
        const std::string& _component_description,
        std::function<QWidget* ()> _graphic_component,
        std::function<bool(QWidget*)> _condition = always_true,
        int _timeout                             = DEFAULT_TIMEOUT
    );

    /**
     * @brief Check if there is a current component. Useful to check if a @ref maybe_take succeeded.
     *
     * @return Is there a current component?
     */
    SIGHT_UI_TEST_API bool exists();

    /**
     * @brief Check if the current component is castable to a given type.
     *
     * @return Is the current component castable to the given type?
     */
    template<typename T>
    bool is_a();

    /**
     * @brief Get the current component.
     *
     * @return The casted current component.
     *
     * @note If there is no current component (because no take were ever called, or after a failed @ref maybe_take), or
     * if the cast fails, the test fails.
     */
    template<typename T>
    T get();

    /**
     * @brief Get the main window
     *
     * @returns The main window
     */
    SIGHT_UI_TEST_API QWidget* get_main_window();

    /**
     * @brief Returns the failure message.
     *
     * @returns The failure message.
     *
     * @pre failed() must return false.
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::string get_failure_message() const;

    /**
     * @brief Returns true if the test failed and false otherwise.
     *
     * @returns Did the test fail?
     */
    [[nodiscard]] SIGHT_UI_TEST_API bool failed() const;

    [[nodiscard]] SIGHT_UI_TEST_API backtrace_lock add_in_backtrace(const std::string& _description);

    /**
     * @brief Returns the description associated to the current graphic component.
     *
     * @returns Current graphic component description
     */
    [[nodiscard]] SIGHT_UI_TEST_API std::string get_description() const;

    /**
     * @brief Properly initializes Guitester.
     * @details It installs an event filter on the QApplication (needed to execute interactions asynchronously) and sets
     * an environment variable so that the remainder of the library knows that GUI tests are running (for now, only
     * needed to show up the Qt file dialog instead of the native one).
     */
    SIGHT_UI_TEST_API static void init();

    /**
     * @brief Helper method that returns the associated widget of a QAction.
     * @details Some components of the GUI are QAction, which are abstract and not directly interactable. This method
     * exists to easily get an interactable widget associated with the QAction.
     *
     * @param _action The action for which the associated widget is needed.
     *
     * @note For convenience, if action is nullptr, this method returns nullptr.
     */
    SIGHT_UI_TEST_API static QWidget* get_widget_from_action(QAction* _action);

    /**
     * @brief Move the mouse on a component, alternative to QTest::mouseMove as a workaround to QTBUG-5232.
     *
     * @param _component The component on which move the mouse
     * @param _pos The position where the mouse will be, in coordinates relative to the component.
     * @param _delay The delay before executing this interaction.
     * @param _button The button which clicked while moving
     * @param _modifiers The modifiers which are active while moving
     *
     * @{
     */
    SIGHT_UI_TEST_API static void mouse_move(
        QWidget* _component,
        QPoint _pos                      = QPoint(),
        int _delay                       = -1,
        Qt::MouseButton _button          = Qt::NoButton,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier
    );
    SIGHT_UI_TEST_API static void mouse_move(
        QWindow* _component,
        QPoint _pos                      = QPoint(),
        int _delay                       = -1,
        Qt::MouseButton _button          = Qt::NoButton,
        Qt::KeyboardModifiers _modifiers = Qt::NoModifier
    );
    /// @}

    /**
     * @brief Compare images by comparing pixel by pixel.
     * @details Returns the percentage of similar pixels between the two images.
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @param _strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize
     * the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_pixel_perfect(QImage _a, QImage _b, bool _strict = false);

    /**
     * @brief Compare images using the Mean Square Error.
     * @details The MSE is normalized so that its value is between 0 and 1.
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @param _strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize
     * the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_mse(QImage _a, QImage _b, bool _strict = false);

    /**
     * @brief Compare images by computing the cosine of the angle between the matrices of the two images.
     * @details Extremely sensible to big color changes.
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @param _strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize
     * the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_cosine(QImage _a, QImage _b, bool _strict = false);

    /**
     * @brief Compare images by computing an histogram.
     * @details May be used with images of different size, but totally ignore the position of the pixels.
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_histogram(const QImage& _a, const QImage& _b);

    /**
     * @brief Compare images by computing Spearman's correlation coefficient of the two matrices of the two images.
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_correlation(QImage _a, QImage _b, bool _strict = false);

    /**
     * @brief Compare images by using the MSE on normalized, scaled-down, rotated and flipped version of the images.
     * @details Comes from https://stackoverflow.com/a/26061
     *
     * @param _a    first image to compare
     * @param _b    second image to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    SIGHT_UI_TEST_API static double compare_images_voodoo(const QImage& _a, const QImage& _b);

    /**
     * @brief Respectively returns the center, the left, the right, the top and the bottom of a widget.
     *
     * @param _widget The widget for which compute the position
     * @returns The position
     *
     * @{
     */
    SIGHT_UI_TEST_API static QPoint center_of(const QWidget* _widget);
    SIGHT_UI_TEST_API static QPoint left_of(const QWidget* _widget);
    SIGHT_UI_TEST_API static QPoint right_of(const QWidget* _widget);
    SIGHT_UI_TEST_API static QPoint top_of(const QWidget* _widget);
    SIGHT_UI_TEST_API static QPoint bottom_of(const QWidget* _widget);
    /// @}

    /**
     * @brief Returns the folder where the images generated by the test should be stored.
     * @details Equals to the value of the IMAGE_OUTPUT_PATH environment variable, or the system temporary folder (such
     * as /tmp on UNIXes) if this environment variable is empty.
     */
    SIGHT_UI_TEST_API static std::filesystem::path get_image_output_path();

    /**
     * @brief Returns a dummy touch screen to emulate touch events
     */
    static QTouchDevice* get_dummy_touch_screen();

    /// The default timeout as used in the methods @ref take, @ref doubt, @ref yields and @ref maybe_take
    static const int DEFAULT_TIMEOUT = 10000;

private:

    static SIGHT_UI_TEST_API void fail(const std::string& _message);
    std::string generate_failure_message();

    static QVector<QVector<QVector<double> > > compute_histogram(const QImage& _img);
    static QImage normalize(QImage _img);
    static QImage voodooize(const QImage& _img);
    SIGHT_UI_TEST_API static bool wait_for_asynchronously(
        std::function<bool()> _predicate,
        int _timeout = DEFAULT_TIMEOUT
    );

    QPointer<QObject> m_graphic_component = nullptr;
    std::string m_component_description;
    std::vector<std::unique_ptr<interaction> > m_interactions;
    bool m_failed = false;
    std::thread m_thread;
    QPointer<QWidget> m_main_window;
    std::string m_failure_message;
    std::string m_result_description;
    const std::string m_test_name;
    bool m_verbose_mode;
    std::vector<std::string> m_backtrace;

    static bool s_already_loaded;
    static std::filesystem::path s_image_output_path;
};

} // namespace sight::ui::test

#include "tester.hxx"
