shader_type spatial;

render_mode unshaded;

uniform sampler2D screen_texture;

uniform sampler2D noise;

uniform sampler2D noise2;

uniform float speed = 0.01;

uniform float cameraSize = 100;

void fragment() {
	float displacement = texture(noise, (UV - 0.5) * (cameraSize/100.0) + vec2(1.0, 2.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(100.0, 260.0)).r;
	
	float displacement2 = texture(noise, (UV - 0.5) * (cameraSize/100.0) + vec2(-1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(100.0, 260.0)).r;
	
	vec4 c = texture(screen_texture, UV + vec2(mix(displacement - 0.5, displacement2 - 0.5, 0.5)/(cameraSize), 0)).rgba;
	
	ALBEDO = c.rgb;
	ALPHA = c.a;
}
