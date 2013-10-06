#version 120
// Note: The version specification above is compulsory (although things might work without it on some cards)
// GLSL version 1.2 is fairly old by now, and I would rather prefer teaching with version 1.3+.
// However it turns out some laptops as well as VirtualBox installations would only suport version 1.2.

varying vec4 vertex_color;   // The newer equivalent declaration is "smooth out vec4 vertex_color".

uniform mat4 transformation_matrix; // We shall connect this to a custom transformation matrix in our display() function.

void main(void) {
    // Use the default (fixed-function) transform.
    gl_Position = ftransform();

    //// ----- Try the following variations and see what they do -----

    //// Vertices are not transformed at all
    //gl_Position = gl_Vertex;

    //// Vertices are manually multiplied by the ModelViewProjection
    //gl_Position = gl_ModelViewProjectionMatrix * gl_Vertex;

    //// Same as above, but less efficient yet more explicit
    //gl_Position = gl_ProjectionMatrix * gl_ModelViewMatrix * gl_Vertex;

    //// All vertices mapped to (0, 0, 0). (Naturally, you won't see any triangle)
    //gl_Position = vec4(0, 0, 0, 1);

    //// Vertices transformed with a matrix sent from the main program
    //gl_Position = transformation_matrix*gl_Vertex;

    //// You can do weird stuff, if you want.
    //// (Don't think too much over what the code below does
    ////   I just put together some arbitrary operations that produced a funny result)
    //vec4 v = gl_ModelViewMatrix * gl_Vertex;
    //v = vec4(normalize(v.xy)*(0.5+0.4*sin(2*atan(v.y, v.x))), 0, 1);
    //gl_Position = v;

    // Leave this as is
    vertex_color = gl_Color;
}

