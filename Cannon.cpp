    #include "Cannon.h"

    // Getter
    Direction Cannon::getCurrentDirection() const
    {
        return current_direction;
    }

    // Rotate 45° left (1/8 turn)
    void Cannon::rotate45Degrees(int clockwise_flag) // clockwise_flag = 1 for right, -1 for left
    {
        if (clockwise_flag == 1)
        {
            //rotate45Right();
            current_direction = static_cast<Direction>((static_cast<int>(current_direction) + 1) % 8);

        } else if (clockwise_flag == -1)
        {
            //rotate45Left();
            current_direction = static_cast<Direction>((static_cast<int>(current_direction) - 1 + 8) % 8);
        }
    }

    // Rotate 90° left (1/4 = 2/8 turn)
    void Cannon::rotate90Degrees(int clockwise_flag) // clockwise_flag = 1 for right, -1 for left
    {
        if (clockwise_flag == 1)
        {
            //rotate90Right();
            current_direction = static_cast<Direction>((static_cast<int>(current_direction) + 2) % 8);

        } else if (clockwise_flag == -1)
        {
            //rotate90Left();
            current_direction = static_cast<Direction>((static_cast<int>(current_direction) - 2 + 8) % 8);
        }
    }
