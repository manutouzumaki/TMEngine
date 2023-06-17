#version 410

uniform sampler2D uTexture;

struct PointLight {
    vec3 attributes;
    float range;
    vec3 color;
    float pad0;
    vec2 position;
    float pad1;
    float pad2;
};

 uniform CLightBuffer {
    vec3 srcAmbient;
    int count;
    PointLight lights[100];
};

layout (location = 3) in vec2 Tex0;
layout (location = 4) in vec3 Norm;
layout (location = 5) in vec4 Color;
layout (location = 6) in vec3 FragPos;

layout (location = 0) out vec4 outColor;

void main() {

    vec4 textureColor = texture(uTexture, Tex0);

    vec3 ambient = srcAmbient * textureColor.rgb;

    if(count > 0) {
        vec3 output0 = ambient;

        for(int i = 0; i < count; ++i)
        {
            vec2 position   = lights[i].position;
            vec3 lightPos = vec3(position.x, position.y, 1);
            vec3 fragPos = FragPos;
            fragPos.z = 1;
            float distance0 = length(lightPos - fragPos);

            float range = lights[i].range;
            if(distance0 > range) continue;

            vec3 attributes = lights[i].attributes;
            vec3 color      = lights[i].color;

            vec3 diffuse = color;
            
            float constant  = attributes.x;
            float linear0   = attributes.y;
            float quadratic = attributes.z;

            float attenuation = 1.0 / (constant + linear0 * distance0 + quadratic * (distance0 * distance0));  
            
            diffuse  *= attenuation;

            output0 += diffuse;
        }

        textureColor.rgb = output0;
    }

   outColor = textureColor * Color;
}
