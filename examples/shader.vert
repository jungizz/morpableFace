 #version 410 core

layout(location=0) in vec3 in_Position;
layout(location=1) in vec3 in_Normal;
layout(location=2) in vec2 in_TexCoords;

uniform mat4 modelMat;
uniform mat4 viewMat;
uniform mat4 projMat;

out vec3 normal;
out vec3 worldPosition;
out vec2 texCoords;
out float z;


void main(void){
	vec4 p = vec4(in_Position.xyz, 1);
	p = projMat*viewMat*modelMat*p;
	normal = normalize((modelMat * vec4(in_Normal,0)).xyz);
	worldPosition= vec3(modelMat* vec4(in_Position,1));
	texCoords=in_TexCoords;
	gl_Position = p;
}

