#version 330 core
out vec4 FragColor;

in vec2 texCoord;
uniform sampler2D texturaBase; 
uniform sampler2D texturaSticker; 
uniform sampler2D texturaSticker2; 

uniform bool usarSticker; 
uniform bool usarSticker2; 

uniform vec3 tintColor; 

void main()
{
    // 1. Dibuja la pared normal
    vec4 colorFinal = texture(texturaBase, texCoord) * vec4(tintColor, 1.0);
    
    // 2. Si hay un sticker 1, lo pega encima directamente en sus coordenadas originales
    if (usarSticker) {
        vec4 colorS1 = texture(texturaSticker, texCoord);
        colorFinal = mix(colorFinal, colorS1, colorS1.a);
    }
    
    // 3. Si hay un sticker 2, lo pega sobre lo anterior
    if (usarSticker2) {
        vec4 colorS2 = texture(texturaSticker2, texCoord);
        colorFinal = mix(colorFinal, colorS2, colorS2.a);
    }

    FragColor = colorFinal; //FragColor = vec4(tintColor, 1.0);
}