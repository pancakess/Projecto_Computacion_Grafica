
#version 330 core

// Positions/Coordinates
layout (location = 0) in vec3 aPos;
// Colors, pero ya no hay pq tenemos sólo texturas
// Texture Coordinates
layout (location = 1) in vec2 aTex;


// Outputs the color for the Fragment Shader, ya no hay
// Outputs the texture coordinates to the fragment shader
out vec2 texCoord;

// Controls the scale of the vertices
uniform float scale;

// Inputs the matrices needed for 3D viewing with perspective
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


void main()
{
	// Outputs the positions/coordinates of all vertices
	gl_Position = proj * view * model * vec4(aPos, 1.0);
	// Assigns the colors from the Vertex Data to "color", ya no hay
	// Assigns the texture coordinates from the Vertex Data to "texCoord"
	texCoord = aTex;
}