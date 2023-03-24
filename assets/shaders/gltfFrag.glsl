#version 410
// #extension GL_ARB_separate_shader_objects : enable

uniform sampler2D uTexture;

layout (location = 3) in vec2 fragUV;
layout (location = 4) in vec3 fragNorm;

layout (location = 0) out vec4 outColor;

void main() {
    vec3 textureColor = texture(uTexture, fragUV).rgb;
    
    vec3 norm = normalize(fragNorm);
    vec3 lightDir = normalize(vec3(0.5, 0.2, -1.0));

    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 color = textureColor * diff;    

    outColor = vec4(color, 1.0);
}
