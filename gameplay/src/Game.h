#pragma once

#include "Platform.h"
#include "Serializable.h"
#include "SceneObject.h"
#include "Camera.h"

namespace gameplay
{

extern Platform* getPlatform();

/**
 * Defines the game class.
 */
class Game
{
public:

    /**
     * The game states.
     */
    enum State
    {
        STATE_UNINITIALIZED,
        STATE_SPLASH,
        STATE_LOADING,
        STATE_RUNNING,
        STATE_PAUSED
    };

    /**
     * Defines a splash screen.
     */
    struct SplashScreen
    {
        std::string url;
        float duration;
    };
    
    /**
     * Constructor.
     */
    Game();

    /**
     * Destructor.
     */
    ~Game();

    /**
     * Gets the single instance of the game.
     * 
     * @return The single instance of the game.
     */
    static Game* getInstance();

    /**
     * Gets the game state.
     *
     * @return The current game state.
     */
    State getState() const;

    /**
     * Gets the total absolute running time (in milliseconds) since platform startup.
     *
     * @return The total absolute running time (in milliseconds) since platform startup. 
     */
    static double getAbsoluteTime();

    /**
     * Gets the total game time (in milliseconds). This is the total accumulated game time (unpaused).
     *
     * You would typically use things in your game that you want to stop when the game is paused.
     * This includes things such as game physics and animation.
     *
     * @return The total game time (in milliseconds).
     */
    static double getGameTime();

    /**
     * Gets the current frame rate.
     *
     * @return The current frame rate.
     */
	size_t getFrameRate() const;

    /**
     * Gets the game window width.
     *
     * @return The game window width.
     */
	int getWidth() const;

    /**
     * Gets the game window height.
     *
     * @return The game window height.
     */
	int getHeight() const;

    /**
     * Gets the aspect ratio of the window (width / height).
     *
     * @return The aspect ratio of the window (width / height).
     */
    float getAspectRatio() const;

    /**
     * Pauses the game.
     */
    void pause();

    /**
     * Resumes the game after being paused.
     */
    void resume();

    /**
     * Exits the game.
     */
    void exit();

    /**
     * Shows splash screens one after another for their duration.
     *
     * @param splashScreens The splash screens to be shown.
     */
    void showSplashScreens(std::vector<SplashScreen> splashScreens);

    /**
     * Sets the loading scene to be be used after calling Game::loadScene.
     *
     * @param url The url of the scene to be used for loading.
     * @see Game::loadScene
     */
    void setLoadingScene(const std::string& url);

    /**
     * Gets the loading scene to be used after calling Game::loadScene.
     *
     * @return The scene to be used for loading.
     * @see Game::loadScene
     */
    std::shared_ptr<SceneObject> getLoadingScene() const;

	/**
	 * Loads the scene and any resources that are needed.
	 *
	 * @param url The url the the scene to be set and loaded. 
     * @param showLoadingScene true shows the loading screen until scene is loaded.
     * @see Game::setLoadingScene
	 */
	void loadScene(const std::string& url, bool showLoadingScene = true);

    /**
     * Unloads the game scene and any resource that are no longer needed.
     *
     * @param scene The scene to be unloaded.
     */
    void unloadScene(std::shared_ptr<SceneObject> scene);

    /**
     * Sets the current scene to be active and shown on screen.
     *
     * @param scene The scene to be set active and shown on screen.
     */
    void setScene(std::shared_ptr<SceneObject> scene);

	/**
	 * Gets the current game scene to be active and shown on screen.
	 *
	 * @return The current game scene to be active and shown on screen.
	 */
	std::shared_ptr<SceneObject> getScene() const;

    /**
     * Sets the active camera the current scene will use to view the scene.
     *
     * The active camera by default is the first object with a camera
     * attached called "mainCamera".
     *
     * The camera is not activated if the camera component is not
     * attached to an scene object in the active scene.
     *
     * @param camera The camera the scene will use to view the scene.
     */
    void setCamera(std::shared_ptr<Camera> camera);

    /**
     * Gets the active camera the current scene will use to view the scene.
     *
     * @return The camera the scene will use to view the scene.
     */
    std::shared_ptr<Camera> getCamera() const;

    /**
     * Initialize event occurs just after the platform starts up priorto first frame event.
     */
    virtual void onInitialize();

    /**
     * Finalize event occurs when the game is about to exit.
     */
    virtual void onFinalize();

    /**
     * Scene load occurs when the scene is completed loading all the scenes resources.
     *
     * @param scene The scene that has completed loading
     */
    virtual void onSceneLoad(std::shared_ptr<SceneObject> scene);

    /**
     * Resize event occurs when the game platform window has been resized.
     *
     * @param width The new game window width.
     * @param height The new game window height.
     */
    virtual void onResize(int width, int height);

    /**
     * Update event called every frame before the scene is updated.
	 *
	 * @param elapsedTime
     */
    virtual void onUpdate(float elapsedTime);

	/**
	 * Render event called every frame before the scene is rendered.
	 *
	 * @param elapsedTime
	 */
	virtual void onRender(float elapsedTime);

    /**
     * Gamepad event occurs on gamepad lifecycle changes such as connect/disconnect.
     *
     * @param evt The type of event.
     * @param gamepadIndex The gamepad index.
     */
    virtual void onGamepadEvent(Platform::GamepadEvent evt, size_t gamepadIndex);

    /**
     * Keyboard event occurs when keyboars keys state change.
     *
     * @param evt The key event that occurred.
     * @param key If evt is 
     */
    virtual void onKeyEvent(Platform::KeyboardEvent evt, int key);

    /**
     * Mouse event occurs when the mouse state change.
     *
     * @param evt The mouse event that occurred.
     * @param x The x position of the mouse in pixels. Left edge is zero.
     * @param y The y position of the mouse in pixels. Top edge is zero.
     * @param wheelDelta The number of mouse wheel ticks. Positive is up (forward), negative is down (backward).
     */
    virtual void onMouseEvent(Platform::MouseEvent evt, int x, int y, int wheelDelta);

	/**
	 * Touch event occurs when touch occurs when touch support is enabled.
	 *
	 * @param evt The touch event that occurred.
	 * @param x The x position of the touch in pixels. Left edge is zero.
	 * @param y The y position of the touch in pixels. Top edge is zero.
	 * @param touchIndex The order of occurrence for multiple touch contacts starting at zero.
	 */
	virtual void onTouchEvent(Platform::TouchEvent evt, int x, int y, size_t touchIndex);
    
    /**
     * Determines whether mouse input is currently set to capture.
     *
     * @return true if mouse input is currently set to capture, false if disabled.
     */
    bool isMouseCapture();
    
    /**
     * Sets mouse capture on or off.
     *
     * On platforms that support a mouse, when mouse capture is enabled,
     * the platform cursor will be hidden and the mouse will be warped
     * to the center of the screen. While mouse capture is enabled,
     * all mouse move events will then be delivered as deltas instead
     * of absolute positions.
     *
     * @param capture true to enable mouse capture mode, false to disable it.
     */
    void setMouseCapture(bool capture);
    
    /**
     * Sets the visibility of the platform cursor.
     *
     * @param visible true to show the platform cursor, false to hide it.
     */
    void setCursorVisible(bool visible);
    
    /**
     * Determines whether the platform cursor is currently visible.
     *
     * @return true if the platform cursor is visible, false otherwise.
     */
    bool isCursorVisible();

    /**
     * Frame event called every frame when running.
     */
    void onFrame();

    /**
     * Game configuration.
     */
    class Config : public Serializable
    {
    public:

        /**
         * Constructor
         */
        Config();

        /**
         * Destructor
         */
        ~Config();

        /**
         * @see Serializer::Activator::CreateObjectCallback
         */
        static std::shared_ptr<Serializable> createObject();

        /**
         * @see Serializable::getClassName
         */
        std::string getClassName();

        /**
         * @see Serializable::onSerialize
         */
        void onSerialize(Serializer* serializer);

        /**
         * @see Serializable::onDeserialize
         */
        void onDeserialize(Serializer* serializer);

        std::string title;
		std::string graphics;
		int width;
		int height;
		bool fullscreen;
		bool vsync;
		size_t multisampling;
		bool validation;
		bool touchSupport;
		bool accelerometerSupport;
		std::string assetsPath;
        std::vector<SplashScreen> splashScreens;
        std::string loadingScene;
		std::string mainScene;
    };

    /**
     * Gets the game configuration
     *
     * @return The game configuration.
     */
    std::shared_ptr<Game::Config> getConfig();

private:

    Game(const Game& copy);
    void initializeSplash();
    void initializeLoading();
    void onSplash(float elapsedTime);
    void onLoading(float elapsedTime);
    double updateFrameRate();

    std::shared_ptr<Game::Config> _config;
    State _state;
	size_t _width;
	size_t _height;
	bool _mouseCapture;
	bool _cursorVisible;
	static std::chrono::time_point<std::chrono::high_resolution_clock> _timeStart;
	static double _pausedTimeLast;
	static double _pausedTimeTotal;
	size_t _pausedCount;
	double _frameLastFPS;
	size_t _frameCount;
	size_t _frameRate;
    std::queue<SplashScreen> _splashScreens;
    std::map<std::string, std::shared_ptr<SceneObject>> _scenesLoaded;
    static std::shared_ptr<SceneObject> _sceneLoadingDefault;
    std::shared_ptr<SceneObject> _sceneLoading;
	std::shared_ptr<SceneObject> _scene;
    std::shared_ptr<Camera> _camera;
};

}
