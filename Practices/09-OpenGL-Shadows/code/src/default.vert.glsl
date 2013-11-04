#version 120
varying vec3 normal;
varying vec4 vertexPos;
varying vec4 color;

void main(void) {
    gl_Position = ftransform();
    normal = gl_NormalMatrix * gl_Normal;
    vertexPos = gl_ModelViewMatrix * gl_Vertex;
    color = gl_Color;

    // .. and here goes a hack which is here for the only reason of hiding some
    // details of OpenGL implementation from the students of the course.
    if (gl_Position.w < 0) gl_Position *= -1;
    if (vertexPos.w < 0) vertexPos *= -1;

    // Now on to some explanation for why it is done.
    // Recall that we are working with homogeneous coordinates. Thus, a uniform rescaling of a vec4
    // does not in principle affect the point it corresponds to:
    // (x, y, z, 1) is the same as (wx, wy, wz, w) is the same as (10wx, 10wy, 10wz, 10w)
    // is the same as (-wx, -wy, -wz, -w) in the sense that all of them denote the point (x, y, z).
    //
    // In theory, (at least that's an easy way to think of it), right after all transformations
    // have been applied, perspective division is applied to the points and the geometry is
    // clipped against the [-1, 1]x[-1, 1]x[-1, 1] cube by testing
    // "-1 <= x, y, z <= 1"
    //
    // In practice, however, it makes sense to delay perspective division for as long as possible,
    // certainly beyond the clipping stage, because we still need full homogeneous coordinates to
    // perform perspective-correct interpolation.
    // Consequently, the clipping test is performed while still in homogeneous space.
    // And although it could be performed as "-1 <= x/w, y/w, z/w <= 1", what is in fact happening
    // in OpenGL is "-w <= x, y, z <= w".
    // This avoids a division step at the expense of the test being incorrect when w is negative.
    //
    // To summarize, whenever your points end up with a negative w they will be killed away by the
    // clipping stage. In most practical situations it is rare for w to become negative:
    // the only transformation that affects w is in fact gluPerspective and this keeps w positive.
    // In the projective shadows example, however, we are using a "shadow projection matrix".
    // Depending on the direction of the normal to the projection plane the resulting matrix may
    // actually produce negative w values. As a result, if we use the usual vertex shader the matrix
    // only "works" when the light source is on the positive side of the projection plane.
    // This is unintuitive and irrelevant from the standpoint of general computer graphics theory,
    // (it's a peculiarity of a particular graphics API). Hence we quietly avoid the problem by
    // making sure the vertex shader always outputs points with positive w.
}
