#version 460 core

struct Material {
    vec3 albedo;
    float metallic;
    float roughness;
};

uniform Material material;

in vec3 v_FragPos;
in vec3 v_Normal;
in vec2 v_TexCoord;

out vec4 FragColor;

void main()
{
    vec3 N = normalize(v_Normal);
    vec3 V = normalize(-v_FragPos); // caméra au centre
    vec3 L = normalize(vec3(0.5, 1.0, 0.3)); // lumière directionnelle
    vec3 H = normalize(V + L);

    // simple Lambert + metallic factor
    float NdotL = max(dot(N, L), 0.0);
    vec3 diffuse = material.albedo * NdotL;
    vec3 specular = vec3(0.0);
    FragColor = vec4(diffuse + specular, 1.0);
}
