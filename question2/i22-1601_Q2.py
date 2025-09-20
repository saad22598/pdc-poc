import asyncio
import platform
import pygame
import numpy as np

# Student: Syed Saad Mohsin
# ID: 221-1601
# PDC Assignment 1

# Visualization settings
SCREEN_WIDTH, SCREEN_HEIGHT = 800, 600
SQUARE_SIZE = 40
FRAME_RATE = 10  # Slow down for visualization

# Color definitions
BACKGROUND = (255, 255, 255)
GRID_COLOR = (0, 0, 0)
HIGHLIGHT = (200, 200, 200)
CURRENT_CELL = (0, 0, 255)
RESULT_COLOR = (255, 0, 0)

# Input sequences
str1 = "ACGTAG"
str2 = "ACTG"

def initialize_display():
    global screen, font, rows, cols, dp_table, row_idx, col_idx, is_running
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Dynamic Programming Distance Visualization")
    font = pygame.font.Font(None, 24)
    
    # Initialize DP table
    rows, cols = len(str1), len(str2)
    dp_table = np.zeros((rows + 1, cols + 1), dtype=int)
    
    # Initialize first row and column
    for i in range(rows + 1):
        dp_table[i][0] = i
    for j in range(cols + 1):
        dp_table[0][j] = j
    
    # Variables for step-by-step computation
    row_idx, col_idx = 1, 1
    is_running = True

def render_table():
    screen.fill(BACKGROUND)
    
    # Draw grid
    for row in range(rows + 1):
        for col in range(cols + 1):
            rect = pygame.Rect(col * SQUARE_SIZE + 50, row * SQUARE_SIZE + 50, SQUARE_SIZE, SQUARE_SIZE)
            pygame.draw.rect(screen, GRID_COLOR, rect, 1)
            
            # Highlight current cell
            if row == row_idx and col == col_idx:
                pygame.draw.rect(screen, CURRENT_CELL, rect)
            
            # Draw table values
            value = dp_table[row][col]
            text = font.render(str(value), True, GRID_COLOR)
            text_rect = text.get_rect(center=rect.center)
            screen.blit(text, text_rect)
    
    # Draw sequence labels
    for idx, char in enumerate(str1):
        text = font.render(char, True, GRID_COLOR)
        screen.blit(text, (50 + (idx + 1) * SQUARE_SIZE, 20))
    for idx, char in enumerate(str2):
        text = font.render(char, True, GRID_COLOR)
        screen.blit(text, (20, 50 + (idx + 1) * SQUARE_SIZE))
    
    # Draw result
    if row_idx > rows or col_idx > cols:
        result_text = font.render(f"Sequence Distance: {dp_table[rows][cols]}", True, RESULT_COLOR)
        screen.blit(result_text, (50, SCREEN_HEIGHT - 50))
    
    pygame.display.flip()

async def computation_step():
    global row_idx, col_idx, is_running
    if is_running and row_idx <= rows and col_idx <= cols:
        # Compute current cell
        cost = 0 if str1[row_idx-1] == str2[col_idx-1] else 1
        dp_table[row_idx][col_idx] = min(
            dp_table[row_idx-1][col_idx] + 1,    # Deletion
            dp_table[row_idx][col_idx-1] + 1,    # Insertion
            dp_table[row_idx-1][col_idx-1] + cost # Substitution
        )
        
        # Move to next cell
        col_idx += 1
        if col_idx > cols:
            col_idx = 1
            row_idx += 1
    
    # Draw the table
    render_table()
    
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            is_running = False

async def main_loop():
    initialize_display()
    while is_running:
        await computation_step()
        await asyncio.sleep(1.0 / FRAME_RATE)

if platform.system() == "Emscripten":
    asyncio.ensure_future(main_loop())
else:
    if __name__ == "__main__":
        asyncio.run(main_loop())
