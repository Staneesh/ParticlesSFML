//TODO(Stanisz): Epsilon distance checking 
//TODO(Stanisz): Window scaling, resizing
//TODO(Stanisz): Vector direction changing GUI with text

#include <SFML/Graphics.hpp>
#include <iostream>
#include <cmath>
#include <random>
#include <climits>
#include <cassert>

#define COUT(x) std::cout<<#x<<": "<<x<<std::endl;

#include "main.h"

#include "math.cpp"

void emit(Particle* particles, Emitter& e, const WorldConstants& wConstants,
		const RendererParameters& rParams)
{
    Particle newParticle;

    newParticle.position = e.position;
    newParticle.velocity = e.speed *
        lerp<sf::Vector2f>(e.direction, randomVec2f(), e.randomnessInDirection);

    newParticle.mass = e.mass;
    newParticle.acceleration = newParticle.mass * sf::Vector2f(0, wConstants.gravity);
    newParticle.lifeSpan = e.lifeSpan;
    newParticle.bounceFactor = e.bounceFactor;
    assert(e.randomnessInSize <= 1.0f && e.randomnessInSize >= 0.0f);

    newParticle.size = (unsigned int)(rand01() * (1.0f - e.randomnessInSize) * rParams.maxParticleSize) + 4;
    newParticle.transparency = (unsigned char)(rand01() * 255.0f * (1.0f - e.randomnessInTransparency)); 

    for (int i =0; i < rParams.maxNumParticles; ++i)
    {
        auto& curPart = particles[i];

        if (curPart.lifeSpan <= 0)
        {
            curPart = newParticle;
            break;
        }
    }
}
void update(float dt, Particle* particles, Emitter* emitters,
		const RendererParameters& rParams, const WorldConstants& wConstants)
{   
    for (int i = 0; i < rParams.maxNumEmitters; ++i)
    {
        auto& e = emitters[i];

        
        e.lastTimeEmitted += dt;
    
        if (e.lastTimeEmitted > e.delay)
        {
            emit(particles, e, wConstants, rParams);
            e.lastTimeEmitted = 0;
        }
    }
    
    for (int i = 0; i < rParams.maxNumParticles; ++i)
    {
        auto& curPart = particles[i];

        bool bounced = false;

        curPart.velocity += dt * curPart.acceleration; // 0.5f * at^2

        auto nextPos = curPart.position + dt * curPart.velocity;

        if (nextPos.y < -1.0f || nextPos.y > 1.0f)
        {
            curPart.velocity.y = -curPart.velocity.y;
            bounced = true;
        }
        if (nextPos.x < -1.0f || nextPos.x > 1.0f)
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


int main()
{
	WindowParameters wParams;
	wParams.width = 1000;
	wParams.height = 1000;

	WorldConstants wConstants;
	wConstants.gravity = -0.01f;

	RendererParameters rParams;
	rParams.maxNumParticles = 10000;
	rParams.maxNumEmitters = 5;
	rParams.maxParticleSize = 10;

    sf::RenderWindow window(sf::VideoMode(wParams.width, wParams.height), "Particles!");
    sf::Clock clock;
    
    float elapsed = 0.0f;

    Particle* particles = (Particle*)malloc(sizeof(Particle) * rParams.maxNumParticles);

    Emitter* emitters = (Emitter*)malloc(sizeof(Emitter) * rParams.maxNumEmitters);

    srand(13333);
    
    for (int i = 0; i < rParams.maxNumEmitters; ++i)
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
        sf::Vector2f mousePos = toNDC((sf::Vector2f)sf::Mouse::getPosition(window), wParams);

        //std::cout<<mousePos.x<<' ' <<mousePos.y<<std::endl;
        //std::cout<<emitter.position.x<<' ' <<emitter.position.y<<std::endl;

        int whichEmitterPicked = -1;
        float minDist = 100;

            
        for (int i = 0; i < rParams.maxNumEmitters; ++i)
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
        if (minDist < 2.0f * emitters[whichEmitterPicked].radius / wParams.width)
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

        update(dt, particles, emitters, rParams, wConstants);
        window.clear();

        
        sf::CircleShape visible;
        
        visible.setRadius(500);
        visible.setOrigin({500, 500});
        visible.setPosition(toSFML({0, 0}, wParams));
        visible.setFillColor({0, 50, 0});
        window.draw(visible);

        visible.setOrigin({4, 4});
        
        for (int i = 0; i < rParams.maxNumParticles; ++i)
        {
            auto& curPart = particles[i];

            visible.setFillColor({0, 255, 255, curPart.transparency});

            visible.setRadius(curPart.size);
            if (curPart.lifeSpan > 0)
            {
                visible.setPosition(toSFML(curPart.position, wParams));
                window.draw(visible);
            }
        }

        for (int i = 0; i < rParams.maxNumEmitters; ++i)
        {
            auto& emitter = emitters[i];
            
            visible.setFillColor({125, 0, 0});
            if (emitter.hovering)
            {
                visible.setFillColor({255, 0, 0});
            }
            visible.setRadius(emitter.radius);
            visible.setOrigin({(float)emitter.radius, (float)emitter.radius});
            visible.setPosition(toSFML(emitter.position, wParams));
            
            window.draw(visible);
        }

        window.display();
    }

    free(particles);
    free(emitters);
    
    return 0;
}
