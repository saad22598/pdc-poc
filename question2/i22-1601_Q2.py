import asyncio
import platform
import pygame
import numpy as np

# Name: Syed Saad Mohsin
# Roll Number: 221-1601
# Assignment 1 for PDC

# Constants for visualization
WIDTH, HEIGHT = 800, 600
CELL_SIZE = 40
FPS = 10  # Slow down for visualization

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY = (200, 200, 200)
BLUE = (0, 0, 255)
RED = (255, 0, 0)

# Input sequences
seq1 = "ACGTAG"
seq2 = "ACTG"

def setup():
    global screen, font, m, n, matrix, i, j, running
    pygame.init()
    screen = pygame.display.set_mode((WIDTH, HEIGHT))
    pygame.display.set_caption("Needleman-Wunsch DP Visualization")
    font = pygame.font.Font(None, 24)
    
    # Initialize DP matrix
    m, n = len(seq1), len(seq2)
    matrix = np.zeros((m + 1, n + 1), dtype=int)
    
    # Initialize first row and column
    for i in range(m + 1):
        matrix[i][0] = i
    for j in range(n + 1):
        matrix[0][j] = j
    
    # Variables for step-by-step computation
    i, j = 1, 1
    running = True

def draw_matrix():
    screen.fill(WHITE)
    
    # Draw grid
    for row in range(m + 1):
        for col in range(n + 1):
            rect = pygame.Rect(col * CELL_SIZE + 50, row * CELL_SIZE + 50, CELL_SIZE, CELL_SIZE)
            pygame.draw.rect(screen, BLACK, rect, 1)
            
            # Highlight current cell
            if row == i and col == j:
                pygame.draw.rect(screen, BLUE, rect)
            
            # Draw matrix values
            value = matrix[row][col]
            text = font.render(str(value), True, BLACK)
            text_rect = text.get_rect(center=rect.center)
            screen.blit(text, text_rect)
    
    # Draw sequence labels
    for idx, char in enumerate(seq1):
        text = font.render(char, True, BLACK)
        screen.blit(text, (50 + (idx + 1) * CELL_SIZE, 20))
    for idx, char in enumerate(seq2):
        text = font.render(char, True, BLACK)
        screen.blit(text, (20, 50 + (idx + 1) * CELL_SIZE))
    
    # Draw result
    if i > m or j > n:
        result_text = font.render(f"Edit Distance: {matrix[m][n]}", True, RED)
        screen.blit(result_text, (50, HEIGHT - 50))
    
    pygame.display.flip()

async def update_loop():
    global i, j, running
    if running and i <= m and j <= n:
        # Compute current cell
        cost = 0 if seq1[i-1] == seq2[j-1] else 1
        matrix[i][j] = min(
            matrix[i-1][j] + 1,    # Deletion
            matrix[i][j-1] + 1,    # Insertion
            matrix[i-1][j-1] + cost # Substitution
        )
        
        # Move to next cell
        j += 1
        if j > n:
            j = 1
            i += 1
    
    # Draw the matrix
    draw_matrix()
    
    # Handle events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False

async def main():
    setup()
    while running:
        await update_loop()
        await asyncio.sleep(1.0 / FPS)

if platform.system() == "Emscripten":
    asyncio.ensure_future(main())
else:
    if __name__ == "__main__":
        asyncio.run(main())
