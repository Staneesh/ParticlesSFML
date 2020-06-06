struct WindowParameters
{
	unsigned width;
	unsigned height;
};

struct WorldConstants
{
	//NOTE(Stanisz): friction?
	float gravity;
};

struct RendererParameters
{
	int maxNumParticles;
	int maxNumEmitters;
	unsigned maxParticleSize;
	int currentNumEmitters;
};


struct Particle
{
    sf::Vector2f position;
    sf::Vector2f velocity;
    sf::Vector2f acceleration;    

    float mass;
    float lifeSpan;
    float bounceFactor;
    unsigned char transparency;
    float size;
};

struct Emitter
{
    sf::Vector2f direction;
    sf::Vector2f position;
    float speed;
    float delay;
    float lastTimeEmitted;
    float randomnessInDirection;
    int radius; // in pixels
    float lifeSpan;
    float mass;
    bool hovering;
    float bounceFactor;
    float randomnessInSize;
    float randomnessInTransparency;
};

struct EmitterButton
{
	sf::Vector2f position;
	sf::Vector2f size;
	sf::Color color;
};
