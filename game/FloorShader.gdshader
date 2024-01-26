shader_type spatial;

uniform sampler2D screen_texture;

void fragment() {
	vec4 color = texture(screen_texture, UV);
	ALBEDO = color.rgb;
	ALPHA = color.a;
	
	if(ALPHA == 1.0) {
		ALPHA = max(max(1.0 - color.r, 1.0 - color.g), 1.0 - color.b);
		ALBEDO = vec3(1.0);
	} else {
		ALBEDO = vec3(0.0);
	}
}
