#version 400 core

// Structure for matrices
uniform struct Matrices
{
	mat4 projMatrix;
	mat4 modelViewMatrix; 
	mat3 normalMatrix;
} matrices;

// Structure holding light information:  its position as well as ambient, diffuse, and specular colours
struct LightInfo
{
	vec4 position;
	vec3 La;
	vec3 Ld;
	vec3 Ls;
	vec3 direction;
	float exponent;
	float cutoff;
};

// Structure holding material information:  its ambient, diffuse, and specular colours, and shininess
struct MaterialInfo
{
	vec3 Ma;
	vec3 Md;
	vec3 Ms;
	float shininess;
};

// Lights and materials passed in as uniform variables from client programme
uniform LightInfo light1; 
uniform LightInfo light2; 
uniform LightInfo light3;
uniform LightInfo light4;
uniform LightInfo light5;
uniform LightInfo light6;
uniform LightInfo light7;
uniform MaterialInfo material1; 
uniform MaterialInfo material2;
uniform MaterialInfo material3;
uniform MaterialInfo material4;
uniform MaterialInfo material5;
uniform MaterialInfo material6;
uniform MaterialInfo material7;
uniform float t;

// Layout of vertex attributes in VBO
layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inCoord;
layout (location = 2) in vec3 inNormal;

// Vertex colour output to fragment shader -- using Gouraud (interpolated) shading
out vec3 vColour;	// Colour computed using reflectance model
out vec2 vTexCoord;	// Texture coordinate
out vec3 worldPosition;	// used for skybox

vec3 BlinnPhongSpotlightModel(LightInfo light, vec4 p, vec3 n, MaterialInfo material){
	vec3 s = normalize(vec3(light.position - p));
	float angle = acos(dot(-s, light.direction));
	float cutoff = radians(clamp(light.cutoff, 0.0, 90.0));
	vec3 ambient = light.La * material.Ma;
	if (angle < cutoff) {
		float spotFactor = pow(dot(-s, light.direction), light.exponent);
		vec3 v = normalize(-p.xyz);
		vec3 h = normalize(v + s);
		float sDotN = max(dot(s, n), 0.0);
		vec3 diffuse = light.Ld * material.Md * sDotN;
		vec3 specular = vec3(0.0);
		if (sDotN > 0.0)
			specular = light.Ls * material.Ms * pow(max(dot(h, n), 0.0), material.shininess);
		return ambient + spotFactor * (diffuse + specular);
	} else 
		return ambient;
}

// This function implements the Phong shading model
// The code is based on the OpenGL 4.0 Shading Language Cookbook, Chapter 2, pp. 62 - 63, with a few tweaks. 
// Please see Chapter 2 of the book for a detailed discussion.
vec3 PhongModel(vec4 eyePosition, vec3 eyeNorm){
	vec3 s = normalize(vec3(light1.position - eyePosition));
	vec3 v = normalize(-eyePosition.xyz);
	vec3 r = reflect(-s, eyeNorm);
	vec3 n = eyeNorm;
	vec3 ambient = light1.La * material1.Ma;
	float sDotN = max(dot(s, n), 0.0f);
	vec3 diffuse = light1.Ld * material1.Md * sDotN;
	vec3 specular = vec3(0.0f);
	float eps = 0.000001f; // add eps to shininess below -- pow not defined if second argument is 0 (as described in GLSL documentation)
	if (sDotN > 0.0f) 
		specular = light1.Ls * material1.Ms * pow(max(dot(r, v), 0.0f), material1.shininess + eps);
	return ambient + diffuse + specular;
}

// This is the entry point into the vertex shader
void main()
{	
	
	// Save the world position for rendering the skybox
	worldPosition = inPosition;

	// Transform the vertex spatial position using 
	gl_Position = matrices.projMatrix * matrices.modelViewMatrix * vec4(inPosition, 1.0f);
	
	// Get the vertex normal and vertex position in eye coordinates
	vec3 vEyeNorm = normalize(matrices.normalMatrix * inNormal);
	vec4 vEyePosition = matrices.modelViewMatrix * vec4(inPosition, 1.0f);

	// Fog
	vec3 fogColor = vec3(0.1, 0.1, 0.1);
	float fogStart = 200; float fogEnd = 300;
	float dist = length(vEyePosition);
	float fogFactor = clamp(((fogEnd - dist)/(fogEnd-fogStart)), 0.0, 1.0);


	// Apply the BlinnPhong model to compute the vertex colour
	vColour =	BlinnPhongSpotlightModel (light1, vEyePosition, normalize(vEyeNorm), material1) +
				(0.7 + 0.3 * sin(0.1*t)) * BlinnPhongSpotlightModel (light2, vEyePosition, normalize(vEyeNorm), material2) + 
				(0.7 + 0.3 * sin(0.1*t)) * BlinnPhongSpotlightModel (light3, vEyePosition, normalize(vEyeNorm), material3) + 
				(0.7 + 0.3 * sin(0.1*t)) * BlinnPhongSpotlightModel (light4, vEyePosition, normalize(vEyeNorm), material4) +
				(0.7 + 0.3 * sin(0.1*t)) * BlinnPhongSpotlightModel (light5, vEyePosition, normalize(vEyeNorm), material5) +
										   BlinnPhongSpotlightModel (light6, vEyePosition, normalize(vEyeNorm), material6) +
										   BlinnPhongSpotlightModel (light7, vEyePosition, normalize(vEyeNorm), material7) ;

										
	vColour = mix(fogColor, vColour, fogFactor); // Fog interfering with spotlights but more realistic. It is mixed with the other lights

	// Pass through the texture coordinate
	vTexCoord = inCoord;
} 
	