#version 430 core

layout(location = 0) in uint iData;

uniform mat4 u_ViewProjection;
uniform vec3 u_ChunkPos;
uniform sampler1D u_ColorTex;
uniform float u_ChunkSpacing = 1.f;

out vec4 vBlockColor;
out vec3 vFaceNormal;
out float vAO;// Ambient Occlusion per vertex

const vec3 FACE_QUAD[6][6] = vec3[6][6](
    vec3[6](vec3(0, 0, 1), vec3(1, 0, 1), vec3(1, 1, 1), vec3(0, 0, 1), vec3(1, 1, 1), vec3(0, 1, 1)), // Z+
    vec3[6](vec3(0, 0, 0), vec3(0, 1, 0), vec3(1, 1, 0), vec3(0, 0, 0), vec3(1, 1, 0), vec3(1, 0, 0)), // Z-
    vec3[6](vec3(1, 0, 0), vec3(1, 1, 0), vec3(1, 1, 1), vec3(1, 0, 0), vec3(1, 1, 1), vec3(1, 0, 1)), // X+
    vec3[6](vec3(0, 0, 1), vec3(0, 1, 1), vec3(0, 1, 0), vec3(0, 0, 1), vec3(0, 1, 0), vec3(0, 0, 0)), // X-
    vec3[6](vec3(0, 1, 0), vec3(0, 1, 1), vec3(1, 1, 1), vec3(0, 1, 0), vec3(1, 1, 1), vec3(1, 1, 0)), // Y+
    vec3[6](vec3(0, 0, 0), vec3(1, 0, 0), vec3(1, 0, 1), vec3(0, 0, 0), vec3(1, 0, 1), vec3(0, 0, 1))// Y-
);

const vec3 FACE_NORMALS[6] = vec3[6](
    vec3(0, 0, 1), // Z+
    vec3(0, 0, -1), // Z-
    vec3(1, 0, 0), // X+
    vec3(-1, 0, 0), // X-
    vec3(0, 1, 0), // Y+
    vec3(0, -1, 0)// Y-
);

void main() {
    uint x       = (iData >>  0u) & 31u;
    uint y       = (iData >>  5u) & 31u;
    uint z       = (iData >> 10u) & 31u;
    uint faceID  = (iData >> 15u) & 7u;
    uint voxelID = (iData >> 18u) & 255u;
    uint aoData  = (iData >> 26u) & 3u;// 2 bits pour AO (0-3)

    vec3 voxelPos = vec3(float(x), float(y), float(z));
    vec4 localPos = vec4(FACE_QUAD[faceID][gl_VertexID], 1.0);

    // Position monde
    vec4 worldPos = vec4(u_ChunkPos * u_ChunkSpacing + voxelPos, 0.0) + localPos;
    gl_Position = u_ViewProjection * vec4(worldPos.xyz, 1.0);

    // Couleur bloc
    float texSize = float(textureSize(u_ColorTex, 0));
    float t = (float(voxelID) + 0.5) / texSize;
    vBlockColor = texture(u_ColorTex, t);

    // Normale face
    vFaceNormal = FACE_NORMALS[faceID];

    // Ambient Occlusion : convertir 0-3 en facteur d'ombrage
    // 0 = coin complètement occlus (plus sombre)
    // 3 = pas d'occlusion (plus clair)
    vAO = 1.0 - (float(aoData) * 0.25);// 1.0, 0.75, 0.5, 0.25
}