#version 430  // GLSL version your computer supports

in layout(location = 0) vec3 position;
in layout(location = 2) vec3 normal;
in layout(location = 1) vec2 vertexUV;
//in layout(location = 3) vec3 color;

uniform mat4 modelTransformMatrix;
uniform mat4 projectionMatrix;

out vec3 theColor;
out vec3 normalWorld;
out vec3 vertexPositionWorld;
out vec2 UV;

void main()
{
	vec4 v = vec4(position, 1.0);
	vec4 newPosition = modelTransformMatrix * v;
	vec4 projectedPosition = projectionMatrix * newPosition;
	gl_Position = projectedPosition;

	vec4 normal_temp = modelTransformMatrix * vec4(normal, 0);
	normalWorld = normal_temp.xyz;

	vertexPositionWorld = newPosition.xyz;
	UV = vertexUV;

	//theColor = color;
}