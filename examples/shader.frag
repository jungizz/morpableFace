#version 150 core

out vec4 out_Color;
uniform vec4 color;
uniform vec3 lightPosition=vec3(200,300,300);
uniform vec3 lightColor=vec3(100000);
uniform float shininess = 0.0f;
uniform vec3 cameraPosition;
uniform vec3 ambientLight = vec3(0.0);
uniform int TexOrColor = 0;

//texture
uniform sampler2D diffuseMap;
uniform sampler2D specularMap;
uniform sampler2D normalMap;
uniform sampler2D heightMap;

in vec3 normal;
in vec3 worldPosition;
in vec2 texCoords;

mat3 getTBN(vec3 N){
	vec3 Q1 = dFdx(worldPosition), Q2 = dFdy(worldPosition);
	vec2 st1 = dFdx(texCoords), st2 = dFdy(texCoords);
	float D = st1.s*st2.t - st1.t*st2.s;
	return mat3( normalize( (Q1*st2.t-Q2*st1.t)*D ), 
				 normalize( (-Q1*st2.s+Q2*st1.s)*D ),
				 N);
}

void main(void)
{
	vec3 l = lightPosition - worldPosition;
	vec3 L = normalize(l);
	vec3 N = normalize(normal);


/*
	mat3 tbn = getTBN(N);
	float dBdu = texture(bumpTex, texCoords+vec2(0.00001, 0)).r
				-texture(bumpTex, texCoords-vec2(0.00001, 0)).r;
	float dBdv = texture(bumpTex, texCoords+vec2(0, 0.00001)).r
				-texture(bumpTex, texCoords-vec2(0, 0.00001)).r;

	N = normalize(N - dBdu* tbn[0]*100 - dBdv*tbn[1]*100);
*/


	vec3 V = normalize(cameraPosition - worldPosition);
	vec3 R = 2*dot(L,N)*N-L;

	vec3 I = lightColor/dot(l,l);

	vec3 final_color = vec3(0.0f);

	//textureÀÇ °æ¿ì
	if(TexOrColor>0){
		vec4 Diffuse = texture(diffuseMap,texCoords);
		vec4 Specular = texture(specularMap,texCoords);
		vec4 Normal = texture(normalMap,texCoords);

		final_color = Diffuse.rgb*max(0,dot(L,N))*I+Diffuse.rgb*ambientLight;
		final_color += pow(dot(R,V),shininess)*I*Specular.rgb;
		out_Color = vec4(pow(final_color,vec3(1/2.2)),1);

	}
	else{
		out_Color = color;
	}

}
