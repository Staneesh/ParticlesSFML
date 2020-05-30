//TODO(Stanisz): Epsilon distance checking 
//TODO(Stanisz): Window scaling, resizing
//TODO(Stanisz): Vector direction changing GUI with text

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <random>
#include <climits>

#define COUT(x) std::cout<<#x<<": "<<x<<std::endl;

unsigned WINDOW_HEIGHT = 1000;
unsigned WINDOW_WIDTH = 1500;

const float GRAVITY = 0.0;

const int MAX_NUM_PARTICLES = 10000;
const int MAX_NUM_EMITTERS = 5;

const float EPSILON = 0.005f;

const unsigned MAX_PARTICLE_SIZE = 10;
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

sf::Vector2f toSFML(const sf::Vector2f& v)
{
    float newX = v.x*WINDOW_WIDTH / 2 + WINDOW_WIDTH / 2, newY = -(v.y * WINDOW_HEIGHT / 2 - WINDOW_HEIGHT / 2);

    return sf::Vector2f(newX, newY);
}

sf::Vector2f toNDC(const sf::Vector2f& v)
{
    float newX = 2 * v.x / WINDOW_WIDTH - 1;
    float newY = 2 * v.y / WINDOW_HEIGHT - 1;
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

void emit(Particle* particles, Emitter& e)
{
    Particle newParticle;

    newParticle.position = e.position;
    newParticle.velocity = e.speed *
        lerp<sf::Vector2f>(e.direction, randomVec2f(), e.randomnessInDirection);

    newParticle.mass = e.mass;
    newParticle.acceleration = newParticle.mass * sf::Vector2f(0, GRAVITY);
    newParticle.lifeSpan = e.lifeSpan;
    newParticle.bounceFactor = e.bounceFactor;
    newParticle.size = (unsigned int)(rand01() * (1.0f - e.randomnessInSize) * MAX_PARTICLE_SIZE) + 4;
    newParticle.transparency = (unsigned char)(rand01() * 255.0f * (1.0f - e.randomnessInTransparency)); 

    for (int i =0; i < MAX_NUM_PARTICLES; ++i)
    {
        auto& curPart = particles[i];

        if (curPart.lifeSpan <= 0)
        {
            curPart = newParticle;
            break;
        }
    }
}

float abs(float a)
{
    return a < 0.0f ? -a : a;
}

bool isWithinEpsilonDistance(float f, float from)
{
    return (abs(f - from) < EPSILON);
}

void update(float dt, Particle* particles, Emitter* emitters)
{   
    for (int i = 0; i < MAX_NUM_EMITTERS; ++i)
    {
        auto& e = emitters[i];

        
        e.lastTimeEmitted += dt;
    
        if (e.lastTimeEmitted > e.delay)
        {
            emit(particles, e);
            e.lastTimeEmitted = 0;
        }
    }
    
    for (int i = 0; i < MAX_NUM_PARTICLES; ++i)
    {
        auto& curPart = particles[i];

        bool bounced = false;

        curPart.velocity += dt * curPart.acceleration; // 0.5f * at^2

        auto nextPos = curPart.position + dt * curPart.velocity;

        if (isWithinEpsilonDistance(nextPos.y, -1.0f)
            || isWithinEpsilonDistance(nextPos.y, 1.0f))
        {
            curPart.velocity.y = -curPart.velocity.y;
            bounced = true;
        }
        if (isWithinEpsilonDistance(nextPos.x, -1.0f)
            || isWithinEpsilonDistance(nextPos.x, 1.0f))
        {
            curPart.velocity.x = -curPart.velocity.x;
            bounced = true;
        }

        
        if (bounced)
        {
            curPart.velocity = lerp<sf::Vector2f> (curPart.velocity,
                                                   randomVec2f(), 1.0f - curPart.bounceFactor);
            if (lenSq(curPart.velocity) < 0.01)
            {
                curPart.velocity = {};
                curPart.acceleration = {};
            }
            curPart.velocity = curPart.velocity * curPart.bounceFactor;
        }
        

        
        curPart.position += dt * curPart.velocity;

        curPart.lifeSpan -= dt;        
    }

}


int main(int argc, char** argv)
{
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Particles!");
    sf::Clock clock;
    
    float elapsed = 0.0f;

    Particle* particles = (Particle*)malloc(sizeof(Particle) * MAX_NUM_PARTICLES);

    Emitter* emitters = (Emitter*)malloc(sizeof(Emitter) * MAX_NUM_EMITTERS);

    srand(13333);
    
    for (int i = 0; i < MAX_NUM_EMITTERS; ++i)
    {
        auto& emitter = emitters[i];

        
        emitter.position = sf::Vector2f(randMinusOneToOne(), randMinusOneToOne());
        
        emitter.direction = sf::Vector2f(0.0f, 1.0f);
        emitter.speed = 1;
        emitter.delay = 0.001f;
        emitter.lastTimeEmitted = 0;
        emitter.randomnessInDirection = 0.1f;
        emitter.radius = 20;
        emitter.hovering = 0;
        emitter.lifeSpan = 3;
        emitter.mass = 100;
        emitter.bounceFactor = 0.1f;
        emitter.randomnessInSize = 0.1f;
        emitter.randomnessInTransparency = 0.3f;
    }

    bool grabbing = false;
    
    while(window.isOpen())
    {
        double dt = clock.restart().asSeconds();
        elapsed += dt;
                
        sf::Event event;

        while(window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
                window.close();
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
            {
                window.close();
            }
        }


        bool clicked = sf::Mouse::isButtonPressed(sf::Mouse::Left);
        sf::Vector2f mousePos = toNDC((sf::Vector2f)sf::Mouse::getPosition(window));

        //std::cout<<mousePos.x<<' ' <<mousePos.y<<std::endl;
        //std::cout<<emitter.position.x<<' ' <<emitter.position.y<<std::endl;

        int whichEmitterPicked = -1;
        float minDist = 100;

            
        for (int i = 0; i < MAX_NUM_EMITTERS; ++i)
        {
            auto& emitter = emitters[i];

            emitter.hovering = 0;
            
            float len = length(mousePos - emitter.position);

            if (len < minDist)
            {
                minDist = len;
                whichEmitterPicked = i;

            }
        }

        //TODO(Stanisz): 1D variable scaling to NDC.
        if (minDist < 2.0f * emitters[whichEmitterPicked].radius / WINDOW_WIDTH)
        {
            emitters[whichEmitterPicked].hovering = 1;

            if (clicked)
            {
                grabbing = 1;
            }
            else
            {
                grabbing = 0;
            }
        }
        else
        {
            emitters[whichEmitterPicked].hovering = 0;
        }
        
        if (grabbing)
        {
            emitters[whichEmitterPicked].position = mousePos;
        }

        update(dt, particles, emitters);
        window.clear();

        
        sf::CircleShape visible;
        
        visible.setRadius(500);
        visible.setOrigin({500, 500});
        visible.setPosition(toSFML({0, 0}));
        visible.setFillColor({0, 50, 0});
        window.draw(visible);

        visible.setOrigin({4, 4});
        
        for (int i = 0; i < MAX_NUM_PARTICLES; ++i)
        {
            auto& curPart = particles[i];

            visible.setFillColor({0, 255, 255, curPart.transparency});

            visible.setRadius(curPart.size);
            if (curPart.lifeSpan > 0)
            {
                visible.setPosition(toSFML(curPart.position));
                window.draw(visible);
            }
        }

        for (int i = 0; i < MAX_NUM_EMITTERS; ++i)
        {
            auto& emitter = emitters[i];
            
            visible.setFillColor({125, 0, 0});
            if (emitter.hovering)
            {
                visible.setFillColor({255, 0, 0});
            }
            visible.setRadius(emitter.radius);
            visible.setOrigin({emitter.radius, emitter.radius});
            visible.setPosition(toSFML(emitter.position));
            
            window.draw(visible);
        }

        window.display();
    }

    free(particles);
    free(emitters);
    
    return 0;
}
