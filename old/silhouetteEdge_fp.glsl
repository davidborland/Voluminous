varying vec3 normal, eyeVec;

void main() {									
	// Compute vectors needed for lighting
	vec3 N = normalize(normal);		
	vec3 E = normalize(eyeVec);
    
    // Threshold based on the dot product of the eye and the surface normal 
	float threshold = 0.2;
	if (abs(dot(N, E)) <= threshold) {	
		// Set a reasonable value for the color
		vec4 color = gl_FrontLightModelProduct.sceneColor;
		color.rgb = normalize(color.rgb);
		color.rgb *= 0.5;
		
		gl_FragColor = color;			
	}
	else {
		discard;
	}		
}