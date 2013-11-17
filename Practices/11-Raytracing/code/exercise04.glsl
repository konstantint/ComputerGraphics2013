// MTAT.03.015 Computer Graphics
// https://courses.cs.ut.ee/2013/cg/
//
// Raytracing exercise session code base.
// Exercise 4.

float time = iGlobalTime*0.1;

// --------------- Utility functions ------------ //
const float INFINITY = 1.0/0.0;

vec4 srgb(vec3 col) {
	return vec4(pow(col, vec3(1.0/2.2)), 1.0);
}

vec3 ray_direction(vec2 pixelCoord, float d) {
	vec2 uv = 2.0*(pixelCoord - iResolution.xy/2.0) / iResolution.yy;
	return normalize(vec3(uv, -d));
}

// --------------- Distance field primitives and operations ------------ //
vec3 opTranslate(vec3 x, vec3 by) {
	return x - by;
}
vec3 opRotateX(vec3 x, float by) {
	// GLSL 1.3- does not have matrix inversion,
	// So we won't bother with general-purpose transforms.
	by = -by; // Apply inverse rotation
	mat3 m = mat3(1, 0, 0, 0, cos(by), sin(by), 0, -sin(by), cos(by));
	return m*x;
}
vec3 opRepeat(vec3 x, vec3 by) {
	return mod(x + 0.5*by, by) - 0.5*by;
}

float opUnion(float a, float b) {
	return min(a, b);
}
float opSubtract(float a, float b) {
    return max(a, -b);
}
float opIntersect(float a, float b) {
	return max(a, b);
}

float sphere(vec3 x, float r) {
	return length(x) - r;
}
float plane(vec3 x, vec4 n) { // N = (nx, ny, nz, b)
	return dot(vec4(x, 1.0), n);
}
float torus(vec3 x, float outer, float inner) {
  vec2 q = vec2(length(x.xz) - outer, x.y);
  return length(q) - inner;
}
float roundBox(vec3 x, vec3 dimensions, float r) {
  return length(max(abs(x) - dimensions/2.0, 0.0)) - r;
}
float cylinder(vec3 x, float radius) {
  return length(x.xy) - radius;
}

// --------------- Whole scene description ------------ //
/**
 * This is the scene description as a single distance field.
 */
float scene(vec3 x) {
	// EXERCISE: Update the scene.
	//   * Drill a hole in the box (using a cylinder with radius 0.5)
	//   * Have the box rotate around its X axis <time> radians per second.
	//     (time is a global variable, defined on line 7)
	//   * Repeat the box infinitely with step 10 along all dimensions.
	//   * (Bonus) Color the drilled inside red (requires changing output type from float d to vec4(color, d)).
	x = opTranslate(x, vec3(0, 0, -3));
	return roundBox(x, vec3(1, 1, 1), 0.2);
}

/**
 * Computes normal to the scene
 * using discrete differentiation of the distance field.
 */
vec3 scene_normal(vec3 x) {
	vec2 e = vec2(0.001, 0);
	float s = scene(x);
	return normalize(vec3(scene(x+e.xyy) - s,
						  scene(x+e.yxy) - s,
						  scene(x+e.yyx) - s));
}

/**
 * Computes the color at a point in the scene 
 * using basic diffuse lighting.
 */
vec3 scene_color(vec3 x) {
	if (x.x == INFINITY) return vec3(0.0);
	else if (length(x) > 100.0) return vec3(0.0); // Hide far away objects
	else {
		vec3 n = scene_normal(x);
		vec3 light_dir = normalize(vec3(0, 1, 1)); // Light direction in camera-space
		float diffuse = max(0.0, dot(light_dir, n));
		float ambient = 0.05;
		return (ambient + diffuse)*vec3(1.0, 1.0, 1.0);
	}
}

// --------------- Raymarching algorithm ------------ //
vec3 raymarch(vec3 ro, vec3 rd) {
	// EXERCISE: Implement the sphere tracing algorithm
	//  - Start at the ray origin rd.
	//	- Proceed step by step along direction rd
	//  - Step size from position (x) is given by the distance
	//    field value, i.e. scene(x)
	//  - Stop when step size is too small (< 0.01)
	//    or when step limit (e.g. 50) is exceeded.
	// If the step limit was exceeded, return vec3(INFINITY)
	// Otherwise return the located intersection point x.
	return vec3(INFINITY);
}

// --------------- Shader main routine ------------ //
void main(void) {
	vec3 ro = vec3(0, 0, 0);					   // Ray origin (camera position)
	vec3 rd = ray_direction(gl_FragCoord.xy, 2.0); // Ray direction
	vec3 hit = raymarch(ro, rd);				   // Perform raymarching
	vec3 color = scene_color(hit);			   	   // Compute lighting
	gl_FragColor = srgb(color);					   // Encode	
}
