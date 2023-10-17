shader_type spatial;

render_mode unshaded;

uniform sampler2D screen_texture;

uniform sampler2D noise;

uniform sampler2D noise2;

uniform sampler2D noise3;

uniform sampler2D gradient;

uniform float speed = 0.005;

uniform float cameraSize = 100;

void fragment() {
	float angle = 2.0 * PI * texture(noise3, (UV - 0.5) * (cameraSize/100.0) + vec2(-1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	float mag = 0.3 * texture(noise2, (UV - 0.5) * (cameraSize/100.0) + vec2(1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	
	float dX = mag * cos(angle);
	float dY = mag * sin(angle);
	
	float displacement = texture(noise, (UV - 0.5 + 0.05 * vec2(dX, dY)) * (cameraSize/100.0) + vec2(1.0, 2.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;

	//float displacement2 = texture(noise2, (UV - 0.5 + 0.01 * vec2(dX, dY)) * (cameraSize/100.0) + vec2(1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	
	vec4 c = texture(screen_texture, UV + vec2(displacement/cameraSize, 0)).rgba;
	
	vec3 grad = texture(gradient, UV).rgb;
	
	c.rgb *= ALPHA;
	ALBEDO = vec3(c.r * grad.r, c.g * grad.g, c.b * grad.b) * 0.5 + vec3(0.25 * mix(displacement - 0.25, displacement - 0.25, 0.5)) * 0.5;
}
