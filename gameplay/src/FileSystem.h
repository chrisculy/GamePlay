#pragma once

#include "Stream.h"
#include <string>

namespace gameplay
{

/**
 * Defines a set of functions for interacting with the device file system.
 */
class FileSystem
{
public:

    /**
     * Access mode flags for opening a stream.
     */
    enum AccessMode
    {
        ACCESS_MODE_READ = 1,
        ACCESS_MODE_WRITE = 2
    };

    /**
     * Dialog mode flags when displaying dialogs.
     */
    enum DialogMode
    {
        DIALOG_MODE_OPEN,
        DIALOG_MODE_SAVE 
    };

    /**
     * Destructor.
     */
    ~FileSystem();

    /**
     * Sets the path to the root of the assets folder for the game.
     *
     * Once set, all asset/file loading will load from the given path.
     * The default asser path is "./".
     * 
     * @param path The path to the root of the assets folder.
     */
    static void setAssetPath(const std::string& path);

    /**
     * Gets the currently set asset path.
     * 
     * @return The currently set asset path.
     */
    static std::string getAssetPath();

    /**
     * Resolves a filesystem path.
     *
     * If the specified path is a filesystem alias, the alias will be
     * resolved and the physical file will be returned.
     *
     * Note that this method does not convert a relative path to an
     * absolute filesystem path.
     *
     * @param path Path to resolve.
     * @return The resolved file path.
     */
    static std::string resolvePath(const std::string& path);

    /**
     * Lists the files in the specified directory and adds the files to the vector. Excludes directories.
     * 
     * @param dirPath Directory path relative to the path set in <code>setAssetPath(const char*)</code>.
     * @param files The vector to append the files to.
     * @return True if successful, false if error.
     */
    static bool listFiles(const std::string& dirPath, std::vector<std::string>& files);

    /**
     * Checks if the file at the given path exists.
     * 
     * @param filePath The path to the file.
     * @return <code>true</code> if the file exists; <code>false</code> otherwise.
     */
    static bool fileExists(const std::string& filePath);

    /**
     * Opens a byte stream for the given asset path.
     *
     * If <code>path</code> is a file path, the file at the specified location is opened relative to the currently set
     * asset path.
     *
     * @param path The path to the asset to be opened, relative to the currently set asset path.
     * @param accessMode The access mode used to open the file.
     * @return A stream that can be used to read or write to the file depending on the mode, 
     *         nullptr if there was an error. (Request mode not supported).
     */
    static Stream* open(const std::string& path, size_t accessMode = ACCESS_MODE_READ);

    /**
     * Reads the entire contents of the specified file and returns its contents.
     *
     * The returned character array is allocated with new[] and must therefore
     * deleted by the caller using delete[].
     *
     * @param filePath The path to the file to be read.
     * @return A string containing the contents of the file or empty string if
     */
    static std::string readAll(const std::string& filePath);

    /**
     * Determines if the file path is an absolute path for the current platform.
     * 
     * @param filePath The file path to test.
     * @return true if the path is an absolute path or false otherwise.
     */
    static bool isAbsolutePath(const std::string& filePath);

    /**
     * Gets the directory name up to and including the trailing '/'.
     * 
     * This is a lexical method so it does not verify that the directory exists.
     * Back slashes will be converted to forward slashes.
     * 
     * - "res/image.png" will return "res/"
     * - "image.png" will return ""
     * - "c:/foo/bar/image.png" will output "c:/foo/bar/"
     * 
     * @param path The file path. May be relative or absolute, forward or back slashes. May be nullptr.
     * @return The directory name with the trailing '/' or emptry string if invalid string or directory.
     */
    static std::string getDirectoryName(const std::string& path);

    /**
     * Gets the extension of the given file path.
     *
     * The extension returned includes all character after and including the last '.'
     * in the file path. The extension is returned as all uppercase.
     *
     * If the path does not contain an extension, an empty string is returned.
     * 
     * @param path File path.
     * @return The file extension, all uppercase, including the '.'.
     */
    static std::string getExtension(const std::string& path);

private:

    FileSystem();
};

}
