#version 330
in vec4 fragmentColor;
in vec2 UV;
out vec4 color;

uniform sampler2D myTextureSampler;

void main()
{
    if(textureSize(myTextureSampler,0).x > 0)
        color = texture2D( myTextureSampler, UV ) * fragmentColor;
    else
        color = fragmentColor;
}

