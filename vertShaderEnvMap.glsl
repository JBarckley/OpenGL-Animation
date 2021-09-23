#version 430

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
out vec3 vNormal;
out vec3 vVertPos;
out vec3 vLightDir;

out vec4 shadow_coord;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 normalMat;
uniform mat4 shadowMVP;

uniform vec3 lightPos;

layout (binding = 0) uniform samplerCube t;
layout (binding = 1) uniform sampler2DShadow shadowTex;

void main(void)
{
	vVertPos = (mv_matrix * vec4(position,1.0)).xyz;
	vNormal = (normalMat * vec4(normal,1.0)).xyz;
	vLightDir = lightPos - vVertPos;

	shadow_coord = shadowMVP * vec4(position, 1.0);

	gl_Position = proj_matrix * mv_matrix * vec4(position,1.0);
}
