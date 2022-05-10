#include <iostream>
#include <stdio.h>
#include <vector>
#include <algorithm>
#include <iterator>
#include <SFML/Graphics.hpp>

static int stageSizeY;
static int stageSizeX;

struct Casilla
{
    int id;
    int x, y;
    int g = 0;
    int h = 0;
    int f = 0;
    bool isWall = false;
    int PadreX = -1;
    int PadreY = -1;
    sf::RectangleShape shape;
    std::vector<Casilla*> Vecinos;
};

int calculateDistance(Casilla Source, Casilla Target)
{
    int _x = abs(Source.x - Target.x);
    int _y = abs(Source.y - Target.y);

    return _x + _y;
}

int calculateWeight(Casilla Source)
{
    int _f = Source.g + Source.h;

    return _f;
}

void updateNeighbourData(Casilla* currentNeighbour, Casilla* Current, Casilla* End)
{
    currentNeighbour->g = Current->g + 1;
    currentNeighbour->h = calculateDistance(*currentNeighbour, *End);
    currentNeighbour->f = calculateWeight(*currentNeighbour);

    currentNeighbour->PadreY = Current->y;
    currentNeighbour->PadreX = Current->x;
}

bool isInSet(std::vector<Casilla> Set, Casilla* currentNeighbour, Casilla* setNeighbourTemp)
{
    for(Casilla setNeighbour : Set)
    {
        if(currentNeighbour->id == setNeighbour.id)
        {
            setNeighbourTemp = &setNeighbour;
            return true;
        }
    }

    return false;
}

void MousePosTOMatrixPos(sf::Vector2i mousePosition, sf::Vector2f pixelSize, sf::Vector2f* mouseMatrix)
{
    mouseMatrix->x = int(mousePosition.x / pixelSize.x);
    mouseMatrix->y = int(mousePosition.y / pixelSize.y);
}

int pathfindAStar()
{
    Casilla *Start = nullptr;
    Casilla *End = nullptr;
    std::vector<Casilla> openSet;
    std::vector<Casilla> closeSet;
    int stageId = 0;

    bool construccionFin = false;
    bool InicioPuesto = false;
    bool FinPuesto = false;
    bool foundEnd = false;

    printf("Ingrese el tamanio del tablero Y;X : ");
    scanf("%d;%d", &stageSizeY, &stageSizeX);

    sf::Vector2f sizePixels((663/stageSizeX) - 1, (663/stageSizeY) - 1);

    Casilla Stage[stageSizeX][stageSizeY];

    for(int Fila = 0; Fila < stageSizeY; Fila++)
    {
        for(int Columna = 0; Columna < stageSizeX; Columna++)
        {
            Stage[Fila][Columna].id = stageId;
            Stage[Fila][Columna].y = Fila;
            Stage[Fila][Columna].x = Columna;
            Stage[Fila][Columna].shape.setSize(sizePixels);
            Stage[Fila][Columna].shape.setPosition(Columna * (663/stageSizeX), (Fila * (663/stageSizeY)));
            Stage[Fila][Columna].shape.setOutlineColor(sf::Color(128,128,128));
            Stage[Fila][Columna].shape.setOutlineThickness(1);

            stageId++;
        }
    }

        Casilla Current;

    sf::RenderWindow window(sf::VideoMode(663, 633), "AStar Visualizator");
    window.setFramerateLimit(10);
    sf::Vector2f MouseMatrix;
    while(window.isOpen())
    {
        sf::Event evento;
        while(window.pollEvent(evento))
        {
            switch(evento.type)
            {
                case sf::Event::Closed:
                    window.close();
                    break;

            default: break;
            }
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Left))
        {
            if(sf::Mouse::getPosition(window).y > 663 || sf::Mouse::getPosition(window).y < 0 || sf::Mouse::getPosition(window).x > 663 || sf::Mouse::getPosition(window).x < 0)
            {
                std::cout<<"Click Fuera del Tablero"<<std::endl;
            }
            else
            {
                MousePosTOMatrixPos(sf::Mouse::getPosition(window), sizePixels, &MouseMatrix);

                if(Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].isWall && !construccionFin)
                {
                    Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].isWall = false;
                    Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].shape.setFillColor(sf::Color::White);
                }
                else if (!Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].isWall && !construccionFin)
                {
                    Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].isWall = true;
                    Stage[int(MouseMatrix.y)][int(MouseMatrix.x)].shape.setFillColor(sf::Color::Black);
                }
            }
        }

        if(sf::Mouse::isButtonPressed(sf::Mouse::Right))
        {
            if(!InicioPuesto)
            {
                Start = &Stage[int(sf::Mouse::getPosition(window).y/sizePixels.y)][int(sf::Mouse::getPosition(window).x/sizePixels.x)];

                openSet.push_back(*Start);
                InicioPuesto = true;
            }
            else if (!FinPuesto)
            {
                End = &Stage[int(sf::Mouse::getPosition(window).y/sizePixels.y)][int(sf::Mouse::getPosition(window).x/sizePixels.x)];

                FinPuesto = true;
            }
        }

        if(sf::Keyboard::isKeyPressed(sf::Keyboard::Enter))
        {
            if(!construccionFin && InicioPuesto && FinPuesto)
            {
                construccionFin = true;
            }
        }

            if(!openSet.empty() && construccionFin && !foundEnd)
            {
                Casilla Current;
                auto minF = std::min_element(std::begin(openSet), std::end(openSet), [] (Casilla const& lhs, Casilla const& rhs) {return lhs.f < rhs.f;});
                auto positionF = std::distance(std::begin(openSet), minF);

                Current = openSet[positionF];

                for(int Fila = 0; Fila < stageSizeY; Fila++)
                {
                    for(int Columna = 0; Columna < stageSizeX; Columna++)
                    {
                        if(Stage[Fila][Columna].id == openSet[positionF].id)
                        {
                            Stage[Fila][Columna].shape.setFillColor(sf::Color::Cyan);
                        }
                    }
                }

                openSet.erase(openSet.begin() + positionF);

                closeSet.push_back(Current);

                for(int Fila = 0; Fila < stageSizeY; Fila++)
                {
                    for(int Columna = 0; Columna < stageSizeX; Columna++)
                    {
                        if(Stage[Fila][Columna].id == Current.id)
                        {
                            Stage[Fila][Columna].shape.setFillColor(sf::Color::Red);
                        }
                    }
                }

                if(Current.id == End->id)
                {
                    std::cout<<"Has encontrado el final!"<<std::endl<<std::endl;

                    Casilla _temp = Current;
                    int move = 1;
                    std::vector<Casilla*> path;

                    while (_temp.PadreX != -1 && _temp.PadreY != -1)
                    {
                        path.push_back(&Stage[_temp.y][_temp.x]);
                        _temp = Stage[_temp.PadreY][_temp.PadreX];
                        move++;
                    }

                    path.push_back(&Stage[_temp.y][_temp.x]);

                    reverse(path.begin(), path.end());

                    for(Casilla *current : path)
                    {
                            current->shape.setFillColor(sf::Color::Cyan);
                    }

                    foundEnd = true;
                    goto Fin;
                }

                if(Current.x > 0 && !Stage[Current.y][Current.x - 1].isWall)
                    Current.Vecinos.push_back(&Stage[Current.y][Current.x - 1]);

                if(Current.x < (stageSizeX-1) && !Stage[Current.y][Current.x + 1].isWall)
                    Current.Vecinos.push_back(&Stage[Current.y][Current.x + 1]);

                if(Current.y > 0 && !Stage[Current.y-1][Current.x].isWall)
                    Current.Vecinos.push_back(&Stage[Current.y - 1][Current.x]);

                if(Current.y < (stageSizeY - 1) && !Stage[Current.y + 1][Current.x].isWall)
                    Current.Vecinos.push_back(&Stage[Current.y + 1][Current.x]);

                for(Casilla *vecinoActual : Current.Vecinos)
                {
                    Casilla setNeighbourTemp;

                    if(isInSet(closeSet, vecinoActual, &setNeighbourTemp))
                    {
                        goto Saltear;
                    }

                    updateNeighbourData(vecinoActual, &Current, End);

                    if(isInSet(openSet, vecinoActual,&setNeighbourTemp) && vecinoActual->g > setNeighbourTemp.g)
                    {
                        goto Saltear;
                    }

                   openSet.push_back(*vecinoActual);
                   vecinoActual->shape.setFillColor(sf::Color::Green);

                   Saltear:;
                }
            }


            Fin:;
            window.clear();

            for(int Fila = 0; Fila < stageSizeY; Fila++)
            {
                for(int Columna = 0; Columna < stageSizeX; Columna++)
                {
                    window.draw(Stage[Fila][Columna].shape);
                }
            }

            window.display();

            if(foundEnd)
            {
                system("pause");

                return 1;
            }
    }

    return 0;
}

int main()
{
    pathfindAStar();

    return 0;
}