#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D heatmap;

void main()
{
    float t = texture(heatmap, TexCoord).r;
    
    // Classic "Jet" or "Thermal" color ramp
    // Low (0.0) = Blue, Mid (0.5) = Green, High (1.0) = Red
    vec3 color;
    color.r = clamp(4.0 * t - 1.5, 0.0, 1.0);
    color.g = clamp(4.0 * t - 0.5, 0.0, 1.0) - clamp(4.0 * t - 3.5, 0.0, 1.0);
    color.b = clamp(1.5 - 4.0 * t, 0.0, 1.0);

    FragColor = vec4(color, 1.0);
}
