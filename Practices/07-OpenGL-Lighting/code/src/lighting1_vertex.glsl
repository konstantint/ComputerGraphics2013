#version 120
// This is the "color" output of the shader, that we have to compute.
// Note that the newer equivalent declaration is "smooth out vec4 vertex_color",
// but we stick to GLSL 1.2 to support older drivers/laptops.
varying vec4 vertex_color;
uniform int disable_lighting;

void main(void) {
    gl_Position = ftransform();
    if (disable_lighting == 1) {
        vertex_color = gl_Color;
        return;
    }

    // In the rest of the code you have to compute the value of the
    // vertex_color variable, that will be sent further to the fragment shader.

    // To do that you can use the built-in variables such as gl_LightSource[], etc
    // See http://mew.cx/glsl_quickref.pdf
    // Note that those are all removed from newer version of OpenGL, so you would
    //   have to specify and pass those variables to the shader manually as uniforms.

    // I'll show you how to get started by computing the contributions to vertex color that are due
    // to the overall ambient light and the spotlight.

    // Your task will be to add the light contributions of the directional "sun" and point-light "cube".

    // First compute some useful vectors:
    vec3 normal = normalize(gl_NormalMatrix * gl_Normal);
    vec3 viewer = vec3(0, 0, 1);  // Direction to the viewer (corresponds to GL_LIGHT_MODEL_LOCAL_VIEWER=false)

    // --------- Ambient lighting ---------
    vertex_color = gl_LightModel.ambient * gl_FrontMaterial.ambient;

    // --------- The spotlight ----------
    // Direction vector to the light source
    vec3 light2_direction = (gl_LightSource[2].position - gl_ModelViewMatrix*gl_Vertex).xyz;

    // Normalized direction vector
    vec3 light2 = normalize(light2_direction);

    // Spotlight attenuation = <angle of light ray, spot_direction> ^ spot_exponent
    float light2_attenuation = pow(clamp(dot(-light2, gl_LightSource[2].spotDirection), 0, 1), gl_LightSource[2].spotExponent);
    // If outside the spotlight cone, no light at all.
    if (dot(-light2, gl_LightSource[2].spotDirection) < gl_LightSource[2].spotCosCutoff) {
        light2_attenuation = 0;
    }

    // Compute the reflection vector
    vec3 reflection2 = 2*normal*dot(light2, normal) - light2;

    // Finaly, add the contribution from this light source
    vertex_color += light2_attenuation*gl_FrontMaterial.diffuse*gl_LightSource[2].diffuse*clamp(dot(normal, light2), 0, 1);
    vertex_color += light2_attenuation*gl_FrontMaterial.specular*gl_LightSource[2].specular*pow(clamp(dot(viewer, reflection2), 0, 1), gl_FrontMaterial.shininess);


    // ------ Exercise: add the contributions of the directional and point light source --------- //
    // Your code here


    // If you get completely lost, here's a page with some useful hints
    // http://en.wikibooks.org/wiki/GLSL_Programming/Blender/Diffuse_Reflection
}

