sf::Vector2f toSFML(const sf::Vector2f& v, const WindowParameters& wParams)
{
    float newX = v.x*wParams.width / 2 + wParams.width / 2, newY = -(v.y * wParams.height / 2 - wParams.height / 2);

    return sf::Vector2f(newX, newY);
}

sf::Vector2f toNDC(const sf::Vector2f& v, const WindowParameters& wParams)
{
    float newX = 2 * v.x / wParams.width - 1;
    float newY = 2 * v.y / wParams.height - 1;
    newY *= -1;

    return sf::Vector2f(newX, newY);
}

float lenSq(const sf::Vector2f& v)
{
    return (v.x * v.x + v.y * v.y);
}

template<typename T>
T lerp(const T& a, const T& b, const float inter)
{
    return ((1 - inter) * a + inter * b);
}

float length(const sf::Vector2f& a)
{
    return sqrt(a.x * a.x + a.y * a.y);
}

float rand01()
{
    return ((float)rand() / INT_MAX);
}

float randMinusOneToOne()
{
    return (2.0f * rand01() - 1);
}

sf::Vector2f randomVec2f()
{
    float X = randMinusOneToOne();
    float Y = randMinusOneToOne();

    sf::Vector2f result(X, Y);

    result = result / length(result);
    
    return result;
}
