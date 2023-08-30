#pragma once

/*
    Collision enum and struct definitions

*/
enum class CollisionType
{
    None,
    Top,
    Middle,
    Bottom,
    Left,
    Right,
};

struct Contact
{
    CollisionType type;
    float penetration;
};
