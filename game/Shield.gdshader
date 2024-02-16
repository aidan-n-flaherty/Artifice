shader_type spatial;

render_mode unshaded;
render_mode cull_disabled;

uniform sampler2D screen_texture;

uniform sampler2D noise;

uniform sampler2D noise2;

uniform sampler2D noise3;

uniform sampler2D gradient;

uniform float speed = 0.005;

void fragment() {
	float angle = 2.0 * PI * texture(noise3, (UV - 0.5) + vec2(-1.0, -1.0) * speed * TIME).r;
	float mag = 0.3 * texture(noise2, (UV - 0.5) + vec2(1.0, -1.0) * speed * TIME).r;
	
	float dX = mag * cos(angle);
	float dY = mag * sin(angle);
	
	float displacement = texture(noise, (UV - 0.5 + 0.05 * vec2(dX, dY)) + vec2(1.0, 2.0) * speed * TIME).r;

	vec4 c = texture(screen_texture, UV + vec2(displacement/10.0, 0)).rgba;
	
	vec3 grad = texture(gradient, UV).rgb;
	
	ALPHA = pow(grad.r, 2);
	//if(grad.r > 0.5) ALPHA += grad.r - 0.5;
	//if(grad.r < 0.5) ALPHA -= (0.5 - grad.r) * c.r * c.r;
	
	ALBEDO = 0.5 + vec3(c.r, c.g, c.b) * 0.5 + 0.5 * min(grad.r, 0.0) + vec3(0.25 * mix(displacement - 0.5, displacement - 0.5, 0.5)) * 0.25;
	ALPHA *= 0.5 * ALBEDO.r;
	
	if(grad.r > 0.75) ALBEDO = vec3(max(ALBEDO.r, (grad.r - 0.75) * 5.0));
}
