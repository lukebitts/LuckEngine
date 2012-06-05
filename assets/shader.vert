#version 330
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexUV;

uniform mat4 ModelViewProjection;
out vec4 fragmentColor;
out vec2 UV;

void main()
{
	gl_Position = vec4(vertexPosition,1) * ModelViewProjection;
    fragmentColor = vertexColor;
    UV = vertexUV;
}
