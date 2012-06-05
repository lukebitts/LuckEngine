#ifndef IMAGE_H
#define IMAGE_H
#include "luck.h"
#include "lodepng.h"
namespace luck { namespace asset
{
    struct Image
    {
        std::vector<u8> data;
        u32 width, height;
        u32 textureID;
        Image(string path, bool invertY = true)
        {
            u32 error = lodepng::decode(data, width, height, path);
            if(error)
            {
                std::cout << "error " << error << ": " << lodepng_error_text(error) << std::endl;
                throw std::exception();
            }

            if(invertY)
            {
                u32 fragSize = data.size()/height/width;
                for(u32 i = 0; i < height/2; i++)
                for(u32 j = 0; j < width*fragSize; j++)
                {
                    u8 temp = data[(i*width*fragSize)+j];
                    data[(i*width*fragSize)+j] = data[(height-1-i)*width*fragSize+j];
                    data[(height-1-i)*width*fragSize+j] = temp;
                }
            }

            /// @todo this should not be here, an image may not be a texture
            glGenTextures(1, &textureID);
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, 4, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, &data[0]);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glGenerateMipmap(GL_TEXTURE_2D);
            glBindTexture(GL_TEXTURE_2D, 0);
        }
    };
}}
#endif

