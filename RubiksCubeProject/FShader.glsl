#version 330

in vec3 fragColor;
in vec2 fragTexCoord;

out vec4 color;

uniform sampler2D textureSampler;

void main() {
    vec4 texColor = texture(textureSampler, fragTexCoord);

    // Detect outline based on UV position
    float borderThreshold = 0.1;
    bool isBorder = fragTexCoord.x < borderThreshold || fragTexCoord.x > 1.0 - borderThreshold ||
                    fragTexCoord.y < borderThreshold || fragTexCoord.y > 1.0 - borderThreshold;

    if (isBorder) {
        color = texColor;
    } else {
        color = vec4(fragColor, 1.0);
    }
}
