#version 330
layout(location = 0) in vec3 vertexPosition;
layout(location = 1) in vec4 vertexColor;

uniform mat4 ModelViewProjection;
out vec4 fragmentColor;

void main()
{
	gl_Position = vec4(vertexPosition,1) * ModelViewProjection;
    fragmentColor = vertexColor;
}
