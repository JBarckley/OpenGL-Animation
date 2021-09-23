#version 430

in vec3 vNormal;
in vec3 vVertPos;
in vec3 vLightDir;
in vec4 shadow_coord;

out vec4 fragColor;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;
uniform mat4 normalMat;
layout (binding = 0) uniform samplerCube t;
layout (binding = 1) uniform sampler2DShadow shadowTex;

void main(void)
{

	float inShadow = textureProj(shadowTex, shadow_coord);
	vec3 r = -reflect(normalize(-vVertPos), normalize(vNormal));
	vec3 L = normalize(vLightDir);

	float spotlightCutoff = 173;

	float lightCone = degrees(acos(dot(L, vec3(0.0, 0.0, -1.0))));

	if(lightCone > spotlightCutoff){

		fragColor = texture(t, r);
	}
	else {
		fragColor = texture(t, r)*0.1 + vec4(0.0, 0.0, 0.0, 1.0);
	}

	if (inShadow == 0.0){
		//fragColor = vec4(1.0, 0.0, 1.0, 1.0);
		if(lightCone > spotlightCutoff){

			fragColor = texture(t, r) - vec4(0.15, 0.15, 0.15, 0.0);
		}
		else {
			fragColor = (texture(t, r) - vec4(0.15, 0.15, 0.15, 0.0))*0.1 + vec4(0.0, 0.0, 0.0, 1.0);
		}
		
	}
}