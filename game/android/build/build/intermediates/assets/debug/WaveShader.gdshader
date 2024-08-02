shader_type canvas_item;

uniform sampler2D noise;

uniform float screenWidth = 1080;

uniform float screenHeight = 1920;

void vertex() {
	// Called for every vertex the material is visible on.
}

void fragment() {
	vec2 pos = UV * vec2(screenWidth/max(screenWidth, screenHeight), screenHeight/max(screenWidth, screenHeight));
	
	float a = float(int(texture(noise, pos).r * 5.0))/5.0;
	
	COLOR.rgb -= a;
}
