#version 410 core
in vec2 fragUV;
in vec3 fragNorm;

uniform sampler2D uTexture;

out vec4 outColor;

void main() {
    vec3 textureColor = texture(uTexture, fragUV).rgb;
    
    vec3 norm = normalize(fragNorm);
    vec3 lightDir = normalize(vec3(0.5, 0.2, 1.0));

    float diff = max(dot(norm, lightDir), 0.0);
    
    vec3 color = textureColor * diff;    

    outColor = vec4(color, 1.0);
}
