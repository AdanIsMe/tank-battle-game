import pygame
import json
import os
import sys

# Constants
TILE_SIZE = 32  # Your asset size
GRID_CELLS_WIDE = 10  # Board width in cells
GRID_CELLS_HIGH = 10  # Board height in cells
SCREEN_WIDTH = GRID_CELLS_WIDE * TILE_SIZE
SCREEN_HEIGHT = GRID_CELLS_HIGH * TILE_SIZE
IMAGE_DIR = "images/"
DIRECTIONS = ["U", "UL", "UR", "L", "R", "D", "DL", "DR"]
GRID_COLOR = (50, 50, 50)  # Dark gray grid lines
#BACKGROUND_COLOR = (0, 150, 0) 
BACKGROUND_COLOR = (0, 0, 0)  # Black background

def load_images():
    """Load and scale all game images to 32x32 pixels"""
    images = {}
    # Map direction abbreviations to extensions
    ext_map = {
        "U": "png", "D": "png", "L": "png", "R": "png",  # Cardinal directions
        "UL": "jpg", "UR": "jpg", "DL": "jpg", "DR": "jpg"  # Diagonal directions
    }

    # Map direction abbreviations to filename parts
    dir_map = {
        "U": "up", "D": "down", "L": "left", "R": "right",
        "UL": "up_left", "UR": "up_right",
        "DL": "down_left", "DR": "down_right"
    }

    try:
        # Load tank images
        for d in DIRECTIONS:
            ext = ext_map[d]
            dir_name = dir_map[d]
            
            # Player 1 (red) tanks
            red_path = os.path.join(IMAGE_DIR, f"tank_red_{dir_name}.{ext}")
            if os.path.exists(red_path):
                img = pygame.image.load(red_path)
                images[f"tank_1_{d}"] = pygame.transform.scale(img, (TILE_SIZE, TILE_SIZE))
            
            # Player 2 (blue) tanks
            blue_path = os.path.join(IMAGE_DIR, f"tank_blue_{dir_name}.{ext}")
            if os.path.exists(blue_path):
                img = pygame.image.load(blue_path)
                images[f"tank_2_{d}"] = pygame.transform.scale(img, (TILE_SIZE, TILE_SIZE))

        # Load other entities
        entity_images = {
            "wall": "wall.png",
            "wall_damaged": "wall_damage.png",
            "mine": "mine.png",
            "shell": "shell.png"
        }

        for name, filename in entity_images.items():
            path = os.path.join(IMAGE_DIR, filename)
            if os.path.exists(path):
                img = pygame.image.load(path)
                images[name] = pygame.transform.scale(img, (TILE_SIZE, TILE_SIZE))

    except Exception as e:
        print(f"Error loading images: {e}")
        raise

    # Verify all required images loaded
    required_images = ["wall", "mine", "shell"] + \
                     [f"tank_{player}_{d}" for player in [1,2] for d in DIRECTIONS]
    for img in required_images:
        if img not in images:
            print(f"Warning: Missing image {img}")

    return images

def draw_entity(screen, img, x, y):
    """Draw an entity at grid position (x,y)"""
    screen.blit(img, (x * TILE_SIZE, y * TILE_SIZE))

def render_round(screen, data, images):
    """Render one game frame"""
    # Clear screen
    screen.fill(BACKGROUND_COLOR)
    
    # Draw grid lines
    for x in range(0, SCREEN_WIDTH + TILE_SIZE, TILE_SIZE):
        pygame.draw.line(screen, GRID_COLOR, (x, 0), (x, SCREEN_HEIGHT))
    for y in range(0, SCREEN_HEIGHT + TILE_SIZE, TILE_SIZE):
        pygame.draw.line(screen, GRID_COLOR, (0, y), (SCREEN_WIDTH, y))

    # Render board elements from the 2D array
    for y, row in enumerate(data.get("board", [])):
        for x, cell in enumerate(row):
            if cell == "#":  # Wall
                draw_entity(screen, images["wall"], x, y)
            elif cell == "@":  # Mine
                draw_entity(screen, images["mine"], x, y)
            elif cell == "$":  # Damaged wall
                draw_entity(screen, images.get("wall_damaged", images["wall"]), x, y)

    # Draw moving entities from their arrays
    for shell in data.get("shells", []):
        draw_entity(screen, images["shell"], shell["x"], shell["y"])

    for tank in data.get("tanks", []):
        if not tank.get("alive", True):
            continue
        key = f"tank_{tank['player']}_{tank['dir']}"
        if key in images:
            draw_entity(screen, images[key], tank["x"], tank["y"])

    pygame.display.flip()

def main():
    """Main game loop"""
    pygame.init()
    screen = pygame.display.set_mode((SCREEN_WIDTH, SCREEN_HEIGHT))
    pygame.display.set_caption("Tank Battle Visualizer")
    
    try:
        images = load_images()
        print("Successfully loaded all images")
    except Exception as e:
        print(f"Failed to load images: {e}")
        pygame.quit()
        sys.exit(1)

    try:
        with open("visualization.json") as f:
            rounds = []
            for line in f:
                line = line.strip()
                if line:
                    try:
                        round_data = json.loads(line)
                        # Basic validation
                        if "board" not in round_data:
                            print("Warning: Missing 'board' in round data")
                            continue
                        rounds.append(round_data)
                    except json.JSONDecodeError as e:
                        print(f"Invalid JSON: {e}")
                        continue

        if not rounds:
            print("Error: No valid rounds found")
            pygame.quit()
            sys.exit(1)

        print(f"Loaded {len(rounds)} rounds")
        clock = pygame.time.Clock()
        current_round = 0

        # Main game loop
        running = True
        while running and current_round < len(rounds):
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    running = False

            # Render current round
            render_round(screen, rounds[current_round], images)
            current_round += 1
            clock.tick(2)  # 2 FPS (0.5 seconds per frame)

    except FileNotFoundError:
        print("Error: visualization.json not found")
    except Exception as e:
        print(f"Error: {e}")
    finally:
        pygame.quit()
        sys.exit(0)

if __name__ == "__main__":
    main()