#version 330

in vec3 fragColor;          // The color of the cube side
in vec2 fragTexCoord;       // Texture coordinates

out vec4 color;             // Final color output

uniform sampler2D textureSampler; // Texture to sample from

void main() {
    // Sample the texture based on the texture coordinates
    vec4 texColor = texture(textureSampler, fragTexCoord);

    // Apply the color from the vertex, multiplied by the texture color for a more dynamic effect
    // If you want to draw only the color, replace texColor with fragColor, but we combine them here.
    color = texColor * vec4(fragColor, 1.0);
}
