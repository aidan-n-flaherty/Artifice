shader_type spatial;

render_mode unshaded;
//render_mode cull_disabled;

uniform sampler2D screen_texture;

uniform float speed = 0.01;

uniform int id = 0;

varying float gradient;

void vertex() {
	gradient = max(1.0 - 0.5 * abs(VERTEX.y), min(1.0, 5.0 * (VERTEX.z - 1.75)));
}

void fragment() {
	vec4 c = texture(screen_texture, UV + vec2(0.83, 0.17) * float(id) + speed * vec2(1.0, -1.0) * TIME).rgba;
	
	vec3 grad = vec3(gradient);
	
	ALPHA = pow(grad.r + c.r * 0.5, 2.0);

	//if(grad.r > 0.5) ALPHA += grad.r - 0.5;
	//if(grad.r < 0.5) ALPHA -= (0.5 - grad.r) * c.r * c.r;
	
	ALBEDO = 0.1 + 0.3 * vec3(c.r * c.r, c.g * c.g, c.b * c.b) + 0.5 * max(grad.r - 0.3, 0.0);
	
	ALPHA *= pow(ALBEDO.r, 1.0);
	
	if(grad.r > 0.75) ALBEDO = vec3(max(ALBEDO.r, (grad.r - 0.75) * 5.0));
}
