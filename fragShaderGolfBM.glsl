#version 430

in vec3 varyingNormal;
in vec3 varyingLightDir;
in vec3 varyingVertPos;

in vec3 originalVertex;

out vec4 fragColor;

struct PositionalLight
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	vec3 position;
};

struct Material
{	vec4 ambient;  
	vec4 diffuse;  
	vec4 specular;  
	float shininess;
};

uniform vec4 globalAmbient;
uniform PositionalLight light;
uniform Material material;
uniform mat4 mv_matrix;	 
uniform mat4 proj_matrix;
uniform mat4 normalMat;

void main(void)
{	// normalize the light, normal, and view vectors:
	vec3 L = normalize(varyingLightDir);
	vec3 N = normalize(varyingNormal);
	vec3 V = normalize(-varyingVertPos);
	
	float a = 0.35;		// controls depth of bumps
	float b = 50.0;	// controls width of bumps
	float x = originalVertex.x;
	float y = originalVertex.y;
	float z = originalVertex.z;
	N.x = varyingNormal.x + a*sin(b*x);   // After playing around with the different sinusodial functions, tan^2() seems to be the most interesting looking.
	N.y = varyingNormal.y + a*sin(b*y);
	N.z = varyingNormal.z + a*sin(b*z);
	N = normalize(N);
	
	// compute light reflection vector, with respect N:
	vec3 R = normalize(reflect(-L, N));
	
	// get the angle between the light and surface normal:
	float cosTheta = dot(L,N);
	
	// angle between the view vector and reflected light:
	float cosPhi = dot(V,R);

	float spotlightCutoff = 173;

	float lightCone = degrees(acos(dot(L, vec3(0.0, 0.0, -1.0))));

	if(lightCone > spotlightCutoff){

	// compute ADS contributions (per pixel):
	fragColor = globalAmbient
	+ light.ambient * material.ambient
	+ light.diffuse * material.diffuse * max(cosTheta,0.0)
	+ light.specular  * material.specular *
		pow(max(cosPhi,0.0), material.shininess);
	}
	else{
	fragColor = (globalAmbient
	+ light.ambient * material.ambient
	+ light.diffuse * material.diffuse * max(cosTheta,0.0)
	+ light.specular  * material.specular *
		pow(max(cosPhi,0.0), material.shininess))*0.1 + vec4(0.0, 0.0, 0.0, 1.0);

	}
}
