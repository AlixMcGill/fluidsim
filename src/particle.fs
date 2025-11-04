#version 330

uniform sampler2D u_particleTex;
uniform int u_pointCount;
uniform vec2 u_resolution;

out vec4 fragColor;

void main() {
    vec2 fragCoord = gl_FragCoord.xy;
    fragCoord.y = u_resolution.y - fragCoord.y; // flip Y

    float density = 0.0;

    // iterate over all particles
    for (int i = 0; i < u_pointCount; i++) {
        float u = (float(i) + 0.5) / float(u_pointCount);
        vec3 particle = texture(u_particleTex, vec2(u, 0.5)).xyz;

        float dist = length(fragCoord - particle.xy);
        density += exp(-dist*dist / (particle.z * 40.0)); // gaussian contribution
    }

    // optional: scale density for brightness
    density = 1.0 - exp(-density * 0.3); // soft saturation

    // color mapping: e.g., blue low, yellow high
    vec3 color = mix(vec3(0.1,0.1,0.1), vec3(0.6,0.6,0.7), density);

    fragColor = vec4(color, 1.0);
}

