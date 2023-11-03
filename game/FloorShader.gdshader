shader_type spatial;

uniform sampler2D screen_texture;

void fragment() {
	vec4 color = texture(screen_texture, UV);
	ALBEDO = color.rgb;
	ALPHA = color.a;
	
	if(ALPHA == 1.0) ALPHA = max(max(color.r, color.g), color.b);
}
