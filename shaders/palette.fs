#version 100

precision mediump float;

varying vec2 fragTexCoord;
varying vec4 fragColor;

uniform sampler2D texture0;
uniform vec4 uPalette[4];

void main()
{
    vec4 texColor = texture2D(texture0, fragTexCoord);

    if(texColor.a == 0.0)
    {
        gl_FragColor = texColor;
        return;
    }

    float intensity = texColor.r;
    vec4 mappedColor = uPalette[0];

    if(intensity > 0.8) mappedColor = uPalette[1];
    else if(intensity > 0.5) mappedColor = uPalette[2];
    else if(intensity > 0.2) mappedColor = uPalette[3];

    gl_FragColor = vec4(mappedColor.rgb, texColor.a * fragColor.a);
}
