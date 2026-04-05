# Sketch_3D

A Qt/OpenGL desktop application for creating and visualizing 2D shapes with real-time 3D preview.

---

## Features

- **Sketch Mode** – Create circles, rectangles, and squares by entering dimensions via dialog
- **Edit Mode** – Drag shape corners/edges to resize interactively
- **3D Mode** – View your shape extruded into a 3D object; click and drag to rotate

---

## Requirements

| Dependency | Version |
|---|---|
| Qt | 5.x or 6.x |
| OpenGL | 2.1+ |
| C++ | 11 or later |
| CMake / qmake | Any recent version |

---

## Project Structure

```
├── main.cpp          # Entry point
├── mainwindow.h/.cpp # Main window, toolbar, mode switching
├── glwidget.h/.cpp   # OpenGL rendering widget (2D + 3D)
```

---

## How to Use

1. **Launch** the application — the left panel shows controls, the right panel is the canvas.

2. **Sketch Mode** (default)
   - Click **Square**, **Rectangle**, or **Circle**
   - Enter the dimensions in the dialog
   - The shape appears centered on the canvas

3. **Edit Mode**
   - Click **Edit Mode**
   - Drag near a corner (rectangle/square) or the edge (circle) to resize

4. **3D Mode**
   - Click **3D Mode**
   - The shape is extruded into a 3D cuboid or sphere
   - Click and drag on the canvas to rotate the object

---

## Shapes & Their 3D Counterparts

| 2D Shape | 3D Shape |
|---|---|
| Circle | Sphere |
| Rectangle | Cuboid |
| Square | Cuboid (equal sides) |

---
## Screenshots

### APp
<img width="1125" height="788" alt="image" src="https://github.com/user-attachments/assets/0b4c8f7a-9618-4d52-9043-166acd6e8b7a" />

### Edit Mode
<img width="1120" height="790" alt="image" src="https://github.com/user-attachments/assets/65e17bf4-5b2c-464d-94c6-285a38a641aa" />

### 3D Mode
<img width="1117" height="785" alt="image" src="https://github.com/user-attachments/assets/1198a46c-b5ed-43a5-966d-1c0372ca5c13" />
<img width="1119" height="781" alt="image" src="https://github.com/user-attachments/assets/5cf37821-1da4-486c-a38f-170ca5284e3f" />

