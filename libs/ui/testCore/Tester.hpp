/************************************************************************
 *
 * Copyright (C) 2021-2023 IRCAD France
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

/// This header defines the GUI Tester class and other helper classes to ease the creating of GUI tests.

#pragma once

#include <QEvent>
#include <QObject>
#include <Qt>
#include <QTest>
#include <QWidget>

#include <iostream>
#include <thread>
#include <filesystem>

#include "ui/testCore/config.hpp"

#include "Interaction.hpp"

namespace sight::ui::testCore
{

/// An event which has a function as a payload. Useful to execute arbitrary code on the main GUI thread.
class TestEvent : public QEvent
{
public:

    GUITEST_API TestEvent(std::function<void()> f);
    std::function<void()> function();

private:

    std::function<void()> m_function;
};

/// An object which filters the previous TestEvent on an object to execute its payload.
class TestEventFilter : public QObject
{
Q_OBJECT

protected:

    bool eventFilter(QObject* obj, QEvent* event) override;
};

/// The exception which is thrown when a GUI test fails.
class TesterAssertionFailed : public std::runtime_error
{
public:

    TesterAssertionFailed(const std::string& msg);
};

/// A function which always return true, ignoring its parameter.
GUITEST_API bool alwaysTrue(QObject* /*unused*/);

/// The main class of the GUI test library, which allows creating GUI tests.
class GUITEST_CLASS_API Tester
{
public:

    class GUITEST_CLASS_API BacktraceLock
    {
    public:

        GUITEST_API BacktraceLock(Tester& tester);
        GUITEST_API ~BacktraceLock();

    private:

        Tester& m_tester;
    };

    /// Constructor. Initializes the test name.
    GUITEST_API Tester(std::string testName, bool verboseMode = false);

    /// Destructor. Joins the thread.
    GUITEST_API ~Tester();

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent A function which returns the new graphic component.
     * @param condition The condition the new graphic component must respect.
     * @param timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void take(
        const std::string& componentDescription,
        std::function<T()> graphicComponent,
        std::function<bool(T)> condition = alwaysTrue,
        int timeout                      = DEFAULT_TIMEOUT
    );
    GUITEST_API void take(
        const std::string& componentDescription,
        std::function<QObject* ()> graphicComponent,
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent The new graphic component.
     */
    GUITEST_API void take(const std::string& componentDescription, QObject* graphicComponent);

    /**
     * @brief Stores a graphic component as the new current graphic component.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param parent The parent of the new current graphic component, typically a window. Default is the main window.
     * @param componentName The objectName of the new current graphic component to be found.
     *
     * @{
     */
    GUITEST_API void take(
        const std::string& componentDescription,
        QObject* parent,
        const std::string& objectName,
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    GUITEST_API void take(
        const std::string& componentDescription,
        const std::string& objectName,
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Interacts with the current graphic component.
     *
     * @param interaction The interaction that will be used on the current graphic component.
     *
     * @warning The interaction must be allocated on the heap. GuiTester will take care of freeing the interaction, it
     * is unsafe to try to access the interaction after this method is called.
     */
    GUITEST_API void interact(std::unique_ptr<Interaction>&& interaction);

    /**
     * @brief Check if the assertion is true.
     *
     * @param resultDescription The description of the expected result. Used in failure messages.
     * @param result The assertion, which must be true for the test to pass.
     * @param timeout The max time to wait for the assertion to become true before the test fails.
     *
     * @note If the template is used, the current component will be casted before being used in result.
     *
     * @{
     */
    template<typename T>
    void doubt(
        const std::string& resultDescription,
        std::function<bool(T)> result,
        int timeout = DEFAULT_TIMEOUT
    );
    GUITEST_API void doubt(
        const std::string& resultDescription,
        std::function<bool(QObject*)> result,
        int timeout = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the graphicComponent function get access to the old graphic component. It also
     * slightly changes the failure messages.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent A function which returns the new graphic component and takes the old graphic component
     * @param condition The condition the new graphic component must respect.
     * @param timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void yields(
        const std::string& componentDescription,
        std::function<T(QObject*)> graphicComponent,
        std::function<bool(T)> condition = alwaysTrue,
        int timeout                      = DEFAULT_TIMEOUT
    );
    GUITEST_API void yields(
        const std::string& componentDescription,
        std::function<QObject* (QObject*)> graphicComponent,
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the graphicComponent is found inside the current graphic component tree. It also
     * slightly changes the failure messages.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param objectName The objectName of the new graphic component to be found.
     *
     * @note The template version narrows the search to the components compatible with that type.
     *
     * @{
     */
    template<typename T>
    void yields(
        const std::string& componentDescription,
        const std::string& objectName           = "",
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    void yields(
        const std::string& componentDescription,
        const std::string& objectName           = "",
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Like @ref take, but the test doesn't fail if the component doesn't appear, instead the following commands
     * will be ignored, until the next @ref take.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent A function which returns the new graphic component and takes the old graphic component
     * @param condition The condition the new graphic component must respect.
     * @param timeout The max time to wait for the component before the test fails.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in
     * condition.
     *
     * @{
     */
    template<typename T>
    void maybeTake(
        const std::string& componentDescription,
        std::function<T()> graphicComponent,
        std::function<bool(T)> condition = alwaysTrue,
        int timeout                      = DEFAULT_TIMEOUT
    );
    GUITEST_API void maybeTake(
        const std::string& componentDescription,
        std::function<QObject* ()> graphicComponent,
        std::function<bool(QObject*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );
    /// @}

    /**
     * @brief Calls the function with the current component, in order to cause side-effects.
     *
     * @param f The function to be called, which will get the current component.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in f.
     *
     * @{
     */
    template<typename T>
    void doSomething(std::function<void(T)> f);
    GUITEST_API void doSomething(std::function<void(QObject*)> f);
    /// @}

    /**
     * @brief Like @ref doSomething, but the function is called asynchronously, on the main GUI thread.
     *
     * @param f The function to be called asynchronously, which will get the current component.
     *
     * @note If the template is used, the value returned by graphicComponent will be casted before being used in f.
     *
     * @{
     */
    template<typename T>
    void doSomethingAsynchronously(std::function<void(T)> f);
    GUITEST_API void doSomethingAsynchronously(std::function<void(QObject*)> f);
    /// @}

    /**
     * @brief Takes a screenshot of the current graphic component
     * @param path Where to save the screenshot
     */
    GUITEST_API void takeScreenshot(const std::filesystem::path& path);

    /**
     * @brief Starts the test.
     * @details It will create a thread which will wait for the main window to appear, call f, then try to close the
     * main window, and finally wait for the main window to disappear. Code in the thread is surrounded with a
     * try/catch, as to not catch exceptions in a thread causes process termination in C++.
     *
     * @param f The function to be called.
     *
     * @pre @ref init was called.
     */
    GUITEST_API void start(std::function<void()> f);

    /**
     * @brief Check if the specified component doesn't exist or exists but is hidden, the test fails else.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent A function which returns the new graphic component and takes the old graphic component
     * @param condition An additional condition the new graphic component must respect.
     * @param timeout The max time to wait for the component.
     *
     * @return Is the component is hidden?
     *
     * @warning The current graphic component becomes undefined after the call of this method.
     */
    GUITEST_API void shouldBeHidden(
        const std::string& componentDescription,
        std::function<QWidget* ()> graphicComponent,
        std::function<bool(QWidget*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );

    /**
     * @brief Check if the specified component exist and is visible, the test fails else. The current graphic component
     * becomes this one after the call.
     *
     * @param componentDescription The description of the new graphic component. Used in failure messages.
     * @param graphicComponent A function which returns the new graphic component and takes the old graphic component
     * @param condition An additional condition the new graphic component must respect.
     * @param timeout The max time to wait for the component.
     *
     * @return Is the component present?
     */
    GUITEST_API void shouldBePresent(
        const std::string& componentDescription,
        std::function<QWidget* ()> graphicComponent,
        std::function<bool(QWidget*)> condition = alwaysTrue,
        int timeout                             = DEFAULT_TIMEOUT
    );

    /**
     * @brief Check if there is a current component. Useful to check if a @ref maybeTake succeeded.
     *
     * @return Is there a current component?
     */
    GUITEST_API bool exists();

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
     * @note If there is no current component (because no take were ever called, or after a failed @ref maybeTake), or
     * if the cast fails, the test fails.
     */
    template<typename T>
    T get();

    /**
     * @brief Get the main window
     *
     * @returns The main window
     */
    GUITEST_API QWidget* getMainWindow();

    /**
     * @brief Returns the failure message.
     *
     * @returns The failure message.
     *
     * @pre failed() must return false.
     */
    [[nodiscard]] GUITEST_API std::string getFailureMessage() const;

    /**
     * @brief Returns true if the test failed and false otherwise.
     *
     * @returns Did the test fail?
     */
    [[nodiscard]] GUITEST_API bool failed() const;

    [[nodiscard]] GUITEST_API BacktraceLock addInBacktrace(const std::string& description);

    /**
     * @brief Returns the description associated to the current graphic component.
     *
     * @returns Current graphic component description
     */
    [[nodiscard]] GUITEST_API std::string getDescription() const;

    /**
     * @brief Properly initializes GuiTester.
     * @details It installs an event filter on the QApplication (needed to execute interactions asynchronously) and sets
     * an environment variable so that the remainder of the library knows that GUI tests are running (for now, only
     * needed to show up the Qt file dialog instead of the native one).
     */
    GUITEST_API static void init();

    /**
     * @brief Helper method that returns the associated widget of a QAction.
     * @details Some components of the GUI are QAction, which are abstract and not directly interactable. This method
     * exists to easily get an interactable widget associated with the QAction.
     *
     * @param action The action for which the associated widget is needed.
     *
     * @note For convenience, if action is nullptr, this method returns nullptr.
     */
    GUITEST_API static QWidget* getWidgetFromAction(QAction* action);

    /**
     * @brief Move the mouse on a component, alternative to QTest::mouseMove as a workaround to QTBUG-5232.
     *
     * @param component The component on which move the mouse
     * @param pos The position where the mouse will be, in coordinates relative to the component.
     * @param delay The delay before executing this interaction.
     * @param button The button which clicked while moving
     * @param modifiers The modifiers which are active while moving
     *
     * @{
     */
    GUITEST_API static void mouseMove(
        QWidget* component,
        QPoint pos                      = QPoint(),
        int delay                       = -1,
        Qt::MouseButton button          = Qt::NoButton,
        Qt::KeyboardModifiers modifiers = Qt::NoModifier
    );
    GUITEST_API static void mouseMove(
        QWindow* component,
        QPoint pos                      = QPoint(),
        int delay                       = -1,
        Qt::MouseButton button          = Qt::NoButton,
        Qt::KeyboardModifiers modifiers = Qt::NoModifier
    );
    /// @}

    /**
     * @brief Compare images by comparing pixel by pixel.
     * @details Returns the percentage of similar pixels between the two images.
     *
     * @param a,b The images to compare
     * @param strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesPixelPerfect(QImage a, QImage b, bool strict = false);

    /**
     * @brief Compare images using the Mean Square Error.
     * @details The MSE is normalized so that its value is between 0 and 1.
     *
     * @param a,b The images to compare
     * @param strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesMSE(QImage a, QImage b, bool strict = false);

    /**
     * @brief Compare images by computing the cosine of the angle between the matrices of the two images.
     * @details Extremely sensible to big color changes.
     *
     * @param a,b The images to compare
     * @param strict Describe the behavior if the size of the images is different: if strict, return 0, else, resize the
     * images and compare the resized images.
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesCosine(QImage a, QImage b, bool strict = false);

    /**
     * @brief Compare images by computing an histogram.
     * @details May be used with images of different size, but totally ignore the position of the pixels.
     *
     * @param a,b The images to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesHistogram(const QImage& a, const QImage& b);

    /**
     * @brief Compare images by computing Spearman's correlation coefficient of the two matrices of the two images.
     *
     * @param a,b The images to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesCorrelation(QImage a, QImage b, bool strict = false);

    /**
     * @brief Compare images by using the MSE on normalized, scaled-down, rotated and flipped version of the images.
     * @details Comes from https://stackoverflow.com/a/26061
     *
     * @param a,b The images to compare
     * @returns a real between 0 (totally different images) and 1 (identical images)
     */
    GUITEST_API static double compareImagesVoodoo(const QImage& a, const QImage& b);

    /**
     * @brief Respectively returns the center, the left, the right, the top and the bottom of a widget.
     *
     * @param widget The widget for which compute the position
     * @returns The position
     *
     * @{
     */
    GUITEST_API static QPoint centerOf(const QWidget* widget);
    GUITEST_API static QPoint leftOf(const QWidget* widget);
    GUITEST_API static QPoint rightOf(const QWidget* widget);
    GUITEST_API static QPoint topOf(const QWidget* widget);
    GUITEST_API static QPoint bottomOf(const QWidget* widget);
    /// @}

    /**
     * @brief Returns the folder where the images generated by the test should be stored.
     * @details Equals to the value of the IMAGE_OUTPUT_PATH environment variable, or the system temporary folder (such
     * as /tmp on UNIXes) if this environment variable is empty.
     */
    GUITEST_API static std::filesystem::path getImageOutputPath();

    /**
     * @brief Returns a dummy touch screen to emulate touch events
     */
    static QTouchDevice* getDummyTouchScreen();

    /// The default timeout as used in the methods @ref take, @ref doubt, @ref yields and @ref maybeTake
    static const int DEFAULT_TIMEOUT = 10000;

private:

    static GUITEST_API void fail(const std::string& message);
    std::string generateFailureMessage();

    static QVector<QVector<QVector<double> > > computeHistogram(const QImage& img);
    static QImage normalize(QImage img);
    static QImage voodooize(const QImage& img);
    GUITEST_API static bool waitForAsynchronously(std::function<bool()> predicate, int timeout = DEFAULT_TIMEOUT);

    QPointer<QObject> m_graphicComponent = nullptr;
    std::string m_componentDescription;
    std::vector<std::unique_ptr<Interaction> > m_interactions;
    bool m_failed = false;
    std::thread m_thread;
    QWidget* m_mainWindow = nullptr;
    std::string m_failureMessage;
    std::string m_resultDescription;
    const std::string m_testName;
    bool m_verboseMode;
    std::vector<std::string> m_backtrace;

    static bool s_alreadyLoaded;
    static std::filesystem::path s_imageOutputPath;
};

} // namespace sight::ui::testCore

#include "Tester.hxx"
