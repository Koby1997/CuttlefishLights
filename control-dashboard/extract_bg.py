from PIL import Image
import sys

def process_image(input_path, output_path):
    print(f"Processing {input_path}...")
    img = Image.open(input_path).convert("RGBA")
    datas = img.getdata()
    
    newData = []
    for item in datas:
        r, g, b, a = item
        # Calculate luminescence from max RGB value
        brightness = max(r, g, b)
        
        # Exponential contrast curve for Alpha mapping
        # By taking (brightness/255)^3, we aggressively crush faint glows (like floor reflections)
        # down to virtually nothing, while keeping the intense neon lines perfectly smooth and bright.
        float_alpha = (brightness / 255.0) ** 3
        alpha = int(float_alpha * 255.0)

        # Absolute cutoff for digital dust
        if alpha < 8:
            alpha = 0
            
        # Output as pure white! Alpha transparency naturally handles the glowing halo effect.
        newData.append((255, 255, 255, alpha))
        
    img.putdata(newData)
    img.save(output_path, "PNG")
    print(f"Saved transparent image to {output_path}")

if __name__ == "__main__":
    process_image("public/logo.png", "public/logo_transparent.png")
