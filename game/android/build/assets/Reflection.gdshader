shader_type spatial;

uniform sampler2D depth : hint_depth_texture;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	vec4 c = texture(depth, UV);
	
	ALBEDO = vec3(2.0 * c.r);
}

//void light() {
	// Called for every pixel for every light affecting the material.
	// Uncomment to replace the default light processing function with this one.
//}
