point: struct {
    x: i32;
    y: i32;
}

(self: >const point).len_squared: () -> i32
{
    return self.x * self.x + self.y * self.y;
}

main: () -> int
{
    p: point = { 10, 10 };

    return p.len_squared();
}
