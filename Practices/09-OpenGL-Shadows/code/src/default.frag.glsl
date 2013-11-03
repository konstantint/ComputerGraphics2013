#version 120
varying vec3 normal;
varying vec4 vertexPos;
varying vec4 color;
uniform int no_lighting; // Return gl_Color rather than perform lighting computations
uniform int no_ambient;  // Drop the ambient term from the lighting model
uniform int no_light0;   // Do not use the light0 term
uniform int no_light1;   // Do not use the light1 term

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
    // Note: for purposes of efficiency we could have computed l, v and h in the
    // vertex shader and would only need to renormalize those here.

    float attenuation = 1/(light.constantAttenuation +
                           light.linearAttenuation * d +
                           light.quadraticAttenuation * d * d);

    return   attenuation * (
                gl_FrontMaterial.diffuse*light.diffuse*max(dot(l, n), 0) +
                gl_FrontMaterial.specular*light.specular*pow(max(dot(h, n), 0), gl_FrontMaterial.shininess)
             );
}

void main(void) {
    if (no_lighting == 0) {
        // Basic lighting (assuming we only use two light sources)
        vec4 c = vec4(0);
        if (no_ambient == 0)
            c += gl_FrontMaterial.ambient * gl_LightModel.ambient;
        if (no_light0 == 0)
            c += blinn(gl_LightSource[0]);
        if (no_light1 == 0)
            c += blinn(gl_LightSource[1]);

        // Encode & output
        gl_FragColor = srgb(vec4(c.xyz, 1.0));
    }
    else {
        gl_FragColor = color;
    }
}
