#version 430 //GLSL version your computer supports

//out vec4 daColor;
in vec3 normalWorld;
in vec3 vertexPositionWorld;
in vec3 theColor;
in vec2 UV;

uniform vec3 emissionLight;
uniform vec4 ambientLight;
uniform vec3 lightPositionWorld;
uniform vec3 lightPositionWorld_1;
uniform vec3 eyePositionWorld;
uniform sampler2D myTextureSampler;
uniform sampler2D myTextureSampler_1;
//uniform bool normalMapping_flag;
uniform float DiffuseIntensity;
uniform float DiffuseIntensity_1;
uniform float SpecularIntensity;
uniform float SpecularIntensity_1;
uniform vec4 lightColor;
out vec4 finalColor;

void main()
{
	// Normal Mapping
	vec3 normal = normalize(normalWorld);
	/*
	if(normalMapping_flag){
		normal = texture(myTextureSampler_1, UV).rgb;
		normal = normalize(normal * 2.0 - 1.0);

	//Material Color
	vec4 MaterialAmbientColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 MaterialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 MaterialSpecularColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0); //vec4(0.3, 0.3, 0.3, 0.3);

	//Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normal); //normalize(normalWorld)
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
	//Diffuse_1
	vec3 lightVectorWorld_1 = normalize(lightPositionWorld_1 - vertexPositionWorld);
	float brightness_1 = dot(lightVectorWorld_1, normal); //normalize(normalWorld)
	vec4 diffuseLight_1 = vec4(brightness_1, brightness_1, brightness_1, 1.0) * lightColor;

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normal); //normalWorld
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	vec3 halfwayDir = normalize(lightVectorWorld + eyeVectorWorld);
	float s = clamp(dot(normal, halfwayDir), 0, 1);
	s = pow(s, 50);
	vec4 specularLight = vec4(s, s, s, 1);
	//Specular_1
	vec3 reflectedLightVectorWorld_1 = reflect(-lightVectorWorld_1, normal); //normalWorld
	vec3 eyeVectorWorld_1 = normalize(eyePositionWorld - vertexPositionWorld);
	vec3 halfwayDir_1 = normalize(lightVectorWorld_1 + eyeVectorWorld_1);
	float s_1 = clamp(dot(normal, halfwayDir_1), 0, 1);
	s_1 = pow(s_1, 50);
	vec4 specularLight_1 = vec4(s_1, s_1, s_1, 1);

	finalColor = vec4(emissionLight, 1.0) * MaterialAmbientColor + MaterialAmbientColor * ambientLight + DiffuseIntensity * MaterialDiffuseColor * clamp(diffuseLight, 0, 1) +SpecularIntensity * MaterialSpecularColor * clamp(specularLight, 0, 1);	
	finalColor = finalColor + vec4(emissionLight, 1.0) * MaterialAmbientColor + MaterialAmbientColor * ambientLight + DiffuseIntensity_1 * MaterialDiffuseColor * lightColor * clamp(diffuseLight_1, 0, 1) + SpecularIntensity_1 * MaterialSpecularColor * lightColor * clamp(specularLight_1, 0, 1);
	}
	else{
	*/
	//Material Color
	vec4 MaterialAmbientColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 MaterialDiffuseColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0);
	vec4 MaterialSpecularColor = vec4(texture( myTextureSampler, UV ).rgb, 1.0); //vec4(0.3, 0.3, 0.3, 0.3);

	//Diffuse
	vec3 lightVectorWorld = normalize(lightPositionWorld - vertexPositionWorld);
	float brightness = dot(lightVectorWorld, normalize(normalWorld)); //normalize(normalWorld)
	vec4 diffuseLight = vec4(brightness, brightness, brightness, 1.0);
	//Diffuse_1
	vec3 lightVectorWorld_1 = normalize(lightPositionWorld_1 - vertexPositionWorld);
	float brightness_1 = dot(lightVectorWorld_1, normalize(normalWorld)); //normalize(normalWorld)
	vec4 diffuseLight_1 = vec4(brightness_1, brightness_1, brightness_1, 1.0) * lightColor;

	//Specular
	vec3 reflectedLightVectorWorld = reflect(-lightVectorWorld, normalWorld); //normalWorld
	vec3 eyeVectorWorld = normalize(eyePositionWorld - vertexPositionWorld);
	float s = clamp(dot(reflectedLightVectorWorld, eyeVectorWorld), 0, 1);
	s = pow(s, 50);
	vec4 specularLight = vec4(s, s, s, 1);
	//Specular_1
	vec3 reflectedLightVectorWorld_1 = reflect(-lightVectorWorld_1, normalWorld); //normalWorld
	vec3 eyeVectorWorld_1 = normalize(eyePositionWorld - vertexPositionWorld);
	float s_1 = clamp(dot(reflectedLightVectorWorld_1, eyeVectorWorld_1), 0, 1);
	s_1 = pow(s_1, 50);
	vec4 specularLight_1 = vec4(s_1, s_1, s_1, 1);

	finalColor = vec4(emissionLight, 1.0) * MaterialAmbientColor + MaterialAmbientColor * ambientLight + DiffuseIntensity * MaterialDiffuseColor * clamp(diffuseLight, 0, 1) +SpecularIntensity * MaterialSpecularColor * clamp(specularLight, 0, 1);	
	finalColor = finalColor + vec4(emissionLight, 1.0) * MaterialAmbientColor + MaterialAmbientColor * ambientLight + DiffuseIntensity_1 * MaterialDiffuseColor * lightColor * clamp(diffuseLight_1, 0, 1) + SpecularIntensity_1 * MaterialSpecularColor * lightColor * clamp(specularLight_1, 0, 1);
	//}
}
