#version 330

in vec2 fragTexCoord;
in vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 uPalette[4];

out vec4 finalColor;

void main()
{
    vec4 texColor = texture(texture0, fragTexCoord);

    if(texColor.a == 0.0)
    {
        finalColor = texColor;
        return;
    }

    int colorIndex = 0;
    float intensity = texColor.r;

    if(intensity > 0.8) colorIndex = 1;
    else if(intensity > 0.5) colorIndex = 2;
    else if(intensity > 0.2) colorIndex = 3;
    else colorIndex = 0;

    finalColor = vec4(uPalette[colorIndex].rgb, texColor.a * fragColor.a);
}
