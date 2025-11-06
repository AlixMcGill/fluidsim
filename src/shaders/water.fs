#version 330

uniform sampler2D u_particleTex;
uniform int u_pointCount;
uniform vec2 u_resolution;

out vec4 fragColor;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = u_resolution.y - fragCoord.y; // flip Y

    vec2 centerPoint = vec2(u_resolution.x * 0.5, u_resolution.y * 0.8);

    float density = 0.0;

    // iterate over all particles
    for (int i = 0; i < u_pointCount; i++) {
        float u = (float(i) + 0.5) / float(u_pointCount);
        vec3 particle = texture(u_particleTex, vec2(u, 0.5)).xyz;

        float dist = length(fragCoord - particle.xy);
        density += exp(-dist*dist / (particle.z * 100.0)); // gaussian contribution
    }

    // optional: scale density for brightness
    density = 1.0 - exp(-density * 0.05); // soft saturation

    vec3 non = vec3(0.05, 0.05, 0.05);  // dark bg
    vec3 water = vec3(0.3,0.3,1.0); // water color
    vec3 waterHighlight = vec3(0.4,0.4,1.0); // water highlight color
    vec3 darkWater = vec3(0.1,0.1,0.8);

    vec3 color = non;
    if (density > 0.001) {
        color = water;
        color = mix(color, waterHighlight, smoothstep(0.01, 0.001, density));
        color = mix(color, darkWater, smoothstep(0.07, 1.0, density));
    }

    fragColor = vec4(color, 1.0);
}
