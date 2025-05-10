import pygame
import sys
import time

# === VISUAL CONFIG ===
CELL_SIZE = 80
STEP_DELAY = 1.2  # seconds per step

# Colors
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
GRAY  = (190, 190, 190)
GREEN = (0, 255, 0)
RED   = (255, 0, 0)
BLUE  = (0, 0, 255)

# 8-way directions
DIRECTION_MAP = {
    'U':  (0, -1), 'UR': (1, -1), 'R':  (1,  0), 'DR': (1, 1),
    'D':  (0,  1), 'DL': (-1, 1), 'L':  (-1, 0), 'UL': (-1, -1)
}

# Globals set during parsing
GRID_WIDTH  = 0
GRID_HEIGHT = 0

def parse_game_log(filepath):
    global GRID_WIDTH, GRID_HEIGHT

    with open(filepath) as f:
        # keep blank lines, strip trailing newline
        lines = [line.rstrip('\n') for line in f]

    i = 0
    # 1) Read board size
    while i < len(lines) and not lines[i].startswith("Board size:"):
        i += 1
    if i == len(lines):
        print("Error: No 'Board size:' line found in log.")
        sys.exit(1)

    # parse "Board size: WxH"
    size_str = lines[i].split(":",1)[1].strip()
    try:
        w,h = size_str.lower().split("x")
        GRID_WIDTH, GRID_HEIGHT = int(w), int(h)
    except:
        print(f"Error: Invalid board size format: '{size_str}'")
        sys.exit(1)
    i += 1

    steps = []
    # 2) Parse each Step block
    while i < len(lines):
        line = lines[i].strip()
        if line.startswith("Step"):
            # new step
            step = {'grid': [], 'dirs': {}}
            i += 1

            # 2a) Read exactly GRID_HEIGHT lines of grid
            for _ in range(GRID_HEIGHT):
                if i < len(lines):
                    row = lines[i]
                    # pad or truncate to GRID_WIDTH
                    row = row.ljust(GRID_WIDTH)[:GRID_WIDTH]
                    step['grid'].append(list(row))
                else:
                    # missing row → fill with spaces
                    step['grid'].append([' ']*GRID_WIDTH)
                i += 1

            # 2b) Expect "Directions:" line
            if i < len(lines) and lines[i].startswith("Directions:"):
                i += 1
                # read two tank directions
                for _ in range(2):
                    if i < len(lines) and ':' in lines[i]:
                        pid, dirstr = lines[i].split(":",1)
                        step['dirs'][int(pid.strip())] = dirstr.strip()
                    i += 1

            steps.append(step)
        else:
            i += 1

    return steps

def draw_grid(screen, grid, dirs):
    for y, row in enumerate(grid):
        for x, ch in enumerate(row):
            rect = pygame.Rect(x*CELL_SIZE, y*CELL_SIZE, CELL_SIZE, CELL_SIZE)
            pygame.draw.rect(screen, GRAY, rect, 1)
            cx, cy = rect.center

            if ch == '#':
                pygame.draw.rect(screen, BLACK, rect.inflate(-10, -10))
            elif ch == '@':
                pygame.draw.circle(screen, GRAY, (cx, cy), 10)
            elif ch == 'o':
                pygame.draw.circle(screen, BLUE, (cx, cy), 8)
            elif ch in ('1','2'):
                color = GREEN if ch=='1' else RED
                pygame.draw.rect(screen, color, rect.inflate(-20, -20))
                # draw cannon
                dirstr = dirs.get(int(ch), 'U')
                dx, dy = DIRECTION_MAP.get(dirstr, (0,-1))
                pygame.draw.line(screen, BLACK, (cx, cy), (cx+dx*20, cy+dy*20), 3)

def run_visualizer(log_file):
    pygame.init()
    steps = parse_game_log(log_file)
    
    width_px  = GRID_WIDTH  * CELL_SIZE
    height_px = GRID_HEIGHT * CELL_SIZE
    screen = pygame.display.set_mode((width_px, height_px))
    pygame.display.set_caption("Tank Game Visualizer")

    clock = pygame.time.Clock()
    step_idx = 0
    last_time = time.time()

    running = True
    while running:
        now = time.time()
        # advance step after STEP_DELAY
        if step_idx < len(steps) and (now - last_time) >= STEP_DELAY:
            step_idx += 1
            last_time = now

        # draw current step
        if step_idx == 0:
            current = steps[0]
        else:
            current = steps[min(step_idx, len(steps)-1)]

        screen.fill(WHITE)
        draw_grid(screen, current['grid'], current['dirs'])
        pygame.display.flip()

        # handle quit
        for ev in pygame.event.get():
            if ev.type == pygame.QUIT:
                running = False

        clock.tick(60)

    pygame.quit()
    sys.exit()

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python tank_visualizer.py <game_flow.txt>")
        sys.exit(1)
    run_visualizer(sys.argv[1])
