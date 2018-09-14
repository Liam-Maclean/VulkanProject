#version 450
#extension GL_ARB_separate_shader_objects : enable

layout (binding = 1) uniform sampler2D texSampler;

layout(location = 0) in vec3 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 lightDiffuse;
layout(location = 3) in vec3 lightDirection;


layout(location = 0) out vec4 outColor;

void main() {

	vec4 color;
	vec3 lightdir;
	vec4 lightColor;
	vec4 textureColor;
	float lightIntensity;
	
	textureColor = texture(texSampler, fragTexCoord);
	
	lightdir = -lightDirection;
	
	lightIntensity = clamp(dot(vec3(0,-1, 0), lightdir), 0.0f, 1.0f);
	
	color = (lightDiffuse * lightIntensity);
	
	outColor = color * textureColor
}