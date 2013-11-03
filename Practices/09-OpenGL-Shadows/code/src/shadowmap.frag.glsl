#version 120
varying vec3 normal;
varying vec4 vertexPos;
varying vec4 color;

// Uniforms
uniform vec3 viewer_position;  // In this example we do lighting computations in the world frame,
                               // hence we need to know where the viewer is (it is not at (0, 0, 0))
uniform int no_lighting;       // Tells the shader to return gl_Color rather than perform lighting computations

uniform mat4 shadowMapMatrix[2]; // Shadow map view-projection transforms for each of the two light sources

uniform sampler2D shadowMapTexture0; // Unfortunately, only starting from GLSL 4.00 is becomes possible
uniform sampler2D shadowMapTexture1; // (GLSL 1.3 allows to declare an array, but it is impossible to loop over it, so it's of no use)

// Gamma encoding
vec4 srgb(vec4 color) {
    return pow(color, vec4(1/2.2));
}

// Blinn model for a single light source
vec4 blinn(gl_LightSourceParameters light) {
    vec3 n = normalize(normal);
    vec3 l = normalize(light.position - vertexPos).xyz;
    float d = length(light.position - vertexPos);
    vec3 v = normalize(viewer_position - vertexPos.xyz);
    vec3 h = normalize(l+v);

    float attenuation = 1.0/(light.constantAttenuation +
                           light.linearAttenuation * d +
                           light.quadraticAttenuation * d * d);

    return   attenuation * (
                gl_FrontMaterial.diffuse*light.diffuse*max(dot(l, n), 0) +
                gl_FrontMaterial.specular*light.specular*pow(max(dot(h, n), 0), gl_FrontMaterial.shininess)
             );
}

void main(void) {
    if (no_lighting == 0) {
        // Ambient light
        vec4 c = gl_FrontMaterial.ambient * gl_LightModel.ambient;

        // YOUR CODE HERE: Implement the shadow mapping algorithm.
        // What you have:
        // * Textures shadowMapTexture0 and shadowMapTexture1 contain the depth buffers
        //   from the point of view of the first and the second light source.
        // * shadowMapMatrix[i] is the view-projection matrix used to render the depth buffer for light source i
        // * vertexPos is the world-coordinate of the fragment you are shading.
        // What you need:
        // * For each light source:
        //   * Compute the coordinates of vertexPos with respect to the light source's projective
        //     frustum. (NB: Don't forget perspective division!)
        //   * Map the resulting ("clip-space") z coordinate to a depth value
        //     by rescaling from [-1, 1] to [0, 1].
        //   * Find out the depth value that was rendered in the corresponding texture for the same location
        //   * Compare the two, and if the pixel's depth value is equal to (or just slightly greater) than
        //     the one in the texture, consider light source to be affecting that pixel position.
        //
        // Hint: Despite the lingering description, the overall number of lines of code you need to add here is around 6-10.

        // So far we just do the usual lighting computation.
        for (int i = 0; i < 2; i++) c += blinn(gl_LightSource[i]);

        // The line below does nothing useful. It is, however, needed so that GLSL compiler wouldn't
        // optimize some variables away due to them not being used (as this will crash the main program).
        // Delete it once you start *actually* using the texture variables and the shadow map matrices.
        gl_FragColor = texture2D(shadowMapTexture0, shadowMapMatrix[0][0].xy)*texture2D(shadowMapTexture1, vec2(0));

        // Encode & output
        gl_FragColor = srgb(vec4(c.xyz, 1.0));
    }
    else {
        gl_FragColor = color;
    }
}
