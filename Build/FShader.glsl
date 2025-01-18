#version 330

in vec3 fragColor;          
in vec2 fragTexCoord;       

out vec4 color;             // Final color output

uniform sampler2D textureSampler; // Texture to sample from

void main() {
    // Sample the texture based on the texture coordinates
    vec4 texColor = texture(textureSampler, fragTexCoord);
    color = texColor * vec4(fragColor, 1.0);
}
