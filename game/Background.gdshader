shader_type canvas_item;

render_mode unshaded;

uniform sampler2D noise;

uniform sampler2D noise2;

uniform sampler2D noise3;

uniform float gradStrength = 1.0;

uniform float speed = 0.005;

uniform float screenWidth = 1080;

uniform float screenHeight = 1920;

vec2 norm(vec2 pos) {
	while(pos.x >= 1.0) pos.x -= 1.0;
	while(pos.x < 0.0) pos.x += 1.0;
	while(pos.y >= 1.0) pos.y -= 1.0;
	while(pos.y < 0.0) pos.y += 1.0;
	
	return pos;
}

void fragment() {
	vec2 uv = UV * vec2(screenWidth/max(screenWidth, screenHeight), screenHeight/max(screenWidth, screenHeight));
	
	vec2 posA = norm((uv - 0.5) + vec2(-1.0, -1.0) * speed * TIME);
	
	vec2 posB = norm((uv - 0.5) + vec2(1.0, -1.0) * speed * TIME);
	
	float angle = 2.0 * PI * texture(noise3, posA).r;
	float mag = 0.3 * texture(noise2, posB).r;
	
	float dX = mag * cos(angle);
	float dY = mag * sin(angle);
	
	vec2 posC = norm((uv - 0.5) + vec2(0.0, 5.0) * speed * TIME + 0.1 * vec2(dX, dY));
	
	float displacement = texture(noise, posC).r;

	//float displacement2 = texture(noise2, (UV - 0.5 + 0.01 * vec2(dX, dY)) * (cameraSize/100.0) + vec2(1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	
	float grad = (1.0 - gradStrength) + gradStrength * (1.0 - UV.y);
	
	COLOR.rgb = vec3(grad * mix(displacement - 0.25, displacement - 0.25, 0.5));
	COLOR.rgb = vec3(COLOR.r, 1.2 * COLOR.g, 1.5 * COLOR.b);
}
