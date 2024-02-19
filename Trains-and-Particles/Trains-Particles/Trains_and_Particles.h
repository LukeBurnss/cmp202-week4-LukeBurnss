/**
 * @file Trains_and_Particles.h
 * @mini_project Trains_and_Particles
 * @author Javad Zarrin
 * @module CMP202
 * @assessment 1
 */
#ifndef PARTICLE_H
#define PARTICLE_H

#include <vector>
#include <mutex>
#include <thread>
#include <string>
#include <iostream>
#include <vector>
#include <string>

 /**
   * A log capturing the events of trains entering, being on, and leaving the shared track.
   * Each string in the vector represents a specific event with a shorthand notation:
   * - "Train B- E": Train B is entering the shared track.
   * - "Train B- O": Train B is on the shared track.
   * - "Train B- L": Train B has left the shared track.
   * - "Train A- E": Train A is entering the shared track.
   * - "Train A- O": Train A is on the shared track.
   * - "Train A- L": Train A has left the shared track.
   * This log is used for verifying the correct sequence of events in the train simulation.
   */
std::vector<std::string> trains_log;

 // Part1:
bool isSimulationCorrect(const std::vector<std::string>& log, std::string& reason, int& record_number) {
    // Flags to keep track of the state of each train
    bool isTrainAOnTrack = false, isTrainBOnTrack = false;
    bool flagTrainA = false; // Flag to check if Train A has moved
    bool flagTrainB = false; // Flag to check if Train B has moved

    int i = 0;
    for (const auto& entry : log) {
        record_number = i; // Record the current log entry number
        i++;

        // Handling Train B's entry log
        if (entry == "Train B- E") {
            flagTrainB = true; // Train B has moved
            // Check if Train B is already on the track or Train A is on the track, which is an error
            if (isTrainBOnTrack || isTrainAOnTrack) {
                reason = "Train B cannot enter if it's already on the track or if Train A is on the track";
                return false;
            }
            isTrainBOnTrack = true; // Mark Train B as on the track
        }
        // Handling Train B's ongoing log
        else if (entry == "Train B- O") {
            // Check if Train B is not on the track, which is an error
            if (!isTrainBOnTrack) {
                reason = "Train B cannot be on the track if it hasn't entered";
                return false;
            }
        }
        // Handling Train B's leaving log
        else if (entry == "Train B- L") {
            // Check if Train B is not on the track, which is an error
            if (!isTrainBOnTrack) {
                reason = "Train B cannot leave if it's not on the track";
                return false;
            }
            isTrainBOnTrack = false; // Mark Train B as off the track
        }
        // Handling Train A's entry log
        else if (entry == "Train A- E") {
            flagTrainA = true; // Train A has moved
            // Check if Train A is already on the track or Train B is on the track, which is an error
            if (isTrainAOnTrack || isTrainBOnTrack) {
                reason = "Train A cannot enter if it's already on the track or if Train B is on the track";
                return false;
            }
            isTrainAOnTrack = true; // Mark Train A as on the track
        }
        // Handling Train A's ongoing log
        else if (entry == "Train A- O") {
            // Check if Train A is not on the track, which is an error
            if (!isTrainAOnTrack) {
                reason = "Train A cannot be on the track if it hasn't entered";
                return false;
            }
        }
        // Handling Train A's leaving log
        else if (entry == "Train A- L") {
            // Check if Train A is not on the track, which is an error
            if (!isTrainAOnTrack) {
                reason = "Train A cannot leave if it's not on the track";
                return false;
            }
            isTrainAOnTrack = false; // Mark Train A as off the track
        }
        // Handling invalid log entries
        else {
            reason = "Not a valid log entry. Have you correctly implemented a mutex?";
            return false;
        }
    }

    // Check if Train A function worked correctly
    if (flagTrainA == false) {
        reason = "The trainA function does not seem to work correctly.";
        return false;
    }

    // Check if Train B function worked correctly
    if (flagTrainB == false) {
        reason = "The trainB function does not seem to work correctly.";
        return false;
    }

    // If all entries are processed without conflict, the simulation is correct
    return true;
}

// Part1: Class RailwaySystem simulates two trains sharing a track segment.
class RailwaySystem {
public:
    RailwaySystem(); // Constructor: Initializes the positions and shared section of the tracks.
    void startSimulation(int numSteps); // Starts the simulation of the trains. It gets a parameter for the number of steps

private:
    int simulationSteps; // Stores the number of simulation steps
    void trainA(); // Simulates the behavior of Train A.
    void trainB(); // Simulates the behavior of Train B.
    void trainC(); // Simulates the behavior of Train C.
    void enterSharedTrack(const std::string& trainName); // Manages a train entering the shared track.
    void onSharedTrack(const std::string& trainName); // Manages a train currently on the shared track.
    void leaveSharedTrack(const std::string& trainName); // Manages a train leaving the shared track.
    void displayTracks(); // Displays the current state of the tracks and trains.

    std::mutex sharedTrackMutex; // Mutex for synchronizing access to the shared track section.
    int positionA, positionB, positionC; // Positions of Train A, Train B and Train C on their respective tracks.
   
    bool canMoveC; // Add this line to control trainC's movement
    std::condition_variable cvC; // Add this for signaling trainC
    std::mutex mtxC; // Mutex for trainC condition variable

    int sharedSectionStart, sharedSectionEnd; // Start and end points of the shared track section.
};

//----------------------------------------------------------------------------------------------------------------
// Part 2: 
// Global variables for the particle simulation
const size_t NUM_PARTICLES = 10; // Number of particles in the simulation
const float DT = 0.4f; // Time step for each update in the simulation
const int WIDTH = 40; // Width of the visualization grid
const int HEIGHT = 20; // Height of the visualization grid
const int NUM_STEPS = 200; // Total number of steps in the simulation
const size_t NUM_THREADS = 10; // std::thread::hardware_concurrency(); // Number of threads used for parallel processing



// Particle class represents a moving particle in the particle simulation part of the program.
class Particle {
public:
    float x, y; // Position of the particle
    float vx, vy; // Velocity of the particle
    int id; // Unique identifier for each particle
    int wallHits; // Number of times the particle hits a wall

    Particle() : x(0), y(0), vx(0), vy(0), id(-1), wallHits(0) {} // Default constructor
    Particle(int id); // Constructor: Initializes a particle with a given ID.
    void update(float dt); // Updates the particle's position based on its velocity.
};

// Function declarations for particle simulation.
void initialize_particles(std::vector<Particle>& particles); // Initializes the particles with random positions and velocities.
void update_particles(std::vector<Particle>& particles, float dt, size_t start, size_t end); // Updates a range of particles.
void visualize_particles(const std::vector<Particle>& particles, int width, int height); // Visualizes the particles on a grid.


/**
 * Tests the particle simulation by checking the final state of each particle.
 * It outputs the simulation parameters and the wall hits for each particle.
 * The function then calculates and returns the total number of wall hits
 * encountered by all particles throughout the simulation.
 *
 * @param particles The vector of Particle objects after simulation.
 * @return The sum of wall hit counts for all particles.
 *
 * Note: The assertions for position bounds and wall hit counts are commented out.
 * Uncomment these lines if you want to enable runtime checks for these conditions.
 * Adjust the range for wall hit counts based on the expected behavior of your simulation.
 */
int test_particles_sim(std::vector<Particle> particles) {
    // Output simulation parameters
    std::cout << " Part2 Results for parameters: \n";
    std::cout << "Number of Particles: " << NUM_PARTICLES << std::endl;
    std::cout << "Time Step (DT): " << DT << std::endl;
    std::cout << "Grid Width: " << WIDTH << std::endl;
    std::cout << "Grid Height: " << HEIGHT << std::endl;
    std::cout << "Number of Simulation Steps: " << NUM_STEPS << std::endl;
    std::cout << "Number of Threads: " << NUM_THREADS << std::endl;

    // Initialize total wall hits counter
    int total_number_of_wallHits = 0;

    // Iterate through each particle to check final state and wall hits
    for (const auto& particle : particles) {
        // Uncomment to use assertions for checking particle bounds and wall hits
        //assert(particle.x >= -10 && particle.x <= 10);
        //assert(particle.y >= -10 && particle.y <= 10);
        //assert(particle.wallHits >= 0 && particle.wallHits <= 20);

        // Output individual particle ID and wall hit count
        std::cout << "P-ID=" << particle.id << ", wallHits=" << particle.wallHits << std::endl;

        // Accumulate total number of wall hits
        total_number_of_wallHits += particle.wallHits;
    }

    // Return total wall hits for validation
    return total_number_of_wallHits;
}


#endif // PARTICLE_H
