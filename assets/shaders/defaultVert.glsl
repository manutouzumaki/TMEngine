#version 410

uniform mtx {
    mat4 uProj;
    mat4 uView;
    mat4 uWorld;
    vec4 color;
    vec4 absUVs;
    vec4 relUVs;
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTex0;
layout (location = 2) in vec3 inNormal;

layout (location = 3) out vec2 Tex0;
layout (location = 4) out vec3 Norm;
layout (location = 5) out vec4 Color;
layout (location = 6) out vec3 FragPos;

void main() {


    Norm = mat3(uWorld) * inNormal;
    Norm = normalize(Norm);

    Color = color;

    float uSize = absUVs.z - absUVs.x;
    float vSize = absUVs.w - absUVs.y;

    float ut = inTex0.x;
    float vt = inTex0.y;

    float relU = (1.0f - ut)*relUVs.x + ut*relUVs.z;
    float relV = (1.0f - vt)*relUVs.y + vt*relUVs.w;

    float u = absUVs.x + uSize * relU;
    float v = absUVs.y + vSize * relV;

    Tex0 = vec2(u, v);

    FragPos = (uWorld * vec4(inPosition, 1.0f)).xyz;


    gl_Position = uProj * uView * uWorld * vec4(inPosition, 1.0);

}
