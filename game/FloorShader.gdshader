shader_type spatial;

uniform sampler2D screen_texture;

void fragment() {
	vec4 color = texture(screen_texture, UV);
	ALBEDO = color.rgb;
	ALPHA = color.a;
	METALLIC = 0.5;
	
	if(ALPHA == 1.0) {
		ALPHA = min(min(1.0 - color.r, 1.0 - color.g), 1.0 - color.b);
		float highest =  max(max(color.r, color.g), color.b);
		
		if(highest > 0.0) ALBEDO = 1.0/highest * color.rgb;
		else ALBEDO = vec3(1.0);
	} else {
		ALBEDO = vec3(0.0);
	}
}
