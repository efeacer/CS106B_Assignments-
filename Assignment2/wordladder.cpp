/**
  * This program is a console based game in which you enter an initial word and a destination
  * word. Then the program generates and displays a word ladder from the initial word to the
  * destination word. A word ladder is the shortest path generated by the meaningful words
  * that are different by their ancestors by a single letter. Following code involves functions
  * to produce, check and print the word ladder.
  * @author EFE ACER
  * CS106B - Section Leader: Ryan Kurohara
  */

//necessary includes
#include <cctype>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include "console.h"
#include "filelib.h"
#include "simpio.h"
#include "lexicon.h"
#include "queue.h"
#include "stack.h"
#include "hashset.h"

using namespace std;

//constant declerations for further editing
const string INTRO = "Welcome to CS 106B Word Ladder.\n"
                     "Please give me two English words, and I will change the\n"
                     "first into the second by changing one letter at a time.\n\n";
const string FILE_PROMPT = "Dictionary file name? ";
const string WORD1_PROMPT = "\nWord #1 (or Enter to quit): ";
const string WORD2_PROMPT = "Word #2 (or Enter to quit): ";
const string LADDER_DISPLAY = "A ladder from back to :\n";
const string LADDER_NOT_FOUND = "No word ladder found from back to .\n";
const string WORD_LENGTH_ERROR = "The two words must be the same length.\n";
const string NOT_FOUND_ERROR = "The two words must be found in the dictionary.\n";
const string SAME_WORD_ERROR = "The two words must be different.\n";
const string FILE_ERROR = "Unable to open that file.  Try again.\n";
const string ALPHABET = "abcdefghijklmnopqrstuvwxyz";

//function declerations
bool getWords(Lexicon &englishWords, string &word1, string &word2);
Lexicon getLexiconFromFile(string &file);
bool checkWords(Lexicon &words, string &word1, string &word2);
Stack<string> findShortestPath(Lexicon &words, string &word1, string &word2);
HashSet<string> findNeighbors(Lexicon &words, string word);
void displayLadder(Stack<string> &stack, string &word1, string &word2);

//main function
int main() {
    cout << INTRO; //displaying the intro welcome message
    string file;
    Lexicon englishWords = getLexiconFromFile(file); //generating a lexicon from a file containing words
    string word1, word2;
    bool quit;
    do {
        quit = getWords(englishWords, word1, word2); //getting the words till the user decides not to enter them
        if (!quit) {
            Stack<string> toDisplay = findShortestPath(englishWords, word1, word2);
            displayLadder(toDisplay, word1, word2); //displays the ladder
        }
    } while (!quit);
    cout << "Have a nice day." << endl;
    return 0;
}

/**
 * @brief getWords Asks for the initial word and the destination word, prints specific error messages
 * if the user enters an invalid input. Repeats this process till the user decides not to enter a word.
 * @param englishWords A reference to the Lexicon containing english words.
 * @param word1 The initial word in the ladder.
 * @param word2 The destination word in the ladder.
 * @return A bool expression indicating user's decision to stop input.
 */
bool getWords(Lexicon &words, string &word1, string &word2) {
    bool quit = false;
    do { //asking for the words
        word1 = getLine(WORD1_PROMPT);
        word1 = toLowerCase(word1);
        if (word1 == "") {
            quit = true;
        }
        else {
            word2 = getLine(WORD2_PROMPT);
            word2 = toLowerCase(word2);
        }
        if (word2 == "") {
            quit = true;
        }
    } while (!quit && !checkWords(words, word1, word2));
    return quit;
}

/**
 * @brief getLexiconFromFile Generates and returns a Lexicon from a file containing words, asks for the
 * desired file name before saving it as a lexicon.
 * @param file The name of the file containing the words.
 * @return The Lexicon generated from the specified file.
 */
Lexicon getLexiconFromFile(string &file) {
    do { //promting a file and processing it
        file = getLine(FILE_PROMPT);
        if (!isFile(file)) {
            cout << FILE_ERROR;
        }
    } while (!isFile(file));
    Lexicon words(file); //placing the words to a lexicon
    return words;
}

/**
 * @brief checkWords Checks whether the first and second words are valid or not. If the words are the
 * same or their lengths are different or they are not english, prints a specified error message.
 * @param words A reference to the Lexicon containing english words.
 * @param word1 The first word that the user entered.
 * @param word2 The second word that the user entered.
 * @return A bool expression that is true if the words are valid and false otherwise.
 */
bool checkWords(Lexicon &words, string &word1, string &word2) {
    if (word1 == word2) {
        cout << SAME_WORD_ERROR;
        return false;
    }
    else if (word1.length() != word2.length()) {
        cout << WORD_LENGTH_ERROR;
        return false;
    }
    else if (!words.contains(word1) || !words.contains(word2)) {
        cout << NOT_FOUND_ERROR;
        return false;
    }
    return true;
}

/**
 * @brief findShortestPath Finds and returns the shortest word ladder connecting
 * two words as a stack of strings. Returns an empty stack if there is no such word
 * ladder. Uses BFS (Breath-first search) algorithm.
 * @param words A reference to the Lexicon containing english words.
 * @param word1 The initial word in the ladder.
 * @param word2 The destination word in the ladder.
 * @return The stack containing the words establishing the word ladder.
 */
Stack<string> findShortestPath(Lexicon &words, string &word1, string &word2) {
    Queue<Stack<string>> ladders = {{word1}};
    Stack<string> ladder;
    HashSet<string> used = {word1};
    while (!ladders.isEmpty()) { //implementing the BFS algorithm
        ladder = ladders.dequeue();
        if (ladder.peek() == word2) {
            return ladder;
        }
        for (string neighbor: findNeighbors(words, ladder.peek())) {
            if (!used.contains(neighbor)) {
                Stack<string> copyLadder = ladder;
                copyLadder.push(neighbor);
                used.add(neighbor);
                ladders.enqueue(copyLadder);
            }
        }
    }
    return {};
}

/**
 * @brief findNeighbors Finds and returns all meaningful english words formed by changing
 * only a single letter in a given word as a HashSet of strings.
 * @param words A reference to the Lexicon containing english words.
 * @param word The given initial word.
 * @return The HashSet of strings containing all the neighbors of the given word.
 */
HashSet<string> findNeighbors(Lexicon &words, string word) {
    HashSet<string> neighbors;
    string copy = word;
    for (int i = 0; i < word.length(); i++) { //nested loops to find the neighbors
        for (int j = 0; j < ALPHABET.length(); j++) {
            copy[i] = ALPHABET[j];
            if (words.contains(copy)) {
                neighbors.add(copy);
            }
        }
        copy = word;
    }
    return neighbors;
}

/**
 * @brief displayLadder Displays the word ladder on the console.
 * @param stack A reference to the collection representing the word ladder as a stack.
 * @param word1 The initial word.
 * @param word2 The destination word.
 */
void displayLadder(Stack<string> &stack, string &word1, string &word2) {
    if (stack.isEmpty()) {
        cout << LADDER_NOT_FOUND.substr(0, 26) + word1 + LADDER_NOT_FOUND.substr(25, 9)
                + word2 + LADDER_NOT_FOUND.substr(LADDER_NOT_FOUND.length() - 2);
    }
    else {
        cout << LADDER_DISPLAY.substr(0, 14) + word2 + LADDER_DISPLAY.substr(13,9)
                + word1 + LADDER_DISPLAY.substr(LADDER_DISPLAY.length() - 2);
        while (!stack.isEmpty()) {
            cout << stack.pop() << " ";
        }
        cout << endl;
    }
}