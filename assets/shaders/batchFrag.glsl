#version 410

uniform sampler2D uTexture;

layout (location = 2) in vec2 fragUV;

layout (location = 0) out vec4 outColor;

void main() {
    vec4 textureColor = texture(uTexture, fragUV);
    outColor = textureColor;
}
