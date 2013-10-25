#version 120
varying vec3 normal;
varying vec4 vertexPos;
uniform sampler2D heightMap;  // The only sampler2D uniform is auto-bound to the only texture

// Gamma encoding
vec4 srgb(vec4 color) {
    return pow(color, vec4(1/2.2));
}

// Blinn model for a single light source
vec4 blinn(gl_LightSourceParameters light) {
    vec3 n = normalize(normal);
    vec3 l = normalize(light.position - vertexPos).xyz;
    float d = length(light.position - vertexPos);
    vec3 v = -normalize(vertexPos.xyz);
    vec3 h = normalize((l+v)/2.0);

    float attenuation = 1/(light.constantAttenuation +
                           light.linearAttenuation * d +
                           light.quadraticAttenuation * d * d);

    return   attenuation * (
                gl_FrontMaterial.diffuse*light.diffuse*max(dot(l, n), 0) +
                gl_FrontMaterial.specular*light.specular*pow(max(dot(h, n), 0), gl_FrontMaterial.shininess)
             );
}

void main(void) {
    // Basic lighting
    vec4 c = gl_FrontMaterial.ambient * gl_LightModel.ambient + blinn(gl_LightSource[0]);

    // Encode & output
    gl_FragColor = srgb(c);
}
