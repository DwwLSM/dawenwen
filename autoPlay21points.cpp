
#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>


class Player {
public:
    Player(std::string _name):name(_name) {}
    std::string name;
    long long property = 800;
    long long (*input)(Player* self) = [](Player* self)->long long {
        return self->property > 500 
            ? self->property * 0.2
            : self->property * 0.5;
    };
    bool (*moreCard)(Player* self) = [](Player* self)->bool {
        return self->current_points < 17;
    };
    
    int current_points = 0;
    int current_input = 0;
    int card_A = 0;
};

class Game {
public:
    long long init_property = 800;
    std::vector<Player*> players;
    int deck = 1;
    int* cardLib;
    int card_index=0;
    bool (*endCondition)(Game* self);
    int bankerDuration = 1;
    int maxCycle = 1;
    
    Game(int _deck): deck(_deck) {
        int totalCards = deck * 52;
        cardLib = new int[totalCards];
    }

    ~Game() {
        delete[] cardLib;
    }
    
    void shuffleCards() {
        int totalCards = deck * 52;
        int cardCount = 0;
        for (int i = 0; i < deck; i++) {
            for (int j = 1; j < 10; j++) {
                for (int k = 0; k < 4; k++) {
                    cardLib[cardCount++] = j;
                }
            }
            for (int k = 0; k < 16; k++) {
                cardLib[cardCount++] = 10;
            }
        }

        srand(static_cast<unsigned>(time(0)));
        for (int i = totalCards - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(cardLib[i], cardLib[j]);
        }
        
        for (int i=0; i<totalCards; i++) 
            std::cout << cardLib[i] << ", ";
    }

    
    void join(Player* player) {
        player->property = init_property;
        players.push_back(player);
        std::cout << player->name << " joined the Game! \n";
    }
    
    void getCard(Player* player, std::string identity = "Player") {
        player->current_points += cardLib[card_index];
        if (cardLib[card_index] == 1) {
            player->card_A++;
        }
        std::cout 
            << identity 
            << " "
            << player->name
            << " get "
            << cardLib[card_index]
            << "! current points: "
            << player->current_points;
        if (player->current_points > 21)
            std::cout << " (Burst!!!)";
        std::cout << std::endl;
            
        card_index = (card_index+1) % (deck*52);
    }
    
    Player* play() {
        
        shuffleCards();
        
        for (int cycle=0; cycle<maxCycle; cycle++) {
            for (
                auto banker = players.begin();
                banker != players.end();
                ++banker
            ) for (int i=0; i<bankerDuration; i++) {
                
                (*banker)->current_points = 0;
                std::cout 
                    << "Banker: "
                    << (*banker)->name 
                    << std::endl;
                getCard(*banker, "Banker");
                
                for (
                    auto player = banker+1;
                    player != players.end();
                    ++player
                ) {
                    (**player).current_points = 0;
                    (**player).current_input = (**player).input(*player);
                    getCard(*player);
                }
                for (
                    auto player = players.begin();
                    player != banker;
                    ++player
                ) {
                    (**player).current_points = 0;
                    (**player).current_input = (**player).input(*player);
                    getCard(*player);
                }
                getCard(*banker, "Banker");
                for (
                    auto player=banker+1;
                    player != players.end();
                    ++player
                ) getCard(*player);
                for (
                    auto player = players.begin();
                    player != banker;
                    ++player
                ) getCard(*player);
                
                
                bool notFinish = false;
                do {
                    notFinish = false;
                    for (
                        auto player = banker+1;
                        player != players.end();
                        ++player
                    ) if ((*player)->moreCard(*player)) {
                        getCard(*player);
                        notFinish = true;
                    }
                    for (
                        auto player = players.begin();
                        player != banker;
                        ++player
                    ) if ((*player)->moreCard(*player)) {
                        getCard(*player);
                        notFinish = true;
                    }
                } while (notFinish);
                
                while ((**banker).current_points < 18) {
                    getCard(*banker, "Banker");
                    if ((**banker).card_A &&
                        (**banker).current_points < 12 &&
                        (**banker).current_points > 5) {
                        (**banker).current_points += 10;
                        break;
                    }
                }
                if ((**banker).current_points > 21)
                    (**banker).current_points = 0;
                
               for (
                    auto player = banker+1;
                    player != players.end();
                    ++player
                ) {
                    if ((**player).current_points > 21)
                        (**player).current_points = 0;
                    if ((**player).card_A &&
                        (**player).current_points < 12)
                        (**player).current_points += 10;
                    if ((**player).current_points >
                        (**banker).current_points) {
                        (**player).property += (**player).current_input;
                        (**banker).property -= (**player).current_input;
                    } else
                    if ((**player).current_points <
                        (**banker).current_points) {
                        (**player).property -= (**player).current_input;
                        (**banker).property += (**player).current_input;
                    }
                }
                for (
                    auto player = players.begin();
                    player != banker;
                    ++player
                ) {
                    if ((**player).current_points > 21)
                        (**player).current_points = 0;
                    if ((**player).card_A &&
                        (**player).current_points < 12)
                        (**player).current_points += 10;
                    if ((**player).current_points >
                        (**banker).current_points) {
                        (**player).property += (**player).current_input;
                        (**banker).property -= (**player).current_input;
                        std::cout 
                            << "Player "
                            << (*player)->name
                            << " win "
                            << (*player)->current_input
                            << "; property: "
                            << (*player)->property
                            << std::endl;
                    } else
                    if ((**player).current_points <
                        (**banker).current_points) {
                        (**player).property -= (**player).current_input;
                        (**banker).property += (**player).current_input;
                        std::cout 
                            << "Player "
                            << (*player)->name
                            << " loss "
                            << (*player)->current_input
                            << "; property: "
                            << (*player)->property
                            << std::endl;
                    }
                }
                std::cout 
                    << "Banker "
                    << (*banker)->name
                    << "'s property: "
                    << (*banker)->property
                    << std::endl;
            }
        }
        auto player = players.begin();
        auto winner = *player;
        std::cout 
            << "Player "
            << (*player)->name
            << "; property: "
            << (*player)->property
            << std::endl;
        for (
            ++player;
            player != players.end();
            ++player
        ) {
            std::cout 
                << "Player "
                << (*player)->name
                << "; property: "
                << (*player)->property
                << std::endl;
            if ((*player)->property > winner->property) {
                winner = *player;
            }
        }
        std::cout 
            << "Winner "
            << winner->name
            << "!!! property: "
            << winner->property
            << std::endl;
        return winner;
    }
    
}; 

int main() {
    
    srand(time(0));

    Game game(1);
    game.bankerDuration = 3;
    std::cout << "Game is Open" << std::endl;
    
    Player Player_1("Andy");
    game.join(&Player_1);
    
    Player Player_2("Ann");
    game.join(&Player_2);
    
    Player Player_3("Kevin");
    game.join(&Player_3);
    
    Player Player_4("Jack");
    game.join(&Player_4);
    
    game.play();
    
    return 0;
}
