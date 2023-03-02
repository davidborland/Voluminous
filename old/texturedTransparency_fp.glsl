varying vec3 objectPosition, normal, lightDir, eyeVec;

void main() {					 
	// Compute texture
	float scale = 0.1;
	float texture = sin(objectPosition.x * objectPosition.z * scale) + 
					sin(objectPosition.y * objectPosition.x * scale) + 
					sin(objectPosition.z * objectPosition.y * scale);
					
	if (texture <= 0.0) discard;
	
	// Compute vectors needed for lighting
	vec3 N = normalize(normal);		
	vec3 L = normalize(lightDir);
	
	vec3 E = normalize(eyeVec);
	vec3 R = reflect(-L, N);
		
	// Perform lighting
	float lambertTerm = abs(dot(N, L));
	float specularTerm = pow(max(dot(R, E), 0.0), gl_FrontMaterial.shininess);
	
	vec4 color = gl_FrontLightModelProduct.sceneColor;
	color += gl_FrontLightProduct[0].ambient;
	color += gl_FrontLightProduct[0].diffuse * lambertTerm;
	color += gl_FrontLightProduct[0].specular * specularTerm;
	
	gl_FragColor = color;
}