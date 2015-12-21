#version 430

in layout(location=0) vec3 position;
in layout(location=1) vec3 vertexColor;
in layout(location=2) mat4 transformationMatrix;

uniform mat4 perspectiveViewMatrix;
uniform int drawLines;

out vec3 theColor;



void main()
{
vec4 v = vec4(position, 1.0);
vec4 newPosition =  perspectiveViewMatrix * transformationMatrix * v;
gl_Position = newPosition;

if(drawLines == 0)
{
theColor = vertexColor;
} else
{
//For now just make the outline black
vec3 newColour = vec3(0,0,0);
theColor = newColour;
}
};