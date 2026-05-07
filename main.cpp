#include <iostream>
#include <array>
#include <fstream>
#include <cassert>
#include <set>
#include <vector>
#include <algorithm>

#define FWORDLE  "./wordle.txt" // NOTE: Alphabetically sorted
#define VALID_WORDS_COUNT 10657
#define WORDLE_ATTEMPT_COUNT 6
#define WORDLE_WORD_LEN 5
#define DEBUG 0

enum class WordleCharFlag {
    GRAY, YELLOW, GREEN
};
struct WordleGuess {
    std::string word;
    std::array<WordleCharFlag, WORDLE_WORD_LEN> flags;

};

std::array<std::string, VALID_WORDS_COUNT> WordleWords{}; 
std::vector<std::string> WordleCandidates{};
void saveCandidates(const std::string& filename = "./wordle-candidates.txt") {
    std::ofstream f(filename);
    for (auto x : WordleCandidates) {
        f << x << std::endl;
    }
}
void loadWordleFile() {
    std::ifstream f(FWORDLE);
    assert(f && "File couldnt be opened");

    int i = 0;
    for (i = 0; !f.eof(); i++) f >> WordleWords[i];

    assert(i == VALID_WORDS_COUNT && "i != VALID_WORDS_COUNT");
    assert(!WordleWords.empty() && "Couldnt read file contents into array");

    WordleCandidates.assign(WordleWords.begin(), WordleWords.end());

};


std::array<WordleCharFlag, WORDLE_WORD_LEN> parseFlags(const std::string &s) {
    assert(s.length() == WORDLE_WORD_LEN);
    std::array<WordleCharFlag, WORDLE_WORD_LEN> ret{};
    for (int i = 0; i < WORDLE_WORD_LEN; i++) {
        switch(s[i]) {
            case 'x': ret[i] = WordleCharFlag::GRAY; break;
            case 'y': ret[i] = WordleCharFlag::YELLOW; break;
            case 'g': ret[i] = WordleCharFlag::GREEN; break;
            default: assert(0 && "Should be unreachable (parse_flags)");
        }
    }
    return ret;
}

void wordleHandleGuesses(const std::array<WordleGuess, WORDLE_ATTEMPT_COUNT> &guesses) {
    std::set<char> greys{};
    std::set<std::pair<char, int>> greens{}, yellows{};
    
    for (auto x : guesses) {
        for (int i = 0; auto y : x.flags) {
            switch (y) {
                case WordleCharFlag::GRAY: greys.insert(x.word[i]); break;
                case WordleCharFlag::YELLOW: yellows.insert({x.word[i], i}); break;
                case WordleCharFlag::GREEN: greens.insert({x.word[i], i}); break;
                default: assert(0 && "Should be unreachable (generate_word)");
            }
            i++;
        }
    }
    WordleCandidates.erase(
        std::remove_if(WordleCandidates.begin(), WordleCandidates.end(),
            [=](const std::string& s) {
                for (char c : s) {
                    if (greys.count(c)) return true;
                }

                for (auto& [c, i] : greens) {
                    if (s[i] != c) return true;
                }

                for (auto& [c, i] : yellows) {
                    if (s[i] == c) return true;
                    if (s.find(c) == std::string::npos) return true; 
                }

                return false;
            }   
        ),
        WordleCandidates.end()
    );

}

void wordleNarrow(std::pair<std::istream&, std::ostream&> io = {std::cin, std::cout}) {
    auto [cin, cout] = io;
    std::array<WordleGuess, WORDLE_ATTEMPT_COUNT> guesses{}; 
    for (int i = 0; i < WORDLE_ATTEMPT_COUNT; i++) {
        std::string flags{}, curr_word{};
        saveCandidates();

        cout << "Choose a word from the candidates, generated in ./wordle-candidates.txt by default" << std::endl;
        cin >> curr_word;

        assert(!curr_word.empty() && curr_word.length() == 5 && "Bad word given by user");
        assert(std::find(WordleCandidates.begin(), WordleCandidates.end(), curr_word) != WordleCandidates.end()
                && "Bad word given by user, not found in candidates");

        cout << "Flags for the chosen word? (x = gray, y = yellow, g = green)" << std::endl;
        cin >> flags;

        assert(!flags.empty() && flags.length() == WORDLE_WORD_LEN && "Bad flags given by user");
        
        guesses[i] = {
            .word = curr_word,
            .flags = parseFlags(flags)
        };
        wordleHandleGuesses(guesses);
    }
    
}

int main() {
#if DEBUG 
#endif  
    loadWordleFile();
    wordleNarrow();
}