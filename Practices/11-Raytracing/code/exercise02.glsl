// MTAT.03.015 Computer Graphics
// https://courses.cs.ut.ee/2013/cg/
//
// Raytracing exercise session code base.
// Exercise 2.
// (continues Exercise 1: 
//    https://www.shadertoy.com/view/ldSGzW
//  See that shader for comments to some utility functions)


// --------------- Utility functions ------------ //
const float INFINITY = 1.0/0.0;

vec4 srgb(vec3 col) {
	return vec4(pow(col, vec3(1.0/2.2)), 1.0);
}

vec3 ray_direction(vec2 pixelCoord, float d) {
	vec2 uv = 2.0*(pixelCoord - iResolution.xy/2.0) / iResolution.yy;
	return normalize(vec3(uv, -d));
}

// --------------- Geometry raycasting functions ------------ //
float raycast_plane(vec3 ro, vec3 rd, vec4 plane) {
	float d = -(dot(plane.xyz, ro) + plane.w)/dot(plane.xyz, rd);
	if (d < 0.0) d = INFINITY;
	return d;
}

float raycast_sphere(vec3 ro, vec3 rd, vec4 sphere) {
	vec3 sd = sphere.xyz - ro;
	float p_sd = dot(sd, rd);
	float dp = sphere.w*sphere.w - (dot(sd, sd) - p_sd*p_sd); // 2 x Pythagoras' theorem.
	if (dp < 0.0) { return INFINITY; }
	else if (p_sd - sqrt(dp) > 0.0) return p_sd - sqrt(dp);
	else if (p_sd + sqrt(dp) > 0.0) return p_sd + sqrt(dp); // Hit from inside the sphere
	else return INFINITY;
}


// --------------- Lighting computations ------------ //
struct Material {
	vec3 diffuse;
	vec3 specular;
	float shininess;
	float reflectivity;
};

struct Light {
	vec3 pos;
	vec3 intensity;
	float constantAttenuation;
	float quadraticAttenuation;
};

// Overall ambient intensity
vec3 ambient = vec3(0.1, 0.1, 0.1);

// Single light source in the scene
Light light = Light(
	vec3(-2.0, 2.0, 0.0),
	vec3(10.0, 10.0, 10.0),
	1.0, 0.1);

/**
 * Blinn-Phong lighting computation for the single light source.
 * (NB: does not take ambient into account)
 */
vec3 lighting(vec3 vertex_pos, vec3 normal, 
			  vec3 viewer_pos, Material material) {
	vec3 l = light.pos - vertex_pos;
	float d = length(l);
	l = normalize(l);
	vec3 v = normalize(viewer_pos - vertex_pos);
	vec3 h = normalize((l + v)/2.0);
	float c_diffuse = max(0.0, dot(normal, l));
	float c_specular = pow(max(0.0, dot(normal, h)), material.shininess);
	float attenuation = 1.0/(light.constantAttenuation + pow(d, light.quadraticAttenuation));	
	return attenuation*(c_diffuse*material.diffuse + c_specular*material.specular);
}

// --------------- Scene objects ------------ //
// Scene will consist of a plane and a sphere

// (nx, ny, nz, b)
vec4 plane = vec4(0.0, 1.0, 0.0, 1.0);

// This time the sphere is jumping
float jump_t = 2.0*mod(iGlobalTime, 0.5);
// (cx, cy, cz, r)
vec4 sphere = vec4(0.0, 2.0*jump_t*(1.0-jump_t), -4.0, 1.0);

// We use lighting, so instead of plain colors 
// we actually use materials
vec3 sky_color = 1.5*vec3(0.25, 0.40, 0.45);
vec3 red = vec3(1.0, 0.0, 0.0);
vec3 white = vec3(1.0);

Material sky_mat = Material(sky_color, white, 0.0, 0.0);
Material sphere_mat = Material(0.7*red, white, 30.0, 0.3);

// As before, plane's material depends on the point
Material plane_mat(vec3 point) {
	// Plane will be colored in a checkerboard fashion.
	float col = float(mod(floor(point.x) + floor(point.z), 2.0));
	return Material(0.8*vec3(col), white, 30.0, 0.2);
}


// --------------- Scene raycasting function ------------ //
/**
 * Now that we use lighting and want to model light reflections
 * and shadows, just returning the color of the point is not enough.
 * raycast_scene will now return this structure,
 */
struct RaycastResult {
	float d; // Distance to hitpoint, INFINITY if no hit.
	vec3 pt; // Point hit (if any)
	vec3 normal; 		// Object normal at hitpoint
	Material material;  // Material at hitpoint
};


/**
 * Casts a ray into the scene and returns hit result as RaycastResult.
 */
RaycastResult raycast_scene(vec3 ro, vec3 rd) {
	// Trick which avoids self-shadowing (shadow acne)
	// and similar artefacts (self-reflections, etc).
	// Try removing this line and see what happens.
	ro = ro + 0.0001*rd;
	
	// Raycast both sphere and plane, see which one is closer
	// determine color of the point and return the result.
	float ds = raycast_sphere(ro, rd, sphere);
	float dp = raycast_plane(ro, rd, plane);
	
	if (ds == INFINITY && dp == INFINITY) {
		return RaycastResult(INFINITY, vec3(0), vec3(0), sky_mat);
	}
	if (dp < ds) {
		vec3 pt = ro + dp*rd;
		vec3 normal = plane.xyz;
		return RaycastResult(dp, pt, normal, plane_mat(pt));
	}
	else {
		vec3 pt = ro + ds*rd;
		vec3 normal = normalize(pt - sphere.xyz);
		return RaycastResult(ds, pt, normal, sphere_mat);
	}
}


// --------------- Scene rendering ------------ //
/**
 * This is the actual rendering routine.
 * For given ray it returns the color, as seen through this ray.
 * It uses raycasting in multiple ways,
 */
vec3 render_scene(vec3 ro, vec3 rd) {
	// EXERCISE: Implement this function
	// To support shadows and reflections.
	
	// Hint: First add shadows
	// Then add a loop over the computation
	// that aggregates color from ray reflections,
	// keeping track of a weight contribution.
	vec3 color = vec3(0.0);
	
	RaycastResult r = raycast_scene(ro, rd);
	if (r.d == INFINITY) {
		color = sky_color;
	}
	else {
		color = ambient*r.material.diffuse;
		color += lighting(r.pt, r.normal, ro, r.material);
	}
	return color;
}

// --------------- Shader main routine ------------ //
void main(void) {
	vec3 ro = vec3(0, 0, 0);
	vec3 rd = ray_direction(gl_FragCoord.xy, 2.0);
	vec3 color = render_scene(ro, rd);
	gl_FragColor = srgb(color);
}
