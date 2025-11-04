#version 330 core

layout(location = 0) in vec2 vertexPos;
layout(location = 1) in vec4 vertexColor;
layout(location = 2) in vec2 vertexTexCoord;

out vec2 fragTexCoord;
out vec4 fragColor;

uniform vec2 screenSize;

void main() {
    fragColor = vertexColor;
    fragTexCoord = vertexTexCoord;

    // Convert from screen space to NDC [-1,1]
    vec2 ndc = (vertexPos / screenSize) * 2.0 - 1.0;
    ndc.y = -ndc.y; // flip Y
    gl_Position = vec4(ndc, 0.0, 1.0);
}


