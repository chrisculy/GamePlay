#pragma once

namespace gameplay
{
    
class Serializable;
class Vector2;
class Vector3;
class Vector4;
class Matrix;
class Stream;

const unsigned char SERIALIZER_VERSION[2] = { GP_ENGINE_VERSION_MAJOR, GP_ENGINE_VERSION_MINOR};

/**
 * Defines an abstract class for reading/writing an objects data to a stream.
 *
 * This class is also used by Serializable object to serialize/deserialize their properties.
 *
 * @code
 *
 * // Write an object to file:
 * auto scene = std::make_shared<Scene>();
 * ...
 * auto writer = SerializerJson::createWriter("res/test.scene");
 * writer->writeObject(nullptr, std::dynamic_pointer_cast<Serializable>(scene));
 * writer->close();
 * GP_SAFE_DELETE(writer);
 *
 * // Read an object from file:
 * auto reader = Serializer::createReader("res/test.scene");
 * std::shared_ptr<Scene> scene = std::dynamic_pointer_cast<Scene>(reader->readObject(nullptr));
 * reader->close();
 * GP_SAFE_DELETE(reader);
 *
 * @endcode
 */
class Serializer
{
public:
    
    /**
     * The serializer format.
     */
    enum Format
    {
        FORMAT_BINARY,
        FORMAT_JSON
    };
    
    /**
     * Creates a serializer for reading (deserializing).
     *
     * @param path The path to load a serialized object from.
     */
    static Serializer* createReader(const std::string& path);

    /**
     * Gets the file path for the reader/writer.
     *
     * @return The file path for the reader/writer.
     */
    std::string getPath() const;
    
    /**
     * Closes the stream once you are done reading/writing.
     */
    virtual void close() = 0;

    /**
     * Gets the serializer format.
     *
     * @return The serializer format
     */
    virtual Format getFormat() const = 0;
    
    /**
     * Gets the major version of the loaded bundle.
     *
     * @return The major version of the loaded bundle.
     */
    virtual unsigned int getVersionMajor() const;
    
    /**
     * Gets the minor version of the loaded bundle.
     *
     * @return The minor version of the loaded bundle.
     */
    virtual unsigned int getVersionMinor() const;

    /**
     * Writes a enumerated value.
     *
     * @param propertyName The property to serialize.
     * @param enumName The name of the enumeration to write. Ex. "gameplay::Camera::Type"
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeEnum(const char* propertyName, const char* enumName, int value, int defaultValue) = 0;

    /**
     * Writes a bool value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeBool(const char* propertyName, bool value, bool defaultValue) = 0;

    /**
     * Writes an int value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeInt(const char* propertyName, int value, int defaultValue) = 0;

    /**
     * Writes a float value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeFloat(const char* propertyName, float value, float defaultValue) = 0;
    
    /**
     * Writes a Vector2 value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeVector(const char* propertyName, const Vector2& value, const Vector2& defaultValue) = 0;
    
    /**
     * Writes a Vector3 value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeVector(const char* propertyName, const Vector3& value, const Vector3& defaultValue) = 0;
    
    /**
     * Writes a Vector4 value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeVector(const char* propertyName, const Vector4& value, const Vector4& defaultValue) = 0;
    
    /**
     * Writes color from a Vector3 (RGB) value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeColor(const char* propertyName, const Vector3& value, const Vector3& defaultValue) = 0;
    
    /**
     *  Writes color from a Vector4 (RGBA) value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeColor(const char* propertyName, const Vector4& value, const Vector4& defaultValue) = 0;

    /**
     * Writes a Matrix value.
     *
     * @param propertyName The property to serialize.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeMatrix(const char* propertyName, const Matrix& value, const Matrix& defaultValue) = 0;

    /**
     * Writes a string.
     *
     * @param propertyName The property to serialize or nullptr if an item in a string list.
     * @param value The value to serialize.
     * @param defaultValue The default value of the specified property.
     */
    virtual void writeString(const char* propertyName, const char* value, const char* defaultValue) = 0;

    /**
     * Writes a list strings.
     *
     * The developer must then call writeString for the count of strings.
     *
     * @param propertyName The property name for the list to begin writing.
     * @param count The number of strings planned to be written following this call.
     */
    virtual void writeStringList(const char* propertyName, size_t count) = 0;

    /**
     * Writes an object.
     *
     * @param propertyName The property to serialize or nullptr if an item in a object list.
     * @param value The object to serialize
     */
    virtual void writeObject(const char* propertyName, std::shared_ptr<Serializable> value) = 0;

    /**
     * Writes a list of objects.
     *
     * The developer must then call writeObject
     * for the count of objects.
     *
     * @param propertyName The property name for the list to begin writing.
     * @param count The number of objects planned to be written following this call.
     */
    virtual void writeObjectList(const char* propertyName, size_t count) = 0;

    /**
     * Writes an array of integers.
     *
     * @param propertyName The property to serialize.
     * @param data The data to be written.
     * @param count The number of elements to be written.
     */
    virtual void writeIntArray(const char* propertyName, const int* data, size_t count) = 0;

    /**
     * Writes an array of floats.
     *
     * @param propertyName The property to serialize.
     * @param data The data to be written.
     * @param count The number of elements to be written.
     */
    virtual void writeFloatArray(const char* propertyName, const float* data, size_t count) = 0;
    
    /**
     * Write an array of byte data.
     *
     * @param propertyName The property to serialize.
     * @param data The data to be written.
     * @param count The number of elements to be written.
     */
    virtual void writeByteArray(const char* propertyName, const unsigned char* data, size_t count) = 0;

    /**
     * Reas a enumerated value.
     *
     * @param propertyName The property to serialize.
     * @param enumName The enumerations name. Ex. "gameplay::Camera::Type"
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual int readEnum(const char* propertyName, const char* enumName, int defaultValue) = 0;
    
    /**
     * Reads a bool.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual bool readBool(const char* propertyName, bool defaultValue) = 0;

    /**
     * Reads an integer.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The integer that is read.
     */
    virtual int readInt(const char* propertyName, int defaultValue) = 0;

    /**
     * Reads an float.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual float readFloat(const char* propertyName, float defaultValue) = 0;
    
    /**
     * Reads an Vector2.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Vector2 readVector(const char* propertyName, const Vector2& defaultValue) = 0;
    
    /**
     * Reads a Vector3.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Vector3 readVector(const char* propertyName, const Vector3& defaultValue) = 0;
    
    /**
     * Reads a Vector4.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Vector4 readVector(const char* propertyName, const Vector4& defaultValue) = 0;
    
    /**
     * Reads a Vector3 from a RGB color value.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Vector3 readColor(const char* propertyName, const Vector3& defaultValue) = 0;
    
    /**
     * Reads a Vector4 from a RGB color value.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Vector4 readColor(const char* propertyName, const Vector4& defaultValue) = 0;
    
    /**
     * Reads a Matrix
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read.
     * @param defaultValue The default value of the specified property.
     * @return The value that is read.
     */
    virtual Matrix readMatrix(const char* propertyName, const Matrix& defaultValue) = 0;

    /**
     * Reads a string.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read or nullptr for returning next property in list.
     * @param value The value to read into.
     * @param defaultValue The default value of the specified property.
     */
    virtual void readString(const char* propertyName, std::string& value, const char* defaultValue) = 0;

    /**
     * Reads a list of strings.
     *
     * This method determines the number of strings to deserialize.
     * The developer must use the return value for the
     * for number of strings to retrieve.
     * Then call readString for the number of strings in the list.
     *
     * @param propertyName The property to read.
     * @return The number of strings in the list
     */
    virtual size_t readStringList(const char* propertyName) = 0;

    /**
     * Reads an object.
     *
     * If the specified property is not found/serialized, the implementation
     * throw an error or return the specified default value.
     *
     * @param propertyName The property to read. If nullptr a new object is created.
     * @return The object that is read into.
     */
    virtual std::shared_ptr<Serializable> readObject(const char* propertyName) = 0;

    /**
     * Reads a list of objects.
     *
     * This method determines the number of objects to deserialize.
     * The developer must use the return value for the
     * for number of objects to retrieve. 
     * Then call readObject for the number of objects in the list.
     *
     * @param propertyName The property to read.
     * @return The number of objects in the list
     */
    virtual size_t readObjectList(const char* propertyName) = 0;

    /**
     * Reads an array of integers from the serializer.
     *
     * @param propertyName The property to read.
     * @param data The data array returned.
     * @return The array elements read.
     */
    virtual size_t readIntArray(const char* propertyName, int** data) = 0;

    /**
     * Reads an array of floats from the serializer.
     *
     * @param propertyName The property to read.
     * @param data The data array returned.
     * @return The array elements read.
     */
    virtual size_t readFloatArray(const char* propertyName, float** data) = 0;
    
    /**
     * Reads an array of bytes from the serializer.
     *
     * @param propertyName The property to read.
     * @param data The data array returned.
     * @return The array elements read.
     */
    virtual size_t readByteArray(const char* propertyName, unsigned char** data) = 0;
    
    /**
     * Defines a class for dynamic object activation.
     */
    class Activator
    {
        friend class Serializer;
        
    public:
        
        /**
         * Callback used to create objects on registered serializable classes.
         */
        typedef std::shared_ptr<Serializable> (*CreateObjectCallback)();
        
        /**
         * Callback used to convert enum values to string on registered serializable enums.
         *
         * @param enumName The enum name to convert.
         * @param value The enum value to be converted to string.
         * @return The string converted or nullptr if not converted successfully.
         */
        typedef std::string (*EnumToStringCallback)(const std::string& enumName, int value);
        
        /**
         * Callback used to parse the enum string to value on registered serializable enums.
         *
         * @param enumName The enum name to parse the string for.
         * @param str The string to be parsed
         * @return The enum value or -1 is not parsed successfully.
         */
        typedef int (*EnumParseCallback)(const std::string& enumName, const std::string& str);
        
        
        /**
         * Creates a new shared object for the specified class name.
         *
         * @param className The serialized class name of the instance to be created.
         * @return The new object instance.
         */
        std::shared_ptr<Serializable> createObject(const std::string& className);
        
        /**
         * Converts the enum value to a string representation.
         *
         * Enum string values are returned uppercase.
         *
         * @param enumName The enum name to convert.
         * @param value The enum value to be converted to string.
         * @return The string converted or default enum string if not matching.
         */
        std::string enumToString(const std::string& enumName, int value);
        
        /**
         * Parses the string representation of the enumeration.
         *
         * @param enumName The enum name to parse the string for.
         * @param str The string to be parsed
         * @return The enum value or -1 is not parsed successfully.
         */
        int enumParse(const std::string& enumName, const std::string& str);
        
        /**
         * Registers a callback for functor for instantiating new object for the specified type name.
         *
         * The className should be namespaced. Ex. gameplay::SceneObject
         *
         * @param className The type name to be registered to create a custom object.
         * @param create The create instance callback used to dynamically create a serializable object.
         */
        void registerType(const std::string& className, CreateObjectCallback create);
        
        /**
         * Registers an enum for resolving enumerated values to/from strings. 
         *
         * The enumName should be namespaced. Ex gameplay::Camera::Type
         *
         * If the enumeration support bitwise operations, it must return the string representation.
         * 
         * @param enumName The enum name to be register for to/from string resolvers.
         * @param enumToString The enumToString callback function.
         * @param enumParse The enumParse callback function.
         */
        void registerEnum(const std::string& enumName, EnumToStringCallback enumToString, EnumParseCallback enumParse);
        
    private:
        
        Activator();
        ~Activator();
        void initializeTypes();
        void initializeEnums();
        
        std::map<std::string, CreateObjectCallback> _classes;
        std::map<std::string, std::pair<EnumToStringCallback, EnumParseCallback>> _enums;
    };
    
    /**
     * Gets the activator that is used to create new serializable object that are registered.
     *
     * @return The activator.
     */
    static Activator* getActivator();
    
    /**
     * Destructor
     */
    virtual ~Serializer();
    
protected:

    enum Type
    {
        TYPE_READER,
        TYPE_WRITER
    };

    Serializer(Type type, const std::string& path, Stream* stream, unsigned int versionMajor, unsigned int versionMinor);

    Type _type;
    std::string _path;
    Stream* _stream;
    unsigned int _version[2];
    
};

}
