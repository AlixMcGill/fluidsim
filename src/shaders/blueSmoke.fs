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
        density += exp(-dist*dist / (particle.z * 200.0)); // gaussian contribution
    }

    // optional: scale density for brightness
    density = 1.0 - exp(-density * 0.05); // soft saturation

    //vec3 color = mix(vec3(0.0,0.0,0.0), vec3(0.5,0.5,0.5), density);

    vec3 non = vec3(0.05, 0.05, 0.05);  // dark smoke
    vec3 smoke = vec3(0.1,0.1,0.5);
    vec3 ember = vec3(0.6, 0.0, 0.0);     // red embers
    vec3 flame = vec3(1.0, 0.5, 0.0);     // orange flame
    vec3 core  = vec3(1.0, 0.9, 0.6);     // white/yellow hot core

    vec3 color = non;
    color = mix(color, smoke, smoothstep(0.1, 0.6, density));
    color = mix(color, ember, smoothstep(0.6, 0.75, density));
    color = mix(color, flame, smoothstep(0.75, 0.85, density));
    color = mix(color, core,  smoothstep(0.85, 1.00, density));

    /*float distFromCenter = length(fragCoord - centerPoint);
    float whiteness = smoothstep(30.0, 150.0, distFromCenter);

    float fireMask = smoothstep(0.2, 0.1, density);

    color = mix(color, vec3(0.3), whiteness - fireMask);*/

    fragColor = vec4(color, 1.0);
}
