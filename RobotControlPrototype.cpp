#include <SFML/Graphics.hpp>
#include <SFML/Network.hpp>
#include <sstream>
#include <iostream>
#include <cmath>

void coordUpdate(float coordinates[], float angle, float distance)
{
    float newX, newY;

    newX = distance*cos(angle);
    newY = distance*sin(angle);

    coordinates[0] += newX;
    coordinates[1] += newY;
}

float distanceCalc(float oldX, float oldY, float newX, float newY)
{
    float x = newX - oldX;
    float y = newY- oldY;
    float s = sqrt(pow(x, 2) + pow(y, 2));
    return s;
}

float angleCalc(float oldX, float oldY, float newX, float newY)
{
    float alpha = atan2(oldY - newY, oldX - newX) + M_PI;
    return alpha;
}


float noNegativeAngle(float angle)
{
    if(angle<0)
    {
        angle += 360;
        return angle;
    }
    else if(angle>360)
    {
        angle -=360;
        return angle;
    }
    else
    {
    return angle;
    }
}

float rad2deg(float angle)
{
    angle = angle*180/M_PI;
    return noNegativeAngle(angle);
}

float deg2rad(float angle)
{
    noNegativeAngle(angle);
    angle = angle*(M_PI/180);
    return angle;
}

int anglediffCheck(float angle, float instruction)
{
    float difference = angle - instruction;
    if(noNegativeAngle(difference) <= 180)
    {
        return 1;
    }
    else if(noNegativeAngle(difference) > 180)
    {
        return 0;
    }
}


//127.0.0.1
//192.168.1.3

int main()
{
    sf::TcpSocket socket;
	sf::Socket::Status status = socket.connect("192.168.2.2", 9090);
	if (status != sf::Socket::Done)
	{
		std::cout << "Connection Failed" << std::endl;
        return 1;
	}

    sf::RenderWindow window(sf::VideoMode(1200, 720), "Robot Control");
    sf::VertexArray lines(sf::LineStrip);

    const float radius = 10.f;
    sf::CircleShape shape(radius);
    shape.setOrigin(radius, radius);

    std::vector<float> X, Y, angleInstructions, displacementInstructions;

    float coordinates[2] = {360, 360};
    float angle = 0;
    float distance = 0;
    float tempDistance = 0;
    char buf[128];
    int run = 0;
    int state = 0;
    int cycle = 0;

    while(window.isOpen())
    {
        std::stringstream ss;

        sf::Event event;
        while(window.pollEvent(event))
        {
            if(event.type == sf::Event::Closed)
            {
                window.close();
            }
            if (event.type == sf::Event::MouseButtonPressed)
            {
                if (event.mouseButton.button == sf::Mouse::Left)
                {
                    if(X.size() == 0) {     // setting starting point as the location of the robot
                        sf::Vector2i coord(coordinates[0], coordinates[1]);
                        sf::Vector2f normalizedCoord = window.mapPixelToCoords(coord);
                        sf::Vertex vx(normalizedCoord);
                        lines.append(vx);

                        X.push_back(coordinates[0]);
                        Y.push_back(coordinates[1]);
                    }

                    // SAVING POINTS
                    sf::Vector2i coord(event.mouseButton.x, event.mouseButton.y);  
                    sf::Vector2f normalizedCoord = window.mapPixelToCoords(coord);
                    sf::Vertex vx(normalizedCoord);
                    lines.append(vx);

                    X.push_back(normalizedCoord.x);
                    Y.push_back(normalizedCoord.y);

                    auto X_last = X.back();
                    auto Y_last = Y.back();
                    auto X_prev = X[X.size()-2];
                    auto Y_prev = Y[Y.size()-2];

                    std::cout << "mouse x: " << X_last << std::endl;
                    std::cout << "mouse y: " << Y_last << std::endl;

                    std::cout << "displacement:" << distanceCalc(X_prev,Y_prev,X_last,Y_last) << std::endl;
                    std::cout << "angle:" << rad2deg(angleCalc(X_prev,Y_prev,X_last,Y_last)) << std::endl;
                    
                    // TRANSLATING VECTORS OF POINTS TO THE

                    displacementInstructions.push_back(distanceCalc(X_prev,Y_prev,X_last,Y_last));
                    angleInstructions.push_back(angleCalc(X_prev,Y_prev,X_last,Y_last));                     
                }
                }
            if(sf::Keyboard::isKeyPressed(sf::Keyboard::X))
            {
                lines.clear();
                X.clear();
                Y.clear();
                displacementInstructions.clear();
                angleInstructions.clear();
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        {
            distance = 1;
            coordUpdate(coordinates, angle, distance);
            
            std::cout << "X = " << coordinates[0] << std::endl;
            std::cout << "Y = " << coordinates[1] << std::endl;

            ss << 1 << std::endl;

                memset(buf, 0, sizeof(buf));
                auto s = ss.str();
                strcpy(buf, s.c_str());

                if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
                {
                    std::cout << "sending failed" << std::endl;
                }
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        {
            
            //angle += M_PI/180;

            //std::cout << "The angle:" << rad2deg(angle) << std::endl;

            ss << 3 << std::endl;

            memset(buf, 0, sizeof(buf));
		    auto s = ss.str();
		    strcpy(buf, s.c_str());

            if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
            {
			    std::cout << "sending failed" << std::endl;
		    }

        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        {
            //angle -= M_PI/180;

            //std::cout << "The angle:" << rad2deg(angle) << std::endl;

            ss << 4 << std::endl;

            memset(buf, 0, sizeof(buf));
		    auto s = ss.str();
		    strcpy(buf, s.c_str());

            if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
            {
			    std::cout << "sending failed" << std::endl;
		    }
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        {
            distance = -1;
            coordUpdate(coordinates, angle, distance);
            
            std::cout << "X = " << coordinates[0] << std::endl;
            std::cout << "Y = " << coordinates[1] << std::endl;

            ss << 2 << std::endl;

            memset(buf, 0, sizeof(buf));
		    auto s = ss.str();
		    strcpy(buf, s.c_str());

            if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
            {
			    std::cout << "sending failed" << std::endl;
		    }
        }
        else if(sf::Keyboard::isKeyPressed(sf::Keyboard::R))
        {
            run = 1;
            state = 1;
        }
        else
        {
            if (run == 0)
            {
                ss << 5 << std::endl;

                memset(buf, 0, sizeof(buf));
                auto s = ss.str();
                strcpy(buf, s.c_str());

                if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
                {
                    std::cout << "sending failed" << std::endl;
                }
            }

        }
        if(run == 1)
        {
            switch(state)
            {
                case 1: // Rotating to the angle from the instructions
                    if(anglediffCheck(angle, angleInstructions[cycle]) == 0)
                    {
                        angle += M_PI/360;
                        std::cout << "The angle:" << rad2deg(angle) << std::endl;
                        ss << 3 << std::endl;

                        memset(buf, 0, sizeof(buf));
                        auto s = ss.str();
                        strcpy(buf, s.c_str());

                        if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
                        {
                            std::cout << "sending failed" << std::endl;
                        }

                        if(angle > angleInstructions[cycle])
                        {
                            state = 2;
                        }
                    }
                    else if(anglediffCheck(angle, angleInstructions[cycle]) == 1)
                    {
                        angle -= M_PI/360;
                        std::cout << "The angle:" << rad2deg(angle) << std::endl;

                        ss << 4 << std::endl;

                        memset(buf, 0, sizeof(buf));
                        auto s = ss.str();
                        strcpy(buf, s.c_str());

                        if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
                        {
                            std::cout << "sending failed" << std::endl;
                        }

                        if(angle < angleInstructions[cycle])
                        {
                            state = 2;
                        }
                    }
                break;
                case 2: // Going to the next point
                    tempDistance += 1;
                    distance = 1;
                    std::cout << "Distance = " << tempDistance << std::endl;

                    ss << 1 << std::endl;

                    memset(buf, 0, sizeof(buf));
                    auto s = ss.str();
                    strcpy(buf, s.c_str());

                    if(socket.send(buf, sizeof(buf)) != sf::Socket::Done) 
                    {
                        std::cout << "sending failed" << std::endl;
                    }

                    coordUpdate(coordinates, angle, distance);

                    if(tempDistance>displacementInstructions[cycle])
                    {
                        state = 1;
                        cycle += 1;
                        tempDistance = 0;
                    }

                break;
            }
            // Ending instruction
            if(cycle >= angleInstructions.size())  
            {
                cycle = 0;
                run = 0;
            }

        }
        std::size_t received;
        if(socket.receive(buf, sizeof(buf), received) != sf::Socket::Done) 
        {
            std::cout << "error" << std::endl;
        }

        std::stringstream anglestream;
        anglestream.str(buf);
        anglestream >> angle;
        angle = deg2rad(angle);

        shape.setPosition(coordinates[0], coordinates[1]);
        window.clear();
        window.draw(lines);
        window.draw(shape);
        window.display();

        sf::sleep(sf::milliseconds(10));
    }

    return 0;
}

/*
IMU ERROR
-35
-35
-35
-36
-35
-35
-35
-36
-37
-36
-42
-42
-42
-42
-41
-41
-40
-42
-42
-41
-39
-39
-38
-39
-34
-35
-35
-42
-43
-41
-45
-45
-45
-44
-38
-39
-38
-42
-43
-42
-43
-39
-39
-40
-45
-45
-46
-46
-45
-45
-44
-36
-37
-36
-37
-36
-36
-37
-31
-31
-32
-40
-40
-40
-40
-38
-37
-38
-39
-39
-40
-39
-34
-34
-34
-37
-38
-38
-38
-38
-37
-37
-37
-38
-38
-36
-36
-36
-39
-39
-41
-40
-40
-36
-36
-37
-37
-35
-35
-37
-38
-37
-37
-38
-34
-34
-37
-37
-40
-40
-42
-41
-42
-38
-39
-39
-39
-39
-40
-38
-40
-40
-40
-39
-41
-40
-35
-36
-36
-39
-39
-40
-41
-40
-41
-37
-37
-37
-41
-41
-40
-41
-41
-41
-41
-38
-39
-39
-35
-35
-36
-36
-36
-36
-37
-37
-36
-35
-35
-35
-41
-40
-41
-42
-39
-39
-39
-37
-37
-37
-37
-37
-37
-35
-35
-35
-36
-33
-33
-32
-32
-32
-33
-41
-41
-41
-41
-39
-39
-40
-37
-38
-38
-38
-37
-37
-37
-36
-37
-36
-37
-38
-38
-38
-42
-42
-43
-37
-35
-34
-34
-34
-34
-34
-40
-41
-40
-42
-37
-37
-37
-37
-38
-38
-38
-38
-39
-38
-37
-37
-37
-39
-38
-38
-39
-43
-42
-42
-36
-37
-37
-37
-37
-36
-36
-37
-38
-37
-43
-42
-43
-42
-38
-39
-38
-37
-37
-35
-36
-39
-39
-38
-37
-37
-37
-37
-38
-38
-38
-38
-38
-38
-38
-38
-39
-38
-37
-37
-38
-36
-36
-37
-37
-36
-37
-37
-38
-38
-38
-39
-39
-39
-39
-40
-41
-40
-40
-40
-40
-40
-40
-40
-37
-38
-37
-35
-36
-35
-39
-39
-38
-37
-37
-38
-38
-35
-35
-35
-38
-38
-38
-33
-33
-33
-38
-38
-38
-38
-38
-38
-36
-36
-36
-34
-34
-36
-38
-38
-38
-36
-37
-38
-38
-39
-39
-39
-37
-37
-38
-41
-41
-41
-44
-43
-44
-44
-43
-44
-44
-34
-35
-35
-39
-37
-39
-37
-38
-37
-37
-36
-36
-36
-38
-38
-38
-31
-32
-31
-31
-42
-42
-41
-42
-34
-34
-34
-34
-34
-34
-35
-34
-34
-34
-36
-36
-36
-36
-33
-33
-33
-33
-38
-38
-38
-38
-36
-35
-35
-39
-39
-38
-39
-40
-39
-38
-38
-36
-35
-35
-35
-36
-35
-35
-36
-36
-36
-36
-38
-37
-37
-37
-37
-37
-37
-36
-35
-36
-35
-37
-37
-37
-38
-36
-36
-36
-36
-35
-35
-35
-39
-39
-39
-39
-39
-40
-38
-40
-40
-40
-41
-35
-35
-35
-35
-36
-36
-36
-36
-35
-36
-36
-38
-38
-37
-37
-44
-44
-43
-44
-37
-37
-38
-38
-37
-37
-37
-38
-37
-37
-38
-38
-39
-39
-40
-40
-40
-41
-43
-44
-44
-44
-34
-33
-33
-40
-40
-41
-41
-40
-40
-41
-37
-38
-37
-38
-37
-38
-38
-42
-41
-41
-41
-41
-41
-41
-38
-38
-37
-37
-33
-33
-33
-38
-38
-38
-38
-34
-35
-34
-36
-36
-36
-37
-36
-37
-37
-37
-37
-37
-37
-37
-38
-37
-39
-39
-38
-37
-38
-37
-37
-35
-34
-34
-40
-39
-40
-39
-31
-32
-32
-34
-33
-33
-33
-36
-36
-36
-37
-36
-37
-36
-38
-38
-38
-38
-37
-37
-37
-42
-42
-42
-42
-38
-39
-38
-42
-42
-42
-42
-39
-40
-39
-39
-38
-38
-38
-38
-36
-36
-36
-37
-36
-36
-36
-38
-39
-39
-38
-39
-39
-39
-37
-38
-37
-38
-38
-38
-38
-38
-37
-37
-36
-37
-43
-43
-43
-40
-40
-40
-40
-39
-39
-38
-38
-39
-39
-39
-40
-40
-40
-41
-33
-32
-33
-33
-39
-39
-39
-39
-39
-38
-39
-39
-39
-39
-40
-39
-39
-38
-39
-38
-38
-38
-38
-38
-38
-37
-38
-38
-37
-38
-37
-38
-38
-34
-34
-34
-35
-37
-37
-37
-38
-40
-38
-39
-40
-37
-37
-37
-39
-38
-38
-38
-41
-41
-41
-41
-41
-41
-40
-41
-42
-41
-42
-42
-42
-41
-41
-30
-30
-30
-40
-40
-40
-39
-38
-39
-38
-41
-40
-40
-39
-38
-37
-35
-36
-35
-35
-35
-35
-38
-39
-38
-44
-43
-43
-34
-33
-33
-38
-38
-38
-35
-36
-35
-36
-40
-39
-39
-41
-42
-42
-42
-42
-41
-41
-42
-41
-38
-39
-38
-37
-38
-38
-38
-37
-38
-38
-37
-36
-37
-37
-38
-37
-39
-39
-39
-36
-36
-36
-36
-37
-36
-36
-39
-38
-39
-35
-35
-34
-33
-40
-40
-40
-38
-37
-37
-32
-31
-32
-31
-40
-41
-41
-36
-35
-36
-35
-39
-39
-39
-40
-39
-40
-40
-40
-41
-40
-34
-34
-34
-45
-45
-46
-45
-33
-33
-34
-40
-40
-40
-41
-38
-38
-38
-43
-42
-42
-43
-43
-43
-43
-43
-44
-38
-39
-44
-44
-45
-39
-39
-39
-39
-39
-38
-38
-38
-38
-38
-39
-42
-42
-41
-41
-41
-42
-42
-40
-40
-40
-41
-41
-42
-42
-42
-37
-37
-32
-32
-32
-32
-32
-34
-35
-36
-37
-36
-37
-36
-35
-34
-34
-34
-34
-35
-35
-40
-38
-37
-37
-37
-37
-39
-38
-39
-41
-41
-43
-42
-42
-42
-39
-39
-40
-39
-42
-42
-41
-35
-35
-36
-35
-33
-33
-32
-32
-32
-33
-39
-40
-39
-39
-43
-43
-43
-39
-39
-39
-39
-38
-38
-38
-37
-38
-38
-38
-34
-34
-34
-40
-38
-39
-38
-36
-37
-36
-36
-35
-36
-36
-40
-40
-40
-36
-35
-36
-36
-37
-36
-38
-39
-39
-40
-40
-39
-38
-39
-38
-33
-34
-33
-34
-36
-35
-36
-36
-37
-36
-36
-36
-36
-36
-36
-38
-38
-38
-37
-37
-37
-37
-42
-42
-42
-42
-41
-41
-41
-41
-42
-41
-41
-40
-40
-40
-40
-38
-39
-38
-38
-37
-38
-39
-35
-36
-36
-36
-40
-40
-41
-41
-36
-36
-36
-36
-41
-40
-40
-36
-36
-35
-36
-38
-38
-37
-38
-37
-38
-37
-37
-38
-38
-38
-39
-38
-38
-37
-36
-36
-36
-35
-41
-41
-42
-37
-36
-37
-36
-40
-39
-40
-39
-37
-36
-37
-37
-34
-34
-33
-36
-37
-37
-36
-40
-39
-40
-41
-40
-40
-40
-41
-41
-41
-41
-35
-36
-35
-36
-37
-36
-37
-39
-35
-34
-35
-32
-33
-33
-32
-34
-34
-34
-37
-37
-36
-37
-37
-36
-37
-36
-35
-36
-35
-35
-40
-40
-41
-41
-37
-37
-37
-37
-38
-38
-39
-38
-38
-43
-42
-43
-38
-38
-38
-38
-38
-38
-40
-40
-40
-40
-42
-41
-42
-40
-39
-40
-38
-38
-38
-33
-33
-33
-33
-38
-38
-38
-36
-36
-36
-42
-41
-41
-41
-40
-41
-35
-36
-36
-35
-37
-37
-37
-39
-40
-39
-37
-37
-37
-36
-39
-39
-38
-39
-40
-39
-40
-36
-37
-36
-39
-39
-38
-39
-38
-38
-38
-41
-41
-41
-35
-36
-35
-36
-34
-33
-34
-41
-40
-41
-41
-38
-38
-39
-33
-33
-33
-33
-39
-38
-39
-39
-40
-39
-39
-39
-39
-40
-35
-35
-35
-37
-37
-38
-37
-35
-35
-34
-36
-36
-37
-36
-36
-35
-35
-34
-34
-34
-34
-37
-37
-37
-37
-37
-36
-36
-40
-40
-40
-38
-38
-39
-34
-35
-35
-40
-40
-40
-35
-36
-36
-39
-40
-39
-37
-37
-37
-40
-39
-40
-39
-39
-39
-41
-41
-41
-40
-40
-40
-40
-41
-40
-41
-40
-40
-40
-40
-40
-40
-39
-39
-39
-39
-39
-39
-39
-39
-40
-39
-39
-39
-38
-36
-35
-35
-40
-40
-40
-38
-37
-38
-37
-38
-38
-37
-37
-37
-42
-42
-42
-37
-36
-36
-36
-36
-37
-36
-41
-41
-41
-44
-44
-45
-35
-36
-35
-38
-37
-38
-40
-41
-40
-41
-38
-38
-38
-38
-37
-38
-40
-40
-41
-41
-41
-40
-42
-41
-42
-38
-39
-39
-39
-36
-37
-37
-40
-40
-40
-40
-37
-37
-37
-42
-42
-42
-38
-39
-38
-39
-38
-39
-34
-34
-34
-36
-36
-37
-37
-36
-36
-37
-37
-37
-39
-38
-36
-36
-36
-36
-36
-36
-36
-39
-38
-40
-40
-41
-40
-41
-38
-38
-36
-35
-35
-36
-36
-39
-39
-39
-39
-34
-33
-36
-35
-35
-39
-39
-43
-43
-38
-37
-38
-37
-37
-40
-40
-39
-40
-39
-39
-42
-38
-37
-39
-39
-40
-39
-40
-35
-36
-35
-35
-41
-41
-41
-41
-36
-39
-40
-39
-39
-39
-40
-32
-33
-33
-40
-40
-39
-39
-34
-35
-34
-34
-37
-37
-38
-38
-32
-32
-33
-34
-34
-39
-39
-39
-38
-38
-38
-38
-37
-37
-37
-37
-37
-37
-37
-37
-37
-37
-40
-41
-41
-40
-40
-40
-40
-39
-39
-39
-39
-34
-35
-35
-36
-36
-36
-35
-33
-34
-32
-39
-40
-39
-39
-39
-39
-39
-39
-39
-39
-39
-38
-38
-37
-37
-37
-36
-37
-42
-42
-43
-42
-40
-40
-40
-42
-37
-37
-37
-36
-37
-37
-36
-40
-41
-40
-41
-41
-40
-41
-36
-36
-36
-36
-37
-37
-37
-37
-37
-37
-37
-40
-40
-40
-40
-37
-37
-37
-36
-40
-40
-41
-42
-42
-41
-41
-28
-29
-29
-30
-29
-29
-29
-36
-35
-35
-35
-34
-33
-34
-33
-39
-39
-40
-42
-42
-43
-42
-38
-38
-38
-38
-36
-36
-36
-40
-39
-40
-39
-40
-39
-39
-39
-39
-40
-39
-38
-37
-36
-37
-40
-40
-40
-40
-40
-40
-40
-40
-39
-39
-39
-35
-36
-36
-36
-35
-36
-35
-36
-39
-39
-40
-40
-39
-40
-39
-33
-34
-33
-35
-35
-35
-35
-35
-35
-35
-32
-33
-33
-32
-35
-35
-36
-37
-38
-38
-37
-42
-43
-43
-37
-38
-38
-37
-37
-37
-37
-41
-41
-41
-36
-36
-36
-35
-35
-35
-36
-38
-38
-40
-40
-39
-40
-40
-40
-41
-39
-39
-38
-38
-39
-39
-38
-39
-38
-38
-38
-37
-40
-40
-41
-41
-38
-38
-39
-36
-37
-36
-35
-34
-34
-34
-35
-35
-38
-40
-39
-40
-40
-40
-39
-41
-41
-41
-36
-36
-37
-41
-42
-42
-41
-39
-39
-39
-37
-37
-38
-37
-36
-36
-36
-38
-38
-39
-39
-37
-37
-37
-37
-37
-37
-34
-35
-35
-35
-43
-43
-43
-37
-37
-38
-36
-38
-38
-38
-35
-35
-34
-35
-39
-39
-39
-32
-33
-32
-33
-33
-33
-32
-33
-32
-32
-38
-37
-37
-41
-41
-41
-41
-42
-42
-42
-39
-38
-38
-38
-37
-37
-36
-40
-40
-40
-40
-35
-35
-36
-36
-36
-35
-38
-37
-38
-37
-37
-37
-36
-35
-34
-35
-35
-38
-38
-37
-40
-40
-40
-40
-43
-43
-43
-40
-39
-39
-40
-32
-32
-32
-32
-32
-32
-35
-35
-35
-35
-36
-36
-31
-31
-31
-38
-37
-38
-33
-33
-33
-33
-35
-34
-35
-39
-39
-39
-39
-40
-39
-39
-39
-40
-37
-37
-37
-42
-41
-41
-41
-42
-41
-38
-38
-39
-37
-36
-35
-36
-36
-36
-35
-36
-36
-36
-36
*/