#version 330

in vec4 vCol;																  
uniform vec3 color;

out vec4 colour;                                                              

void main()                                                                   
{                                                                           
	 colour = vec4(color, 1.0f);		
}