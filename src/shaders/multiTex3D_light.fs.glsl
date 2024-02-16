#version 330 core

in vec3 vPosition_vs; // Position du sommet transformé dans l'espace View
in vec3 vNormal_vs; // Normale du sommet transformé dans l'espace View
in vec2 vTexCoords; // Coordonnées de texture du sommet

out vec3 fFragColor;

uniform sampler2D uTexture0;
uniform sampler2D uTexture1;
uniform vec3 uKd;
uniform vec3 uKs;
uniform float uShininess;
uniform vec3 uLightDir_vs;
uniform vec3 uLightIntensity;


vec3 blinnPhong() {
    vec3 Li = normalize(uLightIntensity);
    vec3 wo = normalize(-1.0*vPosition_vs);
    vec3 wi = normalize(uLightDir_vs);
    vec3 halfVector = normalize((wo + wi) / 2.0);
    return Li * ((normalize(uKd) * (dot(wi, normalize(vNormal_vs)))) +
            (normalize(uKs) * (pow(dot(halfVector, normalize(vNormal_vs)), uShininess))));
}


void main() {
    vec4 planetaryTex = texture(uTexture0, vTexCoords);
    vec4 cloudTex = texture(uTexture1, vTexCoords);
    fFragColor = (vec3(planetaryTex.x, planetaryTex.y, planetaryTex.z)
                    + vec3(cloudTex.x, cloudTex.y, cloudTex.z)) * blinnPhong() * 2.0;
}
