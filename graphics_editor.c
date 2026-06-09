#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

#define ROWS 20
#define COLS 60
#define MAX_SHAPES 100

// ANSI Color Codes for enhanced visual style
#define ANSI_RESET   "\033[0m"
#define ANSI_CYAN    "\033[1;36m"
#define ANSI_GRAY    "\033[90m"
#define ANSI_YELLOW  "\033[1;33m"
#define ANSI_GREEN   "\033[1;32m"
#define ANSI_RED     "\033[1;31m"
#define ANSI_MAGENTA "\033[1;35m"

typedef enum {
    LINE,
    RECTANGLE,
    TRIANGLE,
    CIRCLE
} ShapeType;

typedef struct {
    int x1, y1;
    int x2, y2;
} LineParams;

typedef struct {
    int x, y;
    int width, height;
} RectParams;

typedef struct {
    int x1, y1;
    int x2, y2;
    int x3, y3;
} TriParams;

typedef struct {
    int cx, cy;
    int r;
} CircleParams;

typedef struct {
    ShapeType type;
    union {
        LineParams line;
        RectParams rect;
        TriParams tri;
        CircleParams circle;
    } data;
} Shape;

// Global Application State
Shape shapes[MAX_SHAPES];
int shape_count = 0;
char canvas[ROWS][COLS];

// Helper to clear standard input buffer to prevent parsing loops
void clear_input_buffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

// Read integer safely from stdin with bounds checking
int read_int(const char* prompt, int min_val, int max_val) {
    int val;
    char line[256];
    while (1) {
        printf("%s", prompt);
        if (fgets(line, sizeof(line), stdin)) {
            // Check if we successfully parsed an integer
            if (sscanf(line, "%d", &val) == 1) {
                if (val >= min_val && val <= max_val) {
                    return val;
                }
            }
        }
        printf(ANSI_RED "Invalid input. Please enter an integer between %d and %d." ANSI_RESET "\n", min_val, max_val);
    }
}

// ---------------------------------------------------------
// Core Drawing Functions
// ---------------------------------------------------------

// Bresenham's Line Algorithm
void draw_line_on_canvas(int x1, int y1, int x2, int y2) {
    int dx = abs(x2 - x1);
    int sx = x1 < x2 ? 1 : -1;
    int dy = -abs(y2 - y1);
    int sy = y1 < y2 ? 1 : -1;
    int err = dx + dy;
    int e2;

    while (1) {
        if (x1 >= 0 && x1 < COLS && y1 >= 0 && y1 < ROWS) {
            canvas[y1][x1] = '*';
        }
        if (x1 == x2 && y1 == y2) break;
        e2 = 2 * err;
        if (e2 >= dy) {
            err += dy;
            x1 += sx;
        }
        if (e2 <= dx) {
            err += dx;
            y1 += sy;
        }
    }
}

// Draw Rectangle Borders
void draw_rect_on_canvas(int x, int y, int w, int h) {
    if (w <= 0 || h <= 0) return;
    
    // Top and bottom borders
    for (int col = x; col < x + w; col++) {
        if (col >= 0 && col < COLS) {
            if (y >= 0 && y < ROWS) canvas[y][col] = '*';
            if (y + h - 1 >= 0 && y + h - 1 < ROWS) canvas[y + h - 1][col] = '*';
        }
    }
    // Left and right borders
    for (int row = y; row < y + h; row++) {
        if (row >= 0 && row < ROWS) {
            if (x >= 0 && x < COLS) canvas[row][x] = '*';
            if (x + w - 1 >= 0 && x + w - 1 < COLS) canvas[row][x + w - 1] = '*';
        }
    }
}

// Draw Triangle (3 lines connecting vertices)
void draw_triangle_on_canvas(int x1, int y1, int x2, int y2, int x3, int y3) {
    draw_line_on_canvas(x1, y1, x2, y2);
    draw_line_on_canvas(x2, y2, x3, y3);
    draw_line_on_canvas(x3, y3, x1, y1);
}

// Midpoint Circle Algorithm (Bresenham's Circle)
void draw_circle_on_canvas(int cx, int cy, int r) {
    if (r < 0) return;
    if (r == 0) {
        if (cx >= 0 && cx < COLS && cy >= 0 && cy < ROWS) {
            canvas[cy][cx] = '*';
        }
        return;
    }

    int x = 0;
    int y = r;
    int d = 3 - 2 * r;

    void plot_circle_points(int cx, int cy, int x, int y) {
        int px[] = {cx + x, cx - x, cx + x, cx - x, cx + y, cx - y, cx + y, cx - y};
        int py[] = {cy + y, cy + y, cy - y, cy - y, cy + x, cy + x, cy - x, cy - x};
        for (int i = 0; i < 8; i++) {
            if (px[i] >= 0 && px[i] < COLS && py[i] >= 0 && py[i] < ROWS) {
                canvas[py[i]][px[i]] = '*';
            }
        }
    }

    plot_circle_points(cx, cy, x, y);

    while (y >= x) {
        x++;
        if (d > 0) {
            y--;
            d = d + 4 * (x - y) + 10;
        } else {
            d = d + 4 * x + 6;
        }
        plot_circle_points(cx, cy, x, y);
    }
}

// ---------------------------------------------------------
// Canvas & Shape List Rendering
// ---------------------------------------------------------

// Clear canvas and draw all active shapes in order of creation
void render_all_shapes() {
    // Fill canvas with underscore
    for (int y = 0; y < ROWS; y++) {
        for (int x = 0; x < COLS; x++) {
            canvas[y][x] = '_';
        }
    }

    // Redraw all shapes
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case LINE:
                draw_line_on_canvas(s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case RECTANGLE:
                draw_rect_on_canvas(s.data.rect.x, s.data.rect.y, s.data.rect.width, s.data.rect.height);
                break;
            case TRIANGLE:
                draw_triangle_on_canvas(s.data.tri.x1, s.data.tri.y1, s.data.tri.x2, s.data.tri.y2, s.data.tri.x3, s.data.tri.y3);
                break;
            case CIRCLE:
                draw_circle_on_canvas(s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
        }
    }
}

// Print the canvas to stdout with borders and coordinate guides
void display_canvas() {
    render_all_shapes();

    printf("\n");
    // Print column headers (tens digits)
    printf("     ");
    for (int x = 0; x < COLS; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n");

    // Print column headers (units digits)
    printf("     ");
    for (int x = 0; x < COLS; x++) {
        printf("%d", x % 10);
    }
    printf("\n");

    // Print top border
    printf("    +");
    for (int x = 0; x < COLS; x++) {
        printf("-");
    }
    printf("+\n");

    // Print canvas rows with left/right borders and row coordinates
    for (int y = 0; y < ROWS; y++) {
        printf("%2d  |", y);
        for (int x = 0; x < COLS; x++) {
            if (canvas[y][x] == '*') {
                printf(ANSI_CYAN "*" ANSI_RESET);
            } else {
                printf(ANSI_GRAY "_" ANSI_RESET);
            }
        }
        printf("|  %d\n", y);
    }

    // Print bottom border
    printf("    +");
    for (int x = 0; x < COLS; x++) {
        printf("-");
    }
    printf("+\n");

    // Print column headers at the bottom
    printf("     ");
    for (int x = 0; x < COLS; x++) {
        if (x % 10 == 0) {
            printf("%d", x / 10);
        } else {
            printf(" ");
        }
    }
    printf("\n     ");
    for (int x = 0; x < COLS; x++) {
        printf("%d", x % 10);
    }
    printf("\n\n");
}

// Print active shapes with details
void print_shapes_list() {
    printf("\n" ANSI_YELLOW "--- Active Shapes List ---" ANSI_RESET "\n");
    if (shape_count == 0) {
        printf("No shapes added yet.\n");
        return;
    }
    printf("%-4s | %-12s | %-s\n", "ID", "Shape Type", "Parameters");
    printf("----------------------------------------\n");
    for (int i = 0; i < shape_count; i++) {
        Shape s = shapes[i];
        switch (s.type) {
            case LINE:
                printf("[%2d] | Line         | (%d, %d) to (%d, %d)\n", i, s.data.line.x1, s.data.line.y1, s.data.line.x2, s.data.line.y2);
                break;
            case RECTANGLE:
                printf("[%2d] | Rectangle    | Top-Left (%d, %d), W: %d, H: %d\n", i, s.data.rect.x, s.data.rect.y, s.data.rect.width, s.data.rect.height);
                break;
            case TRIANGLE:
                printf("[%2d] | Triangle     | P1(%d, %d), P2(%d, %d), P3(%d, %d)\n", i, s.data.tri.x1, s.data.tri.y1, s.data.tri.x2, s.data.tri.y2, s.data.tri.x3, s.data.tri.y3);
                break;
            case CIRCLE:
                printf("[%2d] | Circle       | Center (%d, %d), Radius: %d\n", i, s.data.circle.cx, s.data.circle.cy, s.data.circle.r);
                break;
        }
    }
    printf("\n");
}

// ---------------------------------------------------------
// Menu-driven Operations
// ---------------------------------------------------------

// Menu to add a shape
void add_shape_menu() {
    if (shape_count >= MAX_SHAPES) {
        printf(ANSI_RED "Error: Canvas state is full! Delete standard objects before adding." ANSI_RESET "\n");
        return;
    }

    printf("\n" ANSI_MAGENTA "--- Add New Shape ---" ANSI_RESET "\n");
    printf("1. Line\n");
    printf("2. Rectangle\n");
    printf("3. Triangle\n");
    printf("4. Circle\n");
    printf("5. Back to Main Menu\n");
    int choice = read_int("Choose shape type (1-5): ", 1, 5);

    if (choice == 5) return;

    Shape new_shape;
    switch (choice) {
        case 1:
            new_shape.type = LINE;
            new_shape.data.line.x1 = read_int("Enter Start X (0 to 59): ", 0, COLS - 1);
            new_shape.data.line.y1 = read_int("Enter Start Y (0 to 19): ", 0, ROWS - 1);
            new_shape.data.line.x2 = read_int("Enter End X (0 to 59): ", 0, COLS - 1);
            new_shape.data.line.y2 = read_int("Enter End Y (0 to 19): ", 0, ROWS - 1);
            break;
        case 2:
            new_shape.type = RECTANGLE;
            new_shape.data.rect.x = read_int("Enter Top-Left X (0 to 59): ", 0, COLS - 1);
            new_shape.data.rect.y = read_int("Enter Top-Left Y (0 to 19): ", 0, ROWS - 1);
            new_shape.data.rect.width = read_int("Enter Width (1 to 60): ", 1, COLS);
            new_shape.data.rect.height = read_int("Enter Height (1 to 20): ", 1, ROWS);
            break;
        case 3:
            new_shape.type = TRIANGLE;
            new_shape.data.tri.x1 = read_int("Enter Vertex 1 X (0 to 59): ", 0, COLS - 1);
            new_shape.data.tri.y1 = read_int("Enter Vertex 1 Y (0 to 19): ", 0, ROWS - 1);
            new_shape.data.tri.x2 = read_int("Enter Vertex 2 X (0 to 59): ", 0, COLS - 1);
            new_shape.data.tri.y2 = read_int("Enter Vertex 2 Y (0 to 19): ", 0, ROWS - 1);
            new_shape.data.tri.x3 = read_int("Enter Vertex 3 X (0 to 59): ", 0, COLS - 1);
            new_shape.data.tri.y3 = read_int("Enter Vertex 3 Y (0 to 19): ", 0, ROWS - 1);
            break;
        case 4:
            new_shape.type = CIRCLE;
            new_shape.data.circle.cx = read_int("Enter Center X (0 to 59): ", 0, COLS - 1);
            new_shape.data.circle.cy = read_int("Enter Center Y (0 to 19): ", 0, ROWS - 1);
            new_shape.data.circle.r = read_int("Enter Radius (0 to 30): ", 0, 30);
            break;
    }

    shapes[shape_count++] = new_shape;
    printf(ANSI_GREEN "Shape added successfully!" ANSI_RESET "\n");
}

// Menu to delete a shape
void delete_shape_menu() {
    printf("\n" ANSI_MAGENTA "--- Delete a Shape ---" ANSI_RESET "\n");
    if (shape_count == 0) {
        printf("No shapes available to delete.\n");
        return;
    }

    print_shapes_list();
    int idx = read_int("Enter the ID of the shape you want to delete (or -1 to cancel): ", -1, shape_count - 1);
    if (idx == -1) return;

    // Shift shapes to close the gap
    for (int i = idx; i < shape_count - 1; i++) {
        shapes[i] = shapes[i + 1];
    }
    shape_count--;
    printf(ANSI_GREEN "Shape deleted successfully!" ANSI_RESET "\n");
}

// Menu to modify a shape
void modify_shape_menu() {
    printf("\n" ANSI_MAGENTA "--- Modify a Shape ---" ANSI_RESET "\n");
    if (shape_count == 0) {
        printf("No shapes available to modify.\n");
        return;
    }

    print_shapes_list();
    int idx = read_int("Enter the ID of the shape you want to modify (or -1 to cancel): ", -1, shape_count - 1);
    if (idx == -1) return;

    Shape* s = &shapes[idx];
    printf("\nModifying ");
    switch (s->type) {
        case LINE:
            printf("Line:\n");
            s->data.line.x1 = read_int("Enter New Start X (0 to 59): ", 0, COLS - 1);
            s->data.line.y1 = read_int("Enter New Start Y (0 to 19): ", 0, ROWS - 1);
            s->data.line.x2 = read_int("Enter New End X (0 to 59): ", 0, COLS - 1);
            s->data.line.y2 = read_int("Enter New End Y (0 to 19): ", 0, ROWS - 1);
            break;
        case RECTANGLE:
            printf("Rectangle:\n");
            s->data.rect.x = read_int("Enter New Top-Left X (0 to 59): ", 0, COLS - 1);
            s->data.rect.y = read_int("Enter New Top-Left Y (0 to 19): ", 0, ROWS - 1);
            s->data.rect.width = read_int("Enter New Width (1 to 60): ", 1, COLS);
            s->data.rect.height = read_int("Enter New Height (1 to 20): ", 1, ROWS);
            break;
        case TRIANGLE:
            printf("Triangle:\n");
            s->data.tri.x1 = read_int("Enter New Vertex 1 X (0 to 59): ", 0, COLS - 1);
            s->data.tri.y1 = read_int("Enter New Vertex 1 Y (0 to 19): ", 0, ROWS - 1);
            s->data.tri.x2 = read_int("Enter New Vertex 2 X (0 to 59): ", 0, COLS - 1);
            s->data.tri.y2 = read_int("Enter New Vertex 2 Y (0 to 19): ", 0, ROWS - 1);
            s->data.tri.x3 = read_int("Enter New Vertex 3 X (0 to 59): ", 0, COLS - 1);
            s->data.tri.y3 = read_int("Enter New Vertex 3 Y (0 to 19): ", 0, ROWS - 1);
            break;
        case CIRCLE:
            printf("Circle:\n");
            s->data.circle.cx = read_int("Enter New Center X (0 to 59): ", 0, COLS - 1);
            s->data.circle.cy = read_int("Enter New Center Y (0 to 19): ", 0, ROWS - 1);
            s->data.circle.r = read_int("Enter New Radius (0 to 30): ", 0, 30);
            break;
    }
    printf(ANSI_GREEN "Shape modified successfully!" ANSI_RESET "\n");
}

// ---------------------------------------------------------
// Prepopulated Demo Scene
// ---------------------------------------------------------
void load_demo_scene() {
    shape_count = 0;

    // 1. Ground Line
    shapes[shape_count++] = (Shape){
        .type = LINE,
        .data.line = { .x1 = 0, .y1 = 18, .x2 = 59, .y2 = 18 }
    };

    // 2. House Base (Rectangle)
    shapes[shape_count++] = (Shape){
        .type = RECTANGLE,
        .data.rect = { .x = 22, .y = 8, .width = 17, .height = 10 }
    };

    // 3. House Roof (Triangle)
    shapes[shape_count++] = (Shape){
        .type = TRIANGLE,
        .data.tri = { .x1 = 20, .y1 = 8, .x2 = 30, .y2 = 2, .x3 = 40, .y3 = 8 }
    };

    // 4. Door (Rectangle)
    shapes[shape_count++] = (Shape){
        .type = RECTANGLE,
        .data.rect = { .x = 28, .y = 12, .width = 5, .height = 6 }
    };

    // 5. Sun (Circle)
    shapes[shape_count++] = (Shape){
        .type = CIRCLE,
        .data.circle = { .cx = 50, .cy = 4, .r = 3 }
    };

    printf(ANSI_GREEN "Demo scene loaded! Run option 4 to view it." ANSI_RESET "\n");
}

// ---------------------------------------------------------
// Main Application Loop
// ---------------------------------------------------------
int main() {
    // Print Welcome Banner
    printf(ANSI_CYAN "======================================================" ANSI_RESET "\n");
    printf(ANSI_CYAN "          2D CHARACTER GRAPHICS CANVAS EDITOR         " ANSI_RESET "\n");
    printf(ANSI_CYAN "======================================================" ANSI_RESET "\n");
    printf("Initially fills the 60x20 canvas with '" ANSI_GRAY "_" ANSI_RESET "' and draws with '" ANSI_CYAN "*" ANSI_RESET "'.\n");
    
    // Load the demo scene on startup so the canvas isn't blank
    load_demo_scene();

    while (1) {
        printf("\n" ANSI_YELLOW "--- Main Menu ---" ANSI_RESET "\n");
        printf("1. Add a Shape\n");
        printf("2. Delete a Shape\n");
        printf("3. Modify an Existing Shape\n");
        printf("4. Display Canvas (Render Picture)\n");
        printf("5. List Active Shapes\n");
        printf("6. Clear All Shapes (Reset)\n");
        printf("7. Reload Demo Scene\n");
        printf("8. Exit Editor\n");

        int choice = read_int("Choose menu option (1-8): ", 1, 8);

        switch (choice) {
            case 1:
                add_shape_menu();
                break;
            case 2:
                delete_shape_menu();
                break;
            case 3:
                modify_shape_menu();
                break;
            case 4:
                display_canvas();
                break;
            case 5:
                print_shapes_list();
                break;
            case 6:
                shape_count = 0;
                printf(ANSI_GREEN "Canvas reset complete. All shapes removed." ANSI_RESET "\n");
                break;
            case 7:
                load_demo_scene();
                break;
            case 8:
                printf("\nThank you for using the 2D Graphics Editor. Goodbye!\n");
                return 0;
        }
    }
}
