#version 330 core

out vec4 FragColor;
in vec2 TexCoord;

uniform sampler2D heatmap;

const vec2 gridResolution = vec2(100.0, 100.0); 

const float lineThickness = 1.0; 

void main()
{
    float t = texture(heatmap, TexCoord).r;
    
    // 1. Your original Jet color ramp
    vec3 color;
    color.r = clamp(4.0 * t - 1.5, 0.0, 1.0);
    color.g = clamp(4.0 * t - 0.5, 0.0, 1.0) - clamp(4.0 * t - 3.5, 0.0, 1.0);
    color.b = clamp(1.5 - 4.0 * t, 0.0, 1.0);

    // 2. Calculate the grid lines
    // Scale UV coordinates to the total number of grid cells
    vec2 uvGrid = TexCoord * gridResolution;
    
    // Find the distance from the center of the pixel to the nearest grid edge
    vec2 gridDist = abs(fract(uvGrid - 0.5) - 0.5) / fwidth(uvGrid);
    
    // Pick the closest edge (X or Y)
    float edgeIntensity = min(gridDist.x, gridDist.y);
    
    // Smooth the line edges to avoid ugly aliasing (jagged lines)
    float lineFactor = smoothstep(0.0, lineThickness, edgeIntensity);

    // 3. Blend the MATLAB black wireframe over your Jet color
    // lineFactor is 0.0 exactly on the grid line, and 1.0 inside the cell faces
    vec3 wireframeColor = vec3(0.1, 0.1, 0.1); // Dark charcoal grey/black
    vec3 finalColor = mix(wireframeColor, color, lineFactor);

    FragColor = vec4(finalColor, 1.0);
}
