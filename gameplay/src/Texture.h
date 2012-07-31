#ifndef TEXTURE_H_
#define TEXTURE_H_

#include "Ref.h"

namespace gameplay
{

class Image;

/**
 * Represents a texture.
 */
class Texture : public Ref
{
    friend class Sampler;

public:

    /**
     * Defines the set of supported texture formats.
     */
    enum Format
    {
        RGB     = GL_RGB,
        RGBA    = GL_RGBA,
        ALPHA   = GL_ALPHA
    };

    /**
     * Defines the set of supported texture filters.
     */
    enum Filter
    {
        NEAREST = GL_NEAREST,
        LINEAR = GL_LINEAR,
        NEAREST_MIPMAP_NEAREST = GL_NEAREST_MIPMAP_NEAREST,
        LINEAR_MIPMAP_NEAREST = GL_LINEAR_MIPMAP_NEAREST,
        NEAREST_MIPMAP_LINEAR = GL_NEAREST_MIPMAP_LINEAR,
        LINEAR_MIPMAP_LINEAR = GL_LINEAR_MIPMAP_LINEAR
    };

    /**
     * Defines the set of supported texture wrapping modes.
     */
    enum Wrap
    {
        REPEAT = GL_REPEAT,
        CLAMP = GL_CLAMP_TO_EDGE
    };
    
    /**
     * Defines a texture sampler.
     *
     * A texture sampler is basically an instance of a texture that can be
     * used to sample a texture from a material. In addition to the texture
     * itself, a sampler stores per-instance texture state information, such
     * as wrap and filter modes.
     */
    class Sampler : public Ref
    {
        friend class Texture;

    public:

        /**
         * Destructor.
         */
        virtual ~Sampler();

        /**
         * Creates a sampler for the specified texture.
         *
         * @param texture The texture.
         *
         * @return The new sampler.
         * @script{create}
         */
        static Sampler* create(Texture* texture);

        /**
         * Creates a sampler for the specified texture.
         *
         * @param path Path to the texture to create a sampler for.
         * @param generateMipmaps True to force a full mipmap chain to be generated for the texture, false otherwise.
         *
         * @return The new sampler.
         * @script{create}
         */
        static Sampler* create(const char* path, bool generateMipmaps = false);

        /**
         * Sets the wrap mode for this sampler.
         *
         * @param wrapS The horizontal wrap mode.
         * @param wrapT The vertical wrap mode.
         */
        void setWrapMode(Wrap wrapS, Wrap wrapT);

        /**
         * Sets the texture filter modes for this sampler.
         *
         * @param minificationFilter The texture minification filter.
         * @param magnificationFilter The texture magnification filter.
         */
        void setFilterMode(Filter minificationFilter, Filter magnificationFilter);

        /**
         * Gets the texture for this sampler.
         *
         * @return The texture for this sampler.
         */
        Texture* getTexture() const;

        /**
         * Binds the texture of this sampler to the renderer and applies the sampler state.
         */
        void bind();

    private:

        /**
         * Constructor.
         */
        Sampler(Texture* texture);

        /**
         * Hidden copy assignment operator.
         */
        Sampler& operator=(const Sampler&);

        Texture* _texture;
        Wrap _wrapS;
        Wrap _wrapT;
        Filter _minFilter;
        Filter _magFilter;
    };

    /**
     * Creates a texture from the given image resource.
     *
     * Note that for textures that include mipmap data in the source data (such as most compressed textures),
     * the generateMipmaps flags should NOT be set to true.
     *
     * @param path The image resource path.
     * @param generateMipmaps true to auto-generate a full mipmap chain, false otherwise.
     * 
     * @return The new texture, or NULL if the texture could not be loaded/created.
     * @script{create}
     */
    static Texture* create(const char* path, bool generateMipmaps = false);

    /**
     * Creates a texture from the given image.
     * @script{create}
     */
    static Texture* create(Image* image, bool generateMipmaps = false);

    /**
     * Creates a texture from the given texture data.
     * @script{create}
     */
    static Texture* create(Format format, unsigned int width, unsigned int height, unsigned char* data, bool generateMipmaps = false);

    /**
     * Gets the format of the texture.
     *
     * @return The texture format.
     */
    Format getFormat() const;

    /**
     * Gets the texture width.
     *
     * @return The texture width.
     */
    unsigned int getWidth() const;

    /**
     * Gets the texture height.
     *
     * @return The texture height.
     */
    unsigned int getHeight() const;

    /**
     * Sets the wrap mode for this texture.
     *
     * @param wrapS Horizontal wrapping mode for the texture.
     * @param wrapT Vertical wrapping mode for the texture.
     */
    void setWrapMode(Wrap wrapS, Wrap wrapT);

    /**
     * Sets the minification and magnification filter modes for this texture.
     *
     * @param minificationFilter New texture minification filter.
     * @param magnificationFilter New texture magnification filter.
     */
    void setFilterMode(Filter minificationFilter, Filter magnificationFilter);

    /**
     * Generates a full mipmap chain for this texture if it isn't already mipmapped.
     */
    void generateMipmaps();

    /**
     * Determines if this texture currently contains a full mipmap chain.
     *
     * @return True if this texture is currently mipmapped, false otherwise.
     */
    bool isMipmapped() const;

    /**
     * Determines if this texture is a compressed texture.
     */
    bool isCompressed() const;

    /**
     * Returns the texture handle.
     *
     * @return The texture handle.
     */
    TextureHandle getHandle() const;

private:

    /**
     * Constructor.
     */
    Texture();

    /**
     * Copy constructor.
     */
    Texture(const Texture& copy);

    /**
     * Destructor.
     */
    virtual ~Texture();

    /**
     * Hidden copy assignment operator.
     */
    Texture& operator=(const Texture&);

    static Texture* createCompressedPVRTC(const char* path);

    static Texture* createCompressedDDS(const char* path);

    static GLubyte* readCompressedPVRTC(const char* path, FILE* file, GLsizei* width, GLsizei* height, GLenum* format, unsigned int* mipMapCount);

    static GLubyte* readCompressedPVRTCLegacy(const char* path, FILE* file, GLsizei* width, GLsizei* height, GLenum* format, unsigned int* mipMapCount);

    std::string _path;
    TextureHandle _handle;
    Format _format;
    unsigned int _width;
    unsigned int _height;
    bool _mipmapped;
    bool _cached;
    bool _compressed;
};

}

#endif