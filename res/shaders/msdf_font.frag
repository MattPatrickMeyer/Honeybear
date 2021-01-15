#version 330 core
in vec2 TexCoords;
in vec4 Colour;
in float PixelRange;

out vec4 FragColor;

uniform sampler2D image;

float median(float r, float g, float b) {
    return max(min(r, g), min(max(r, g), b));
}

float anothermedian(vec3 v)
{
    return median(v.x, v.y, v.z);
}

float contour(float d, float w){
    return smoothstep(0.5 - w, 0.5 + w, d);
}
float samp(vec2 uv, float w){
    return contour(texture(image, uv).a, w);
}

void main()
{    
    // vec2 pos = TexCoords;
    // vec3 sample = texture(image, TexCoords).rgb;
    // ivec2 sz = textureSize(image, 0).xy;
    // float dx = dFdx(pos.x) * sz.x;
    // float dy = dFdy(pos.y) * sz.y;
    // float to_pixels = 20.0 * inversesqrt(dx * dx + dy * dy);
    // float sig_dist = median(sample.r, sample.g, sample.b) - 0.5;
    // float w = fwidth(sig_dist);
    //float o = clamp(sig_dist * to_pixels + 0.5, 0.0, 1.0);

    // ------------------------- DON"T TOUCH
    //float px_range = 10.0;
    float px_range = PixelRange;
    vec2 msdf_unit = px_range / vec2(textureSize(image, 0));
    vec3 sample = texture(image, TexCoords).rgb;
    float dist = median(sample.r, sample.g, sample.b) - 0.5;
    dist *= dot(msdf_unit, 0.5 / fwidth(TexCoords));
    float alpha = clamp(dist + 0.5, 0.0, 1.0);
    // -------------------------

    // ------------------------
    // vec3 sample = texture(image, TexCoords).rgb;
    // float dist = median(sample.r, sample.g, sample.b);
    // float width = fwidth(dist);
    // float alpha = contour(dist, width);

    // float dscale = 0.354;
    // vec2 duv = dscale * (dFdx(TexCoords) + dFdy(TexCoords));
    // vec4 box = vec4(TexCoords - duv, TexCoords + duv);
    // float asum = samp(box.xy, width)
    //            + samp(box.zw, width)
    //            + samp(box.xw, width)
    //            + samp(box.zy, width);
    // alpha = (alpha * 0.5 * asum) / 3.0;
    // ------------------------

    // float d_scale = 0.354;
    // float friends = 0.5;
    // vec2 duv = d_scale * (dFdx(TexCoords) + dFdy(TexCoords));
    // vec4 box = vec4(TexCoords - duv, TexCoords + duv);
    // vec4 c = texture(image, box.xy) + texture(image, box.zw) + texture(image, box.xw) + texture(image, box.zy);
    // float sum = 4.0;
    // vec4 center = vec4(texture(image, TexCoords).rgb, 1.0);
    // vec4 average_sample = Colour * (center + friends * c) / (1.0 + sum * friends);

    FragColor = vec4(Colour.rgb, alpha * Colour.a);
}

