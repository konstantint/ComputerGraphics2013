// MTAT.03.015 Computer Graphics
// https://courses.cs.ut.ee/2013/cg/
//
// Raytracing exercise session code base.
// Exercise 1.


// --------------- Utility functions ------------ //
/** 
 * This is the "positive infinity" constant, which is rather
 * helpful in raytracing.
 * Yes, floats have a special value for both positive and negative
 * infinities.
 *
 * Note that there might be a warning about this during
 * shader compilation, but this is OK (it does not hurt the
 * functionality).
 */
const float INFINITY = 1.0/0.0;


/**
 * Encode vec3 rgb into vec4 srgb.
 */
vec4 srgb(vec3 col) {
	return vec4(pow(col, vec3(1.0/2.2)), 1.0);
}


/**
 * Returns the ray direction for given pixel
 * d is the distance to the raycasting plane
 */
vec3 ray_direction(vec2 pixelCoord, float d) {
	// Map pixelCoord from [ 0..n] x [ 0..m]
	//                to   [-a..a] x [-1..1]
	vec2 uv = 2.0*(pixelCoord - iResolution.xy/2.0) / iResolution.yy;
	
	// Return normalized ray direction, assuming
	// the raycasting plane is at distance d
	return normalize(vec3(uv, -d));
}


// --------------- Geometry raycasting functions ------------ //
/**
 * Raycast a plane (nx + b = 0).
 * Plane is described using a vec4(nx, ny, nz, b).
 * Returns a distance value to the nearest hit (or INFINITY)
 */
float raycast_plane(vec3 ro, vec3 rd, vec4 plane) {
	float d = -(dot(plane.xyz, ro) + plane.w)/dot(plane.xyz, rd);
	if (d < 0.0) d = INFINITY;
	return d;
}

/**
 * Raycast a sphere. 
 * Sphere is described using a vec4(cx, cy, cz, R).
 * Returns distance to hit.
 */
float raycast_sphere(vec3 ro, vec3 rd, vec4 sphere) {
	vec3 sd = sphere.xyz - ro;
	float p_sd = dot(sd, rd);
	float dp = sphere.w*sphere.w - (dot(sd, sd) - p_sd*p_sd); // 2 x Pythagoras' theorem.
	if (dp < 0.0) { return INFINITY; }
	else if (p_sd - sqrt(dp) > 0.0) return p_sd - sqrt(dp);
	else if (p_sd + sqrt(dp) > 0.0) return p_sd + sqrt(dp); // Hit from inside the sphere
	else return INFINITY;
}


// --------------- Scene objects ------------ //
// Scene will consist of a plane and a sphere

// (nx, ny, nz, b)
vec4 plane = vec4(0.0, 1.0, 0.0, 1.0);

// (cx, cy, cz, r)
vec4 sphere = vec4(0.0, -0.5, -4.0, 1.0);

// Colors of different objects
vec3 sky_color = 1.5*vec3(0.25, 0.40, 0.45);
vec3 sphere_color = vec3(1.0, 0.0, 0.0);

// Plane color depends on the point at which we hit it.
vec3 plane_color(vec3 point) {
	// Checkerboard
	float col = float(mod(floor(point.x) + floor(point.z), 2.0));
	return 0.8*vec3(col);
}


// --------------- Scene raycasting function ------------ //
/**
 * Basic raycasting routine. Given a ray origin and direction
 * finds the closest object and returns its color.
 */
vec3 raycast_scene(vec3 ro, vec3 rd) {
	// Raycast both sphere and plane, see which one is closer
	// determine color of the point and return the result.
	
	// EXERCISE: Implement this function
	// Use raycast_sphere, raycast_plane
	// as well as plane, sphere parameters specified above.
	return sky_color;
}


// --------------- Shader main routine ------------ //
void main(void) {
	vec3 ro = vec3(0, 0, 0);    				   // Ray origin (camera position)
	
	// EXERCISE: 
	// Set up the distance to raytracing plane to achieve
	// fovY = 60 degrees.
	vec3 rd = ray_direction(gl_FragCoord.xy, 2.0); // Ray direction

	// EXERCISE: Implement 3x3 antialiasing.
	// I.e. rather than sampling the scene at a single direction
	// corresponding to pixel (x, y), sample a grid with
	// (x + [-0.2, 0.0, 0.2], y + [-0.2, 0.0, 0.2])
	vec3 color = raycast_scene(ro, rd);

	gl_FragColor = srgb(color);
}
