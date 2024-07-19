shader_type spatial;
uniform bool inverted;
uniform bool translucent = false;
uniform sampler2D icon;

void fragment() {
	METALLIC = 1.0;
	ROUGHNESS = 1.0;
	vec4 tex = texture(icon, UV);
	ALBEDO.rgb = tex.rgb;
	ALPHA = tex.a;
	
	if(inverted) {
		EMISSION.rgb = vec3(1) - ALBEDO.rgb;
	}
	if(translucent) {
		ALBEDO.rgb *= 0.6;
		ALPHA = min(ALPHA, 0.8);
	}
}
