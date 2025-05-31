import pygame, json, time
import os

# Constants
CELL_SIZE = 64
WIDTH, HEIGHT = 10, 10  # board size
IMAGE_DIR = "images/"
DIRECTIONS = ["U", "UL", "UR", "L", "R", "D", "DL", "DR"]



def load_images():
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

    for d in DIRECTIONS:
        ext = ext_map[d]
        dir_name = dir_map[d]
        # Load red tank (player 1)
        images[f"tank_1_{d}"] = pygame.image.load(
            os.path.join(IMAGE_DIR, f"tank_red_{dir_name}.{ext}")
        )
        # Load blue tank (player 2)
        images[f"tank_2_{d}"] = pygame.image.load(
            os.path.join(IMAGE_DIR, f"tank_blue_{dir_name}.{ext}")
        )

    # Load other entities
    images["shell"] = pygame.image.load(os.path.join(IMAGE_DIR, "shell.png"))
    images["mine"] = pygame.image.load(os.path.join(IMAGE_DIR, "mine.png"))
    images["wall"] = pygame.image.load(os.path.join(IMAGE_DIR, "wall.png"))
    images["wall_damaged"] = pygame.image.load(os.path.join(IMAGE_DIR, "wall_damage.png"))
    
    return images

def draw_entity(screen, img, x, y):
    screen.blit(img, (x * CELL_SIZE, y * CELL_SIZE))

def render_round2(screen, data, images):
    screen.fill((0, 0, 0))

    # Draw walls
    for wall in data.get("walls", []):
        img = images["wall"]
        if wall.get("health", 2) == 1:
            img = images["wall_damaged"]
        draw_entity(screen, img, wall["x"], wall["y"])

    # Draw mines
    for mine in data.get("mines", []):
        draw_entity(screen, images["mine"], mine["x"], mine["y"])

    # Draw shells
    for shell in data.get("shells", []):
        draw_entity(screen, images["shell"], shell["x"], shell["y"])

    # Draw tanks - with more robust alive checking
    for tank in data.get("tanks", []):
        # Default to alive if field doesn't exist
        if not tank.get("alive", True):
            continue
        key = f"tank_{tank['player']}_{tank['dir']}"
        if key in images:
            draw_entity(screen, images[key], tank["x"], tank["y"])

    pygame.display.flip()

def render_round3(screen, data, images):
    screen.fill((0, 0, 0))  # Clear screen with black
    
    # Draw grid lines
    grid_color = (50, 50, 50)  # Dark gray color for grid
    for x in range(0, WIDTH * CELL_SIZE, CELL_SIZE):
        pygame.draw.line(screen, grid_color, (x, 0), (x, HEIGHT * CELL_SIZE))
    for y in range(0, HEIGHT * CELL_SIZE, CELL_SIZE):
        pygame.draw.line(screen, grid_color, (0, y), (WIDTH * CELL_SIZE, y))

    # Draw walls
    for wall in data.get("walls", []):
        img = images["wall"]
        if wall.get("health", 2) == 1:
            img = images["wall_damaged"]
        draw_entity(screen, img, wall["x"], wall["y"])

    # Draw mines
    for mine in data.get("mines", []):
        draw_entity(screen, images["mine"], mine["x"], mine["y"])

    # Draw shells
    for shell in data.get("shells", []):
        draw_entity(screen, images["shell"], shell["x"], shell["y"])

    # Draw tanks
    for tank in data.get("tanks", []):
        if not tank.get("alive", True):
            continue
        key = f"tank_{tank['player']}_{tank['dir']}"
        if key in images:
            draw_entity(screen, images[key], tank["x"], tank["y"])

    pygame.display.flip()

def render_round(screen, data, images):
    screen.fill((0, 0, 0))  # Clear screen
    
    # Draw grid
    grid_color = (50, 50, 50)  # Dark gray
    for x in range(0, WIDTH * CELL_SIZE, CELL_SIZE):
        pygame.draw.line(screen, grid_color, (x, 0), (x, HEIGHT * CELL_SIZE))
    for y in range(0, HEIGHT * CELL_SIZE, CELL_SIZE):
        pygame.draw.line(screen, grid_color, (0, y), (WIDTH * CELL_SIZE, y))

    # Render board elements from the 2D array
    for y, row in enumerate(data["board"]):
        for x, cell in enumerate(row):
            if cell == "#":  # Wall
                draw_entity(screen, images["wall"], x, y)
            elif cell == "@":  # Mine
                draw_entity(screen, images["mine"], x, y)
            elif cell == "$":  # Damaged wall (if present)
                draw_entity(screen, images["wall_damaged"], x, y)

    # Draw moving entities from their dedicated arrays
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
    pygame.init()
    screen = pygame.display.set_mode((WIDTH * CELL_SIZE, HEIGHT * CELL_SIZE))
    pygame.display.set_caption("Tank Battle Replay")
    
    try:
        images = load_images()
    except Exception as e:
        print(f"Error loading images: {e}")
        pygame.quit()
        return

    # Load and validate visualization file
    try:
        with open("visualization.json", 'r') as f:
            rounds = []
            for line_num, line in enumerate(f, 1):
                line = line.strip()
                if not line:  # Skip empty lines
                    continue
                try:
                    round_data = json.loads(line)
                    # Validate basic structure
                    if not all(key in round_data for key in ['step', 'board', 'tanks']):
                        print(f"Warning: Invalid structure in line {line_num}")
                        continue
                    rounds.append(round_data)
                except json.JSONDecodeError as e:
                    print(f"Error parsing line {line_num}: {e}")
                    print(f"Problematic line: {line[:100]}...")  # Show first 100 chars
                    continue

        if not rounds:
            print("Error: No valid rounds found in visualization.json")
            pygame.quit()
            return

        # Main visualization loop
        clock = pygame.time.Clock()
        for round_data in rounds:
            for event in pygame.event.get():
                if event.type == pygame.QUIT:
                    pygame.quit()
                    return
            
            try:
                render_round(screen, round_data, images)
            except Exception as e:
                print(f"Error rendering round {round_data.get('step', 'unknown')}: {e}")
                continue
            
            pygame.display.flip()
            clock.tick(2)  # 2 FPS (0.5 sec per frame) - better than time.sleep()

    except FileNotFoundError:
        print("Error: visualization.json not found")
    except Exception as e:
        print(f"Unexpected error: {e}")
    finally:
        pygame.quit()

if __name__ == "__main__":
    main()
