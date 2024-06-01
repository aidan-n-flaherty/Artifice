shader_type spatial;
render_mode unshaded;

uniform sampler2D screen_texture;

void fragment() {
	vec4 color = texture(screen_texture, UV);
	ALBEDO = vec3(color.r, color.g, min(color.r, color.g));
	ALPHA = color.b;
	METALLIC = 1.0;
}
