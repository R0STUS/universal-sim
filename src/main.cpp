#include "main.hpp"


void out(std::string from, std::string input, std::string what) {
    bool isDebug = false;
    if (isDebug == true || what != "debug")
        std::cout << "  [" << from << "]  " << what << "  " << input << std::endl;
}

long random(long max) {
    return (std::rand() % max);
}

struct Color {
    int r;
    int g;
    int b;
};

struct Civilisation {
    double commType;
    double commBarrier;
    double developSpeed;
    double developBarrier;
    double develop;
};

struct Position {
    long double x;
    long double y;
};

struct Velocity {
    double x;
    double y;
};

struct Planet {
    int id;
    double water;
    double temperature;
    double civilChance;
    double civilSpeed;
    double civil;
};

struct Object {
    long id;
    Position pos;
    std::vector<Planet> planets;
    double weight;
    double meteoriteChance;
    std::vector<Civilisation> civilisations;
    bool isInfinityWeight;
    Velocity vel;
};

Velocity computeVelocity(Position p) {
    long double r = std::sqrt(p.x * p.x + p.y * p.y);
    if (r == 0) {
        return {0.0f, 0.0f};
    }
    long double normX = p.x / r;
    long double normY = p.y / r;
    double velocityX = -normY;
    double velocityY = normX;
    return {velocityX * 0.0005, velocityY * 0.0005};
}

bool isInRadius(Position center, Position point, double radius) {
    return ((point.x - center.x) * (point.x - center.x) + (point.y - center.y) * (point.y - center.y)) <= (radius * radius);
}

Color weightToRGB(int temperature) {
    Color color;
    double r, g, b;
    temperature = std::clamp(temperature * 50, 1000, 40000);
    if (temperature <= 6600) {
        r = 255;
        g = (temperature / 100) - 2;
        g = (g < 0) ? 0 : (g > 255) ? 255 : g;
        b = (temperature / 100 - 10) / 5;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
    }
    else if (temperature > 6600 && temperature <= 35000) {
        r = (temperature - 6000) / 400 * 255;
        r = (r < 0) ? 0 : (r > 255) ? 255 : r;
        g = 255;
        b = 0;
    }
    else {
        r = 65;
        g = (temperature / 100) - 2;
        g = ((g < 0) ? 0 : (g > 255) ? 255 : g) * 0.5;
        b = (temperature / 100) - 10;
        b = (b < 0) ? 0 : (b > 255) ? 255 : b;
    }
    color.r = static_cast<int>(r);
    color.g = static_cast<int>(g);
    color.b = static_cast<int>(b);
    return color;
}

std::vector<Object> generateUniverse() {
    std::vector<Object> result;
    std::string funcType = "universe-generator";
    out(funcType, "Generating objects...", "info");
    long maxObjects = (random(750) + 250) * 0.5;
    int dir;
    maxObjects = 1024;
    out(funcType, "Max objects: " + std::to_string(maxObjects), "debug");
    for (long i = 0; i < maxObjects; i++) {
        result.push_back({i});
        dir = random(720);
        result[i].pos = {(std::sin(dir) * random(250)) * 2, (std::cos(dir) * random(250)) * 2};
        result[i].weight = random(1000) * 0.75;
        if (result[i].weight >= 745) {
            result[i].weight = 1024 * 86;
            //result[i].pos.x *= 0.05;
            //result[i].pos.y *= 0.05;
            result[i].isInfinityWeight = true;
            out(funcType, "Object " + std::to_string(i) + " is black hole. Coords: " + std::to_string(result[i].pos.x) + " / " + std::to_string(result[i].pos.y), "debug");
        }
        else
            result[i].isInfinityWeight = false;
        if (result[i].isInfinityWeight == false) {
            int planetsTotal = (random(10) + 1) * (result[i].weight * 0.0075);
            for (int ii = 0; ii < planetsTotal; ii++) {
                result[i].planets.push_back({ii, random(200) * 0.01, random(200) * 0.01, random(200) * 0.01, random(200) * 0.01});
            }
            if (result[i].planets.size() == 1) {
                out(funcType, "Object " + std::to_string(i) + " is single-planet.", "debug");
            }
        }
        result[i].vel = computeVelocity(result[i].pos);
        if (result[i].isInfinityWeight)
            result[i].vel = {0, 0};
        // out(funcType, "Created objects: " + std::to_string(i + 1), "debug");
    }
    out(funcType, "Total generated objects: " + std::to_string(maxObjects), "info");
    return result;
}

int main() {
    std::srand(static_cast<unsigned int>(std::time(0)));
    std::string funcType = "MAIN";
    out(funcType, "Initialising universe...", "info");
    std::vector<Object> objects = generateUniverse();
    out(funcType, "Creating window...", "info");
    sf::ContextSettings settings;
    settings.antialiasingLevel = 16;
    sf::RenderWindow window(sf::VideoMode::getDesktopMode(), "universe-sim", sf::Style::Fullscreen, settings);
    sf::CircleShape circle;
    double force;
    Color color;
    out(funcType, "Universe initialised.", "info");
    while (window.isOpen()) {
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed)
                window.close();
        }
        window.clear();
        // auto t1 = std::chrono::high_resolution_clock::now();
        for (size_t i = 0; i < objects.size(); ++i) {
            bool isInRadiusNow = false;
            auto& obj = objects[i];
            for (size_t j = 0; j < objects.size(); ++j) {
                if (i != j) {
                    auto& targ = objects[j];
                    double deltaPosY = targ.pos.y - obj.pos.y;
                    double deltaPosX = targ.pos.x - obj.pos.x;
                    double distance = std::sqrt(deltaPosX * deltaPosX + deltaPosY * deltaPosY);
                    if (distance > 0) {
                        double dist = std::pow(distance, 2) * 10;
                        if (dist < 1)
                            dist = 1;
                        if (obj.weight > targ.weight) {
                            force = ((targ.weight / (obj.weight * 500)) / dist) * 0.00000000000000025;//0.00000000000000025
                        }
                        else
                            force = ((obj.weight * (targ.weight * 500)) / dist) * 0.00000000000000025;
                        obj.vel.x += force * (deltaPosX / distance);
                        obj.vel.y += force * (deltaPosY / distance);
                    }
                    double radius = targ.weight * 0.0001;
                    if (targ.isInfinityWeight) {
                        radius *= 0.25;
                    }
                    if (isInRadius(targ.pos, obj.pos, radius)) {
                        bool isDeleteAttent = false;
                        bool uped = false;
                        if (objects[j].id > objects[i].id) {
                            isDeleteAttent = true;
                        }
                        bool firstDeleted = false;
                        if (!obj.isInfinityWeight) {
                            objects.erase(std::remove_if(objects.begin(), objects.end(), [&obj](const Object& o) { return o.id == obj.id; }), objects.end());
                            isInRadiusNow = true;
                            break;
                        }
                        else {
                            objects[i].weight += targ.weight;
                            if (objects[i].vel.x > 0 && objects[j].vel.x > 0 || objects[i].vel.x < 0 && objects[j].vel.x < 0) {
                                objects[i].vel.x -= objects[j].vel.y * (objects[i].weight / objects[j].weight);
                            }
                            else {
                                objects[i].vel.x -= (objects[j].vel.y * -1) * (objects[i].weight / objects[j].weight);
                            }
                            if (objects[i].vel.y > 0 && objects[j].vel.y > 0 || objects[i].vel.y > 0 && objects[j].vel.y > 0) {
                                objects[i].vel.y -= objects[j].vel.y * (objects[i].weight / objects[j].weight);
                            }
                            else {
                                objects[i].vel.y -= (objects[j].vel.y * -1) * (objects[i].weight / objects[j].weight);
                            }
                            objects[i].vel.y = 0;
                            objects[i].vel.x = 0;
                            objects.erase(std::remove_if(objects.begin(), objects.end(), [&targ](const Object& o) { return o.id == targ.id; }), objects.end());
                            uped = true;
                        }
                        if (!objects[j].isInfinityWeight && !uped) {
                            if (firstDeleted == false || isDeleteAttent == false) {
                                objects.erase(std::remove_if(objects.begin(), objects.end(), [&targ](const Object& o) { return o.id == targ.id; }), objects.end());
                                break;
                            }
                            else {
                                objects.erase(std::remove_if(objects.begin(), objects.end(), [&targ](const Object& o) { return o.id == targ.id; }), objects.end());
                                break;
                            }
                        }
                        else if (firstDeleted == false || isDeleteAttent == false) {
                            if (!uped && objects[j].isInfinityWeight == true) {
                                if (objects[i].vel.x > 0 && objects[j].vel.x > 0 || objects[i].vel.x < 0 && objects[j].vel.x < 0) {
                                    objects[j].vel.x -= objects[i].vel.y * (objects[j].weight / objects[i].weight);
                                }
                                else {
                                    objects[j].vel.x -= (objects[i].vel.y * -1) * (objects[j].weight / objects[i].weight);
                                }
                                if (objects[i].vel.y > 0 && objects[j].vel.y > 0 || objects[i].vel.y > 0 && objects[j].vel.y > 0) {
                                    objects[j].vel.y -= objects[i].vel.y * (objects[j].weight / objects[i].weight);
                                }
                                else {
                                    objects[j].vel.y -= (objects[i].vel.y * -1) * (objects[j].weight / objects[i].weight);
                                }
                                objects[j].vel.y = 0;
                                objects[j].vel.x = 0;
                                objects[j].weight += objects[i].weight;
                                break;
                            }
                        }
                        else if (!uped && objects[j + 1].isInfinityWeight == true) {
                                if (objects[i].vel.x > 0 && objects[j + 1].vel.x > 0 || objects[i].vel.x < 0 && objects[j + 1].vel.x < 0) {
                                    objects[j + 1].vel.x -= objects[i].vel.y * (objects[j + 1].weight / objects[i].weight);
                                }
                                else {
                                    objects[j + 1].vel.x -= (objects[i].vel.y * -1) * (objects[j + 1].weight / objects[i].weight);
                                }
                                if (objects[i].vel.y > 0 && objects[j].vel.y > 0 || objects[i].vel.y > 0 && objects[j].vel.y > 0) {
                                    objects[j + 1].vel.y -= objects[i].vel.y * (objects[j + 1].weight / objects[i].weight);
                                }
                                else {
                                    objects[j + 1].vel.y -= (objects[i].vel.y * -1) * (objects[j + 1].weight / objects[i].weight);
                                }
                            objects[j + 1].vel.x = 0;
                            objects[j + 1].vel.y = 0;
                            objects[j + 1].weight += objects[i].weight;
                            break;
                        }
                    }
                }
            }
            if (!isInRadiusNow) {
                obj.pos.x += obj.vel.x;
                obj.pos.y += obj.vel.y;
                if (objects[i].pos.x > 2000 || objects[i].pos.x < -2000) {
                    objects.erase(std::remove_if(objects.begin(), objects.end(), [&objects, &i](const Object& o) { return o.id == objects[i].id; }), objects.end());
                    isInRadiusNow = true;
                    out(funcType, "Object " + std::to_string(obj.id) + " has flown to outer space.", "info");
                }
                if (objects[i].pos.y > 1000 || objects[i].pos.y < -1000) {
                    objects.erase(std::remove_if(objects.begin(), objects.end(), [&objects, &i](const Object& o) { return o.id == objects[i].id; }), objects.end());
                    isInRadiusNow = true;
                    out(funcType, "Object " + std::to_string(obj.id) + " has flown to outer space.", "info");
                }
                if (!isInRadiusNow) {
                    out(funcType, "Id: " + std::to_string(obj.id) + " Coords: " + std::to_string(obj.pos.x) + " / " + std::to_string(obj.pos.y), "debug");
                    color = weightToRGB(objects[i].weight);
                    circle.setFillColor(sf::Color(color.r, color.g, color.b));
                    double radius = objects[i].weight * 0.0001;
                    circle.setOutlineThickness(0);
                    circle.setOutlineColor(sf::Color(0, 0, 0));

                        circle.setOutlineThickness(2.5);
                        circle.setOutlineColor(sf::Color(100, 0, 0));

                    if (objects[i].isInfinityWeight) {
                        radius *= 0.25;
                        circle.setFillColor(sf::Color(0, 0, 1));
                        circle.setOutlineThickness(1);
                        circle.setOutlineColor(sf::Color(250, 150, 100));
                    }
                    circle.setRadius(radius);
                    circle.setPosition((objects[i].pos.x + 1000) - radius, (objects[i].pos.y + 500) - radius);
                    window.draw(circle);
                }
            }
        }
        window.display();
        // auto t2 = std::chrono::high_resolution_clock::now();
        // out(funcType, "Delta ms: " + std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1).count()), "info");
    }
    out(funcType, "End of universe.", "info");
    return 0;
}