#version 430 core

in vec4 vBlockColor;
in vec3 vFaceNormal;
in float vAO;

out vec4 FragColor;

const vec3 lightDir = normalize(vec3(0.5, 1.0, 0.3));
const float minLight = 0.3;
const float maxLight = 1.0;

void main() {
    // Éclairage directionnel
    float diff = max(dot(normalize(vFaceNormal), lightDir), 0.0);
    diff = mix(minLight, maxLight, diff);

    // Appliquer l'ambient occlusion
    // L'AO assombrit les coins et arêtes
    float aoFactor = mix(0.4, 1.0, vAO);// Plus sombre dans les coins

    vec3 color = vBlockColor.rgb * diff * aoFactor;

    FragColor = vec4(color, vBlockColor.a);
}