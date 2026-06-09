# 2D Graphics Editor in C

A beautiful, interactive, menu-driven command-line 2D Graphics Editor in C. The application lets you design and render graphical objects on a 2D character canvas (60 columns by 20 rows) using standard integer-based computer graphics algorithms.

The canvas is initially filled with the character `_` (underscore), and shapes are plotted using the character `*` (asterisk) with ANSI color highlights for a modern look.

---

## Features

1. **Complete Shape Catalog**:
   - **Line**: Drawn using Bresenham's Line Algorithm.
   - **Rectangle**: Plotted using border drawing.
   - **Triangle**: Drawn by plotting three line segments.
   - **Circle**: Rendered using the Midpoint Circle Algorithm.
2. **State Management**:
   - Add shapes dynamically.
   - Delete shapes by selecting their index.
   - Modify existing shapes by selecting their index and re-entering coordinates/dimensions.
   - Render overlapping shapes perfectly without screen artifacts.
3. **Interactive Menu Interface**:
   - Console menu with input validation to prevent crashes.
   - Full coordinates tracking system with vertical and horizontal numbers displayed around the canvas.
4. **Prepopulated Demo Scene**:
   - Loads a beautiful house, door, roof, ground, and sun on startup.

---

## File Structure

- `graphics_editor.c` - Core application source code.
- `compile.bat` - One-click compilation and execution script for Windows.
- `README.md` - Documentation.

---

## How to Build & Run

### Method 1: Using the compilation script (Windows)
Double-click `compile.bat` or run it in your terminal:
```powershell
.\compile.bat
```

### Method 2: Manual compilation with GCC
Open your terminal and run:
```bash
gcc -O2 -Wall -Wextra graphics_editor.c -o graphics_editor.exe
.\graphics_editor.exe
```

---

## How to Submit (Moodle & GitHub Setup Instructions)

As per your assignment guidelines:
1. **Submit the link to your GitHub repository in Moodle** (in the online text field provided).
2. **Daily push to GitHub is mandatory**.

Here is how to set up your GitHub repository and perform the initial push:

### 1. Create a Repository on GitHub
1. Log in to [GitHub](https://github.com).
2. Click **New** (or go to `https://github.com/new`).
3. Set Repository name to `2D-Graphics-Editor-C` (or any name you prefer).
4. Choose **Public** or **Private** (depending on your course requirements).
5. Do **NOT** initialize the repository with a README, `.gitignore`, or license (keep it completely empty).
6. Click **Create repository**.
7. Copy the Remote Repository URL (looks like `https://github.com/your-username/your-repo-name.git`).

### 2. Connect Your Local Directory to GitHub and Push
Run the following commands in your terminal (inside this directory):

```bash
# Add the remote GitHub repository URL
git remote add origin <PASTE_YOUR_GITHUB_REPOSITORY_URL_HERE>

# Rename default branch to main
git branch -M main

# Add and commit files
git add .
git commit -m "Initial commit: Add 2D graphics editor in C"

# Push to GitHub main branch
git push -u origin main
```

### 3. Making Daily Pushes
Whenever you make updates, commit and push them to keep your repository up to date:
```bash
git add .
git commit -m "Update: Describe changes here"
git push
```
