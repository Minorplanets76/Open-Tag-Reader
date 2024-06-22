#include "otrFeedback.h"
#include <map>
#include <string>
#include <vector>

// Define a map to store note frequencies based on note names
std::map<std::string, int> noteFrequencies = {
    {"C4", 261},
    {"C#4", 277},
    {"D4", 294},
    {"D#4", 311},
    {"E4", 330},
    {"F4", 349},
    {"F#4", 370},
    {"G4", 392},
    {"G#4", 415},
    {"A4", 440},
    {"A#4", 466},
    {"B4", 494},
    {"C5", 523},
    {"C#5", 554},
    {"D5", 587},
    {"D#5", 622},
    {"E5", 659},
    {"F5", 698},
    {"F#5", 740},
    {"G5", 784},
    {"G#5", 831},
    {"A5", 880},
    {"A#5", 932},
    {"B5", 988}
};

// Function to play the melody "Click Goes The Shears"
void playClickGoesTheShears(int buzzerPin)
{
    // Melody notes and pauses defined by their names ("P" for pause)
    std::vector<std::string> melody = {"D5", "P", "D5", "C5", "B4", "G4", "C5", "E5", "C5"};
    
    // Corresponding note durations in milliseconds
    const int noteDurations[] = {600, 200, 400, 200, 600, 600, 600, 600, 1000}; // Adjust durations as needed
    
    // Variables for timing and melody index
    static unsigned long previousMillis = 0;
    static int currentNoteIndex = 0;
    const int noteDuration = 600; // Default note duration in milliseconds
    
    // Check if it's time to play the next note
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= noteDurations[currentNoteIndex]) {
        // Check if the current note is a pause ("P")
        if (melody[currentNoteIndex] == "P") {
            // Pause (do nothing for rest duration)
            Serial.println("Pause");
        } else {
            // Get the frequency of the current note from the noteFrequencies map
            int frequency = noteFrequencies[melody[currentNoteIndex]];
            
            // Play the current note
            tone(buzzerPin, frequency, noteDuration);
            
            // Print the note being played (optional)
            Serial.print("Playing note: ");
            Serial.println(melody[currentNoteIndex].c_str());
        }
        
        // Move to the next note
        currentNoteIndex++;
        if (currentNoteIndex >= melody.size()) {
            // Reset back to the beginning of the melody
            currentNoteIndex = 0;
        }
        
        // Update the previousMillis for the next note timing
        previousMillis = currentMillis;
    }
}

// Function to play the melody "Waltzing Matilda - Part 1" using note names
void playWaltzingMatilda(int buzzerPin)
{
    // Melody notes and pauses defined by their names ("P" for pause)
    std::vector<std::string> melody = {
        "A4", "A4", "E5", "A5", "P", "A4", "D5", "E5", "P", "A4", "F#5", "G5", "A5"
    };
    
    // Corresponding note durations in milliseconds
    const int noteDurations[] = {
        400, 200, 400, 400, 400, 200, 400, 400, 400, 200, 400, 400, 800
    }; // Adjust durations as needed
    
    // Variables for timing and melody index
    static unsigned long previousMillis = 0;
    static int currentNoteIndex = 0;
    const int noteDuration = 600; // Default note duration in milliseconds
    
    // Check if it's time to play the next note
    unsigned long currentMillis = millis();
    if (currentMillis - previousMillis >= noteDurations[currentNoteIndex]) {
        // Check if the current note is a pause ("P")
        if (melody[currentNoteIndex] == "P") {
            // Pause (do nothing for rest duration)
            Serial.println("Pause");
        } else {
            // Get the frequency of the current note from the noteFrequencies map
            int frequency = noteFrequencies[melody[currentNoteIndex]];
            
            // Play the current note
            tone(buzzerPin, frequency, noteDuration);

        }
        
        // Move to the next note
        currentNoteIndex++;
        if (currentNoteIndex >= melody.size()) {
            // Reset back to the beginning of the melody
            currentNoteIndex = 0;
        }
        
        // Update the previousMillis for the next note timing
        previousMillis = currentMillis;
    }
}
