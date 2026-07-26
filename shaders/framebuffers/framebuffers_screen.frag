#version 330 core

in vec2 TexCoords;

out vec4 FragColor;

uniform sampler2D screenTexture;
uniform int uEffect;
uniform float uKernelOffset;

// Kernel 常量
const float kernelSharpen[9] = float[](
    -1.0, -1.0, -1.0,
    -1.0,  9.0, -1.0,
    -1.0, -1.0, -1.0
);

const float kernelBlur[9] = float[](
    1.0/16.0, 2.0/16.0, 1.0/16.0,
    2.0/16.0, 4.0/16.0, 2.0/16.0,
    1.0/16.0, 2.0/16.0, 1.0/16.0
);

const float kernelEdge[9] = float[](
     1.0,  1.0,  1.0,
     1.0, -8.0,  1.0,
     1.0,  1.0,  1.0
);

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-1.0,  1.0), vec2(0.0,  1.0), vec2( 1.0,  1.0),
        vec2(-1.0,  0.0), vec2(0.0,  0.0), vec2( 1.0,  0.0),
        vec2(-1.0, -1.0), vec2(0.0, -1.0), vec2( 1.0, -1.0)
    );

    if (uEffect == 0)  // Normal
    {
        FragColor = texture(screenTexture, TexCoords);
    }
    else if (uEffect == 1)  // Invert
    {
        FragColor = vec4(vec3(1.0 - texture(screenTexture, TexCoords)), 1.0);
    }
    else if (uEffect == 2)  // Simple Grayscale
    {
        FragColor = texture(screenTexture, TexCoords);
        float avg = (FragColor.r + FragColor.g + FragColor.b) / 3.0;
        FragColor = vec4(avg, avg, avg, 1.0);
    }
    else if (uEffect == 3)  // Weighted Grayscale
    {
        FragColor = texture(screenTexture, TexCoords);
        float gray = 0.2126 * FragColor.r + 0.7152 * FragColor.g + 0.0722 * FragColor.b;
        FragColor = vec4(gray, gray, gray, 1.0);
    }
    else if (uEffect == 4)  // Sharpen
    {
        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
            sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i] * uKernelOffset));

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
            col += sampleTex[i] * kernelSharpen[i];
        FragColor = vec4(col, 1.0);
    }
    else if (uEffect == 5)  // Blur
    {
        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
            sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i] * uKernelOffset));

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
            col += sampleTex[i] * kernelBlur[i];
        FragColor = vec4(col, 1.0);
    }
    else if (uEffect == 6)  // Edge Detection
    {
        vec3 sampleTex[9];
        for (int i = 0; i < 9; i++)
            sampleTex[i] = vec3(texture(screenTexture, TexCoords + offsets[i] * uKernelOffset));

        vec3 col = vec3(0.0);
        for (int i = 0; i < 9; i++)
            col += sampleTex[i] * kernelEdge[i];
        FragColor = vec4(col, 1.0);
    }
    else
    {
        FragColor = texture(screenTexture, TexCoords);
    }
}
