#version 330 core

uniform sampler2D u_particleTex; // particle positions + radius
uniform int u_pointCount;
uniform vec2 u_resolution;

out vec4 fragColor;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = u_resolution.y - fragCoord.y; // flips y axis

    // Background color
    vec3 color = vec3(0.0); // black

    // Iterate over all particles
    for (int i = 0; i < u_pointCount; i++) {
        float u = (float(i) + 0.5) / float(u_pointCount);
        vec3 particle = texture(u_particleTex, vec2(u, 0.5)).xyz; 
        vec2 pos = particle.xy;
        float radius = particle.z; // particle.z is radius

        // Draw a circle
        float dist = length(fragCoord - pos);
        if (dist < radius) {
            color = vec3(1.0, 1.0, 1.0); // white particle
        }
    }

    fragColor = vec4(color, 1.0);
}


