#version 330

uniform vec2 u_resolution;

// Struct array can't be used directly in raylib uniform uploads,
// So we send each point as vec3: (x, y, radius)
uniform vec3 u_points[1024];
uniform int u_pointCount;

out vec4 fragColor;

void main()
{
    vec2 uv = gl_FragCoord.xy;

    float minDist = 99999.0;
    int index = -1;

    for (int i = 0; i < u_pointCount; i++)
    {
        vec2 p = u_points[i].xy;
        float r = u_points[i].z;

        float dist = length(uv - p) - r;

        if (dist < minDist) {
            minDist = dist;
            index = i;
        }
    }

    // Signed distance threshold
    if (minDist <= 0.0) {
        fragColor = vec4(1.0, 0.6, 0.1, 1.0); // Orange circle
    } else {
        fragColor = vec4(0.0, 0.0, 0.0, 0.0); // Transparent background
    }
}

