# Assets

This folder contains embedded assets (images, fonts, etc.) for the firmware.

## Converting BMP Images

1. Place your BMP file in this directory (e.g., `logo.bmp`)
2. Convert it to a C array:
   ```bash
   python3 bmp_to_c.py logo.bmp logo.h
   ```
3. This generates `logo.h` and `logo.c` files
4. Add the generated `.c` file to CMakeLists.txt
5. Include the `.h` file in your code and use the array

## BMP Format Requirements

- **Color format**: RGB565 (16-bit) is recommended for best compatibility
- **Size**: Keep images reasonably sized (e.g., ≤ 240x320 pixels for full screen)
- **Orientation**: Standard BMP (bottom-up)

## Example

```bash
# Convert a logo
python3 bmp_to_c.py my_logo.bmp my_logo.h

# Use in code
#include "my_logo.h"
LCD_DrawBitmap(0, 0, (uint8_t*)my_logo_bmp);
```

## Notes

- Large images increase firmware size significantly
- Consider compressing or optimizing images before conversion
- Multiple small images are better than one large image if you need flexibility
