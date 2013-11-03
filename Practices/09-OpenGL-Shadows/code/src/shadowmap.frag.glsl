#version 120
varying vec3 normal;
varying vec4 vertexPos;
varying vec4 color;

// Uniforms
uniform vec3 viewer_position;  // In this example we do lighting computations in the world frame,
                               // hence we need to know where the viewer is (it is not at (0, 0, 0))
uniform int no_lighting;       // Tells the shader to return gl_Color rather than perform lighting computations

uniform mat4 shadowMapMatrix[2]; // Shadow map view-projection transforms for each of the two light sources

uniform sampler2D shadowMapTexture0; // In case you were wondering: no, it is not possible to have an array of samplers.
uniform sampler2D shadowMapTexture1; // To be more precise, starting from GLSL 1.3 you *can* have an array, but you may not iterate over it. Iteration is possible only starting with GLSL 4.0.

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
        //   * Rescale the resulting ("clip-space") x, y, z coordinates from the range [-1, 1], to the
        //     texture-space range [0, 1]. (Note that the z will then become "depth" as in the depth buffer).
        //   * Find out the depth value that was rendered in the corresponding texture for the current location
        //   * Compare the "current pixel" and "texture" depths, and if the pixel's depth value is smaller or equal to
        //     the one in the texture, consider pixel to be lit by the corresponding light source.
        //
        // Hint: Despite the lingering description, the overall number of lines of code you need to add here is around 5-10.

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
