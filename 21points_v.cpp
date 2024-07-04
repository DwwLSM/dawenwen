#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

class Player;
class Behavior;
class Game;
struct endCondition;

#define response(n) (1<<n)


class Player {
public:
    std::string name;
    Behavior*   behavior;
    long long   property;
    int         current_points = 0;
    int         current_input  = 0;
    bool        card_A         = true;
    Player*     next;

    Player(std::string _name):name(_name) {}
};

class Behavior {
public:
    long long (*input)(Player*) = [](Player* self)->long long {
        return self->property < 5 
            ? self->property
            : self->property > 500 
                ? self->property * 0.2
                : self->property * 0.5;
    };
    bool (*moreCard)(Player*) = [](Player* self)->bool {
        return self->card_A 
            ? self->current_points < 8
            : self->current_points < 17;
    };
};


enum Event {
    onSomeoneBankrupt,
    onCycleStart,
    onCycleEnd,
    onSomeoneWin,
    onSomeoneLoss,
    end
};

class StopCondition {
public:

    bool (*Functions[Event::end])(Game*);
    int responseEvents = 0;

    void setCondition(int responseEvents, bool (*Function)(Game*)) {
        for (int event=0; event<Event::end; ++event) {
            if (responseEvents&response(event)) {
                Functions[event] = Function;
            }
        }
        this->responseEvents |= responseEvents;
    }
};


class Game {
public:
    int deck = 1;
    int bankerEvolve = 0;
    Player* current_player;
    Player* current_banker;
    Player* last_player;

    long long init_property;
    std::vector<Player*> players;
    std::vector<int>     cardLib;
    int card_index;
    int bankerDuration = 1;
    int maxCycle = 1;
    int current_cycle = 0;

    Game(int _dect) : deck(_dect) {
        cardLib.resize(deck*52);
    }

    void shuffleCards() {
        int totalCards = deck * 52;
        int cardCount = 0;
        for (int i = 0; i < deck; i++) {
            for (int j = 1; j < 10; j++) {
                cardLib[cardCount++] = j;
                cardLib[cardCount++] = j;
                cardLib[cardCount++] = j;
                cardLib[cardCount++] = j;
                cardLib[cardCount++] = 10;
            }
            cardLib[cardCount++] = 10;
            cardLib[cardCount++] = 10;
            cardLib[cardCount++] = 10;
            cardLib[cardCount++] = 10;
            cardLib[cardCount++] = 10;
            cardLib[cardCount++] = 10;
        }
        srand(static_cast<unsigned>(time(0)));
        for (int i = totalCards - 1; i > 0; i--) {
            int j = rand() % (i + 1);
            std::swap(cardLib[i], cardLib[j]);
        }

        auto it = cardLib.begin();
        switch (totalCards % 8)
        {
        case 0: do { std::cout << *(it++) << ", ";
        case 7:      std::cout << *(it++) << ", ";
        case 6:      std::cout << *(it++) << ", ";
        case 5:      std::cout << *(it++) << ", ";
        case 4:      std::cout << *(it++) << ", ";
        case 3:      std::cout << *(it++) << ", ";
        case 2:      std::cout << *(it++) << ", ";
        case 1:      std::cout << *(it++) << "\n";
                    } while (it != cardLib.end());
        } std::cout << std::endl;
        
    }

    
    void join(Player* player) {
        player->property = init_property;
        if (!player->behavior) player->behavior = new Behavior();
        this->players.push_back(player);
        if (this->last_player) {
            this->last_player->next = player;
        }
        if (!player->behavior) {
            player->behavior = new Behavior();
        }
        std::cout << player->name << " joined the Game! \n";
    }
    
    Player* newPlayer(std::string name) {
        Player* nPlayer = new Player(name);
        this->join(nPlayer);
        return nPlayer;
    }
    
    Player* PlayUnitlEnd(int bankerDuration=0, int maxCycle=0, bool settle = true) {
        if (bankerDuration)
            this->bankerDuration = bankerDuration;
        if (maxCycle)
            this->maxCycle       = maxCycle;

        return PlayerUntil(new StopCondition(), settle);
    }
    
    Player* PlayerUntil(StopCondition* cond, bool settle = true) {

        newGame();
        
        while (1) {
            if (this->PlayCycle(cond)) {
                return settle ? Settlement() : nullptr;
            }
        }
    }

    bool PlayCycle(StopCondition* cond) {

        std::cout << "\nCycle " << this->current_cycle << ":\n";
        if (
            cond->responseEvents&response(Event::onCycleStart) &&
            cond->Functions[Event::onCycleStart]
        ) return true;

        for (
            this->current_banker =  this->players.front();
            this->current_banker != this->players.back();
            this->current_banker =  this->current_banker->next
        ) {
            for (int i=0; i<this->bankerDuration; ++i) {
                if (this->PlayPeriod(cond))
                    return true;
            }
        }
        
        return 
            ++this->current_cycle == this->maxCycle || (
                cond->responseEvents&response(Event::onCycleEnd) &&
                cond->Functions[Event::onCycleEnd]
            );
    }

    bool PlayPeriod(StopCondition* cond) {
        if (this->current_banker->property <= 0) {
            return false;
        }

        bool stop = false;
        
        this->current_banker->current_points = 0;
        this->current_banker->card_A = false;
        std::cout 
            << "Banker: "
            << this->current_banker->name 
            << std::endl;
        getCard(this->current_banker, "Banker");

        playersGetCard();
        getCard(this->current_banker, "Banker");
        playersGetCard();

        bool notFinish = false;
        do {
            playersGetCard(notFinish);
        } while (notFinish);

        while(this->current_banker->current_points < 18) {
            getCard(this->current_banker, "Banker");
                if (this->current_banker->card_A &&
                    this->current_banker->current_points < 12 &&
                    this->current_banker->current_points > 5) {
                    this->current_banker->current_points += 10;
                    break;
                }
            }
            if (this->current_banker->current_points > 21)
                this->current_banker->current_points = 0;

            for (
                this->current_player =  this->current_banker->next;
                this->current_player != this->current_banker;
                this->current_player =  this->current_player->next
            ) {
                if (this->current_player->current_points > 21)
                    this->current_player->current_points = 0;
                if (this->current_player->card_A &&
                    this->current_player->current_points < 12)
                    this->current_player->current_points += 10;
                if (this->current_player->current_points >
                    this->current_banker->current_points) {
                    this->current_player->property += this->current_player->current_input;
                    this->current_banker->property -= this->current_player->current_input;
                    std::cout
                        << "Player "
                        << this->current_player->name
                        << " win "
                        << this->current_player->current_input
                        << "; property: "
                        << this->current_player->property
                        << std::endl;
                    stop = 
                        cond->responseEvents&response(Event::onSomeoneWin) &&
                        cond->Functions[Event::onSomeoneWin](this);
                } else
                if (this->current_player->current_points <
                    this->current_banker->current_points) {
                    this->current_player->property -= this->current_player->current_input;
                    this->current_banker->property += this->current_player->current_input;
                    std::cout
                        << "Player "
                        << this->current_player->name
                        << " loss "
                        << this->current_player->current_input
                        << "; property: "
                        << this->current_player->property
                        << std::endl;
                    stop = 
                        cond->responseEvents&response(Event::onSomeoneLoss) &&
                        cond->Functions[Event::onSomeoneLoss](this);
                }
            }
            std::cout 
                << "Banker "
                << this->current_banker->name
                << "; property: "
                << this->current_banker->property
                << std::endl;
        
        if (++this->bankerEvolve == bankerDuration) {
            ++this->current_banker;
            this->bankerEvolve = 0;
        }
        return stop;
    }

    void playersGetCard() {
        for (
            this->current_player =  this->current_banker->next;
            this->current_player != this->current_banker;
            this->current_player =  this->current_player->next
        ) {
            if (this->current_player->property > 0)
                getCard(this->current_player);
        }
    }

    void playersGetCard(bool& notFinish) {
        for (
            this->current_player =  this->current_banker->next;
            this->current_player != this->current_banker;
            this->current_player =  this->current_player->next
        ) {
            if (this->current_player->property > 0 && 
                this->current_player->behavior->moreCard(this->current_player)) {
                getCard(this->current_player);
                notFinish = true;
            }
        }
    }

    Player* Settlement() {
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

    void newGame() {
        std::cout << "Game is Started\n";
        players.back()->next = players.front();
        shuffleCards();
        current_cycle = 1;
    }

    void getCard(Player* player, std::string identity = "Player") {
        
        if (player->property <= 0)
            return;

        player->current_points += this->cardLib[this->card_index];
        if (this->cardLib[this->card_index] == 1) {
            player->card_A = true;
        }
        std::cout 
            << identity 
            << " "
            << player->name
            << " get "
            << this-cardLib[this->card_index]
            << "! current points: "
            << player->current_points;
        if (player->current_points > 21)
            std::cout << " (Burst!!!)";
        std::cout << std::endl;
            
        card_index = (card_index+1) % (deck*52);
    }

};

int main() {
    
    Game game(1);
    
    Player* player1 = game.newPlayer("Dawenwen");
    Player* player2 = game.newPlayer("Bosco   ");
    Player* player3 = game.newPlayer("Hayden  ");
    Player* player4 = game.newPlayer("Alex    ");
    
    game.PlayUnitlEnd();
    
    return 0;
}

#undef response


