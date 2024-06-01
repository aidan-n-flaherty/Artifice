shader_type canvas_item;

render_mode unshaded;

uniform sampler2D noise;

uniform sampler2D noise1;

uniform sampler2D noise2;

uniform sampler2D noise3;

uniform sampler2D noise4;

uniform sampler2D noise5;

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
	
	uv.y += (1.0 - screenHeight/max(screenWidth, screenHeight));
	
	vec2 posA = norm((uv - 0.5) + vec2(-1.0, -1.0) * speed * TIME);
	
	vec2 posB = norm((uv - 0.5) + vec2(1.0, -1.0) * speed * TIME);
	
	float angle = 2.0 * PI * texture(noise3, posA).r;
	float mag = 0.1 * texture(noise2, posB).r;
	
	float dX = mag * cos(angle);
	float dY = mag * sin(angle);
	
	vec2 size = vec2(textureSize(noise5, 0));
	
	//COLOR.rgb = texture(noise6, norm(vec2(UV.x, UV.y) * vec2(1.0, (screenHeight/screenWidth) / (size.y/size.x)) + 0.05 * vec2(dX, dY))).rgb;
	//COLOR.rgb *= 10.0 * COLOR.rgb;
	
	vec2 scale = vec2(1.0, (screenHeight/screenWidth) / (size.y/size.x));
	if(scale.y > 1.0) {
		scale.x *= 1.0/scale.y;
		scale.y = 1.0;
	}
	COLOR.rgb = texture(noise5, norm(vec2(UV.x, UV.y) * scale + vec2(0.0, max(0.0, 1.0 - scale.y)) + 0.05 * vec2(dX, dY))).rgb;
	COLOR.rgb *= 5.0 * COLOR.rgb;
	if(COLOR.r > 0.5) {
		COLOR.rgb *= (1.0 + 0.25 * (COLOR.r - 0.5));
	}
	//vec2 posC = norm((uv - 0.5) + vec2(0.0, 5.0) * speed * TIME + 0.1 * vec2(dX, dY));
	
	//float displacement = texture(noise, posC).r * 0.5 + texture(noise5, vec2(-posC.x, -posC.y)).r;
	
	//float displacement2 = texture(noise2, (UV - 0.5 + 0.01 * vec2(dX, dY)) * (cameraSize/100.0) + vec2(1.0, -1.0) * speed * TIME + NODE_POSITION_WORLD.xz/vec2(-100.0, 260.0)).r;
	
	float grad = (1.0 - gradStrength * 0.9) + gradStrength * 0.9 * (1.0 - UV.y);
	
	grad *= grad + (1.0 - grad) * texture(noise4, uv).r;
	
	COLOR.rgb = vec3(0.04, 0.06, 0.1) * 1.5 + COLOR.rgb * 0.1;
	
	//COLOR.rgb += 1.0 * texture(noise6, vec2(uv.x, 1.0 - uv.y)).rgb * texture(noise6, vec2(uv.x, 1.0 - uv.y)).rgb;
	
	//COLOR.rgb -= 0.1 * texture(noise, norm(uv + vec2(-2.0, 2.0) * speed * TIME)).rgb * texture(noise1, norm(uv + vec2(2.0, 2.0) * speed * TIME)).rgb;
	
	//COLOR.rgb = vec3(COLOR.r, 1.2 * COLOR.g, 1.5 * COLOR.b);
}
