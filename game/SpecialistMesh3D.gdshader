shader_type spatial;
render_mode unshaded;

uniform bool inverted;
uniform bool translucent = false;

void fragment() {
	METALLIC = 1.0;
	ROUGHNESS = 1.0;
	
	if(inverted) ALBEDO = vec3(1) - COLOR.rgb;
	if(translucent) ALPHA = 0.6;
}