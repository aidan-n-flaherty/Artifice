shader_type spatial;

render_mode unshaded;
//render_mode cull_disabled;

uniform sampler2D screen_texture;

uniform sampler2D gradient;

uniform float speed = 0.01;

uniform bool dark;

uniform int id = 0;

void fragment() {
	vec4 c = texture(screen_texture, UV + vec2(0.83, 0.17) * float(id) + 0.015 * vec2(1.0, -1.0) * TIME).rgba;
	
	vec3 grad = texture(gradient, UV).rgb;
	
	ALPHA = pow(grad.r + c.r * 0.5, 2.0);

	//if(grad.r > 0.5) ALPHA += grad.r - 0.5;
	//if(grad.r < 0.5) ALPHA -= (0.5 - grad.r) * c.r * c.r;
	
	ALBEDO = 0.1 + 0.3 * vec3(c.r * c.r, c.g * c.g, c.b * c.b) + 0.5 * max(grad.r - 0.3, 0.0);
	
	ALPHA *= pow(ALBEDO.r, 1.0);
	
	if(grad.r > 0.75) ALBEDO = vec3(max(ALBEDO.r, (grad.r - 0.75) * 10.0));
	
	float line = min(min(abs(UV.x - 0.5), abs(UV.y - 0.5)), min(abs(UV.x + UV.y - 1.0), abs(UV.x - UV.y)));
	float mag = sqrt((UV.x - 0.5) * (UV.x - 0.5) + (UV.y - 0.5) * (UV.y - 0.5));
	
	line = min(max(line, max(0.0, 0.15 - mag)), abs(mag - 0.15));
	
	if(line < 0.025) {
		ALPHA += 0.025 * (1.0 - 40.0 * line);
		ALBEDO += vec3((1.0 - 40.0 * line));
	}
	
	if(dark) {
		ALBEDO = vec3(0.0);
		ALPHA += 0.9;
	}
}
