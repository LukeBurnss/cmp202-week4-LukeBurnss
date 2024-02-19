/**
 * @file Trains_and_Particles.cpp
 * @A sample mini_project, Trains_and_Particles
 * @Javad Zarrin j.zarrin@abertay.ac.uk
 * @module CMP202
 * @
 */

#include "Trains_and_Particles.h"
#include <random>
#include <iostream>
#include <thread>
#include <chrono>
#include <fstream>
#include <mutex>

 //global variables for logging
std::once_flag log_init_flag;
std::ofstream log_file;

//log function
void initialize_logging()
{
    log_file.open("app.log"); // Open a file for logging.
    log_file << "Logging initialized." << std::endl;
}


// Function that logs messages.
// It ensures that the logging is initialized before any message is logged.
void log(const std::string& message) {
    std::call_once(log_init_flag, initialize_logging); // Ensure initialization happens only once.
    log_file << message << std::endl; // Write the message to the log file.
}

//-----------------------------------------------------------Part 1: Trains ------------------------------------------------------------------//

// Constructor: Initializes positions of the trains and the shared track section boundaries.
RailwaySystem::RailwaySystem() : positionA(0), positionB(0), positionC(0), canMoveC(false), sharedSectionStart(10), sharedSectionEnd(15) {
}

void RailwaySystem::trainC() 
{
    while (true) 
    {
        std::unique_lock<std::mutex> lock(mtxC);
        cvC.wait(lock, [this] { return canMoveC; }); // Wait until canMoveC is true
        // Movement logic for trainC
        log("Train C position: " + std::to_string(positionC));
        positionC = (positionC + 1) % 25;
        lock.unlock();
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Simulate time taken for each step
    }
}

// Starts the simulation by launching threads for Train A and Train B.
void RailwaySystem::startSimulation(int numSteps) {
    simulationSteps = numSteps;

    //Todo: Task 1 
    std::thread threadA(&RailwaySystem::trainA, this); // Creates and starts a thread for Train A (Read Note1 in the labsheet).
    std::thread threadB(&RailwaySystem::trainB, this); // Creates and starts a thread for Train B.
    std::thread threadC(&RailwaySystem::trainC, this); // Creates and starts a thread for Train C.

    threadA.detach(); // Detaches threadA to run independently.
    threadB.detach(); // Detaches threadB to run independently.
    threadC.detach(); // Detaches threadC to run independently.

    for (int step = 0; step < simulationSteps; step++) // Within a loop limited by simulationSteps, perform the following:
    {
        displayTracks(); // Continuously updates and displays the current state of the tracks. Call the displayTracks() function
        std::this_thread::sleep_for(std::chrono::milliseconds(500)); // Pauses the loop for a short duration. (Read Note2 in the labsheet).
    }
}

// Simulates the behavior of Train A.
void RailwaySystem::trainA() {
    int step = 0;

    while (step < simulationSteps / 2) {
        step++;
        // Log the position of Train A at each step
        log("Train A position: " + std::to_string(positionA));

        if (positionA == sharedSectionStart - 1) {
            enterSharedTrack("Train A"); // Train A prepares to enter the shared track.
            std::lock_guard<std::mutex> lock(mtxC);
            canMoveC = true; // Signal trainC to start
            cvC.notify_one();
        }

        if (positionA >= sharedSectionStart && positionA <= sharedSectionEnd) {
            onSharedTrack("Train A"); // Train A is on the shared track.
        }

        if (positionA == sharedSectionEnd) {
            leaveSharedTrack("Train A"); // Train A leaves the shared track.
        }

        positionA = (positionA + 1) % 25; // Moves Train A forward and loops around the track.
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits before the next movement.
    }
}

// Simulates the behavior of Train B.
void RailwaySystem::trainB() {
    // Todo: Task 2
        // Implement this function in a manner very similar to the trainA function.
    int step = 0;

    while (step < simulationSteps / 2) {
        step++;
        // Log the position of Train B at each step
        log("Train B position: " + std::to_string(positionB));

        if (positionB == sharedSectionStart - 1) {
            enterSharedTrack("Train B"); // Train B prepares to enter the shared track.
        }

        if (positionB >= sharedSectionStart && positionB <= sharedSectionEnd) {
            onSharedTrack("Train B"); // Train B is on the shared track.
        }

        if (positionB == sharedSectionEnd) {
            leaveSharedTrack("Train B"); // Train B leaves the shared track.
            std::lock_guard<std::mutex> lock(mtxC);
            canMoveC = false; // Signal trainC to stop
            cvC.notify_one();
        }

        positionB = (positionB + 1) % 25; // Moves Train B forward and loops around the track.
        std::this_thread::sleep_for(std::chrono::seconds(1)); // Waits before the next movement.
    }
}

// Manages a train entering the shared track section.
void RailwaySystem::enterSharedTrack(const std::string& trainName) {

    sharedTrackMutex.lock(); // Locks the mutex to ensure exclusive access to the shared track.
    std::cout << trainName << " is entering the shared track." << std::endl; // Outputs a message (e.g. "Train A is entering the shared track.") indicating the given train is entering the shared track.
    log(trainName + " is entering the shared track."); // Log this event
    trains_log.push_back(trainName + "- E"); // Update the trains_log (see the trains_log description in the header file)

}

// Manages a train currently on the shared track.
void RailwaySystem::onSharedTrack(const std::string& trainName) {
    // Append a log entry indicating that the train is currently on the shared track.
    // The suffix "- O" signifies the "On the track" status of the train.
    // For example, if 'trainName' is "Train A", the log entry will be "Train A- O".
    std::cout << trainName << " is on the shared track." << std::endl; // Outputs a message (e.g. "Train B is on the shared track.") indicating the given train is on the shared track.
    log(trainName + " is on the shared track."); // Log this event
    trains_log.push_back(trainName + "- O");
}

// Manages a train leaving the shared track section.
void RailwaySystem::leaveSharedTrack(const std::string& trainName) {
    //Todo: Task3 
    trains_log.push_back(trainName + "- L"); // Update the trains_log (see the trains_log description in the header file)
    std::cout << trainName << " has left the shared track." << std::endl; // Outputs a message (e.g. "Train B has left the shared track.") indicating the given train has left the shared track.
    log(trainName + " has left the shared track."); // Log this event
    sharedTrackMutex.unlock(); // Unlocks the mutex, allowing the other train to access the shared track.

}

// Displays the current state of the tracks and trains.
void RailwaySystem::displayTracks() {
    std::cout << "Displaying current tracks state..." << std::endl; // Example display message
    log("Displaying current tracks state..."); // Log this event for record-keeping
    std::cout << "\x1B[2J\x1B[H"; // Clears the screen for fresh display of tracks.

    const int trackLength = 10; // Length of individual track sections.
    std::string space(5, ' '); // Spacer between track sections.
    std::string space2(10, ' '); // Spacer for shared track section.
    std::string trackA(trackLength, '-'); // Visual representation of Track A.
    trackA = trackA + space + trackA; // Combining sections for Track A.
    std::string trackB(trackLength, '-'); // Visual representation of Track B.
    trackB = trackB + space + trackB; // Combining sections for Track B.
    std::string sharedTrack(5, '-'); // Visual representation of the shared track.
    sharedTrack = space2 + sharedTrack + space2; // Positioning the shared track.

    std::string trackC(trackLength, '-'); // Adding visual representation of Track C.
    trackC[positionC] = 'C'; // Set Train C's position on its track.


    // Update the position of Train A on the track.
    trackA[positionA] = positionA >= sharedSectionStart && positionA < sharedSectionEnd ? ' ' : 'A';
    // Update the position of Train B on the track.
    trackB[positionB] = positionB >= sharedSectionStart && positionB < sharedSectionEnd ? ' ' : 'B';
    // Reflect Train A's position on the shared track.
    sharedTrack[positionA] = positionA >= sharedSectionStart && positionA < sharedSectionEnd ? 'A' : sharedTrack[positionA];
    // Reflect Train B's position on the shared track.
    sharedTrack[positionB] = positionB >= sharedSectionStart && positionB < sharedSectionEnd ? 'B' : sharedTrack[positionB];
    


    // Print the current state of all tracks.
    std::cout << "Track A:      " << trackA << std::endl;
    std::cout << "Shared Track: " << sharedTrack << std::endl;
    std::cout << "Track B:      " << trackB << std::endl;
    std::cout << "Track C: " << trackC << std::endl; // Display Track C.

}

//------------------------------------------------------------Part 2: Moving Particles --------------------------------------------------------//
// The following part of the code is related to the particle simulation.


// Implementation of the Particle class constructor
Particle::Particle(int id) : x(0), y(0), vx(0), vy(0), id(id), wallHits(0) {
    // Initializes a particle with a unique ID and zero initial position, velocity, and wall hits.
}

// Updates the position of a particle based on its velocity and checks for boundary collisions
void Particle::update(float dt) {
    x += vx * dt; // Update the x-coordinate of the particle's position
    y += vy * dt; // Update the y-coordinate of the particle's position

    // Check for boundary collisions and reverse velocity if a collision occurs
    if (x <= -10 || x >= 10) {
        vx *= -1; // Reverse the x-velocity
        wallHits++; // Increment the wall hit count
    }
    if (y <= -10 || y >= 10) {
        vy *= -1; // Reverse the y-velocity
        wallHits++; // Increment the wall hit count
    }
}

// Initializes particles with random positions and velocities
void initialize_particles(std::vector<Particle>& particles) {
    //std::random_device rd; // Random number generator
    //std::mt19937 gen(rd()); // Mersenne Twister generator. I replaced this with the line below (including a fixed seed value) for the purpose of reproducibility
    std::mt19937 gen(12345); // Fixed seed value ensures reproducibility

    std::uniform_real_distribution<> dis(-10.0, 10.0); // Distribution for position and velocity

    for (auto& p : particles) {
        p.x = dis(gen); // Set random x-coordinate
        p.y = dis(gen); // Set random y-coordinate
        p.vx = dis(gen) * 0.1f; // Set random x-velocity
        p.vy = dis(gen) * 0.1f; // Set random y-velocity
    }
}

// Updates a range of particles in parallel
void update_particles(std::vector<Particle>& particles, float dt, size_t start, size_t end) {
    for (size_t i = start; i < end; ++i) {
        particles[i].update(dt); // Update each particle in the specified range
    }
}

// Visualizes particles on a 2D grid in the console
void visualize_particles(const std::vector<Particle>& particles, int width, int height) {
    std::vector<std::string> grid(width * height, " "); // Create a blank grid

    // Create the visual representation of the walls
    for (int x = 0; x < width; ++x) {
        grid[x] = "-"; // Top wall
        grid[x + (height - 1) * width] = "-"; // Bottom wall
    }
    for (int y = 0; y < height; ++y) {
        grid[y * width] = "|"; // Left wall
        grid[(y + 1) * width - 1] = "|"; // Right wall
    }

    // Plot each particle on the grid
    for (const auto& p : particles) {
        int x = static_cast<int>((p.x + 10) / 20 * (width - 2)) + 1;
        int y = static_cast<int>((p.y + 10) / 20 * (height - 2)) + 1;
        if (x > 0 && x < width - 1 && y > 0 && y < height - 1) {
            std::string color; // Color code for the particle based on the number of wall hits
            // Assign color based on wall hit count
            if (p.wallHits < 3) color = "\033[37m"; // White for fewer than 3 hits
            else if (p.wallHits < 5) color = "\033[32m"; // Green for 3-4 hits
            else if (p.wallHits < 7) color = "\033[33m"; // Yellow for 5-6 hits
            else color = "\033[31m"; // Red for 7 or more hits

            grid[x + y * width] = color + std::to_string(p.id) + "\033[0m"; // Set the particle's position on the grid with color and ID
        }
    }

    // Draw the grid to the console
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            std::cout << grid[x + y * width]; // Print each cell of the grid
        }
        std::cout << std::endl; // New line at the end of each row
    }
}

// Manages the parallel movement of particles in the simulation
std::vector<Particle> parallel_moving_particles() {
    std::vector<Particle> pars;
    std::cout << " You need to implement the parallel_moving_particles() method before testing it.\n\n";
    //Todo Task 4
    std::vector<Particle> particles(NUM_PARTICLES); // Container for particles
    std::vector<std::thread> threads(NUM_THREADS); // Container for threads

    for (int i = 0; i < NUM_PARTICLES; i++) // Initialize particles with unique IDs
    {
        particles[i] = Particle(i); // Add new particles to the vector
    }

    initialize_particles(particles); // Initialize particles with random positions and velocities

    for (int step = 0; step < NUM_STEPS; step++) // Run the simulation for a set number of steps
    {
        int step_size = NUM_PARTICLES / NUM_THREADS; // Calculate the number of particles each thread should handle

        for (int i = 0; i < NUM_THREADS; i++) // Launch threads to update particles in parallel
        {
            int start = i * step_size; // Determine the start index for each thread
            int end = (i + 1) * step_size; // Determine the end index for each thread

            if (i == NUM_THREADS - 1) // Ensure the last thread covers the remaining particles
            {
                end = NUM_PARTICLES; // Start the thread for updating particles
            }

            threads[i] = std::thread(update_particles, std::ref(particles), DT, start, end);

        }

        for (auto& t : threads) // Join threads after their completion
        {
            if (t.joinable()) // Wait for the thread to finish
            {
                t.join();
            }
        }

        std::cout << "\x1B[2J\x1B[H"; // Clear the console for the next visualization

        visualize_particles(particles, WIDTH, HEIGHT); // Visualize particles on the grid

        std::this_thread::sleep_for(std::chrono::milliseconds(100));// Wait for a short time before the next update
    }
    return particles; // return the particles
    return pars;
}

int main() {
    log("Simulation started."); // Log message indicating the start of the simulation
    //-----------------------------------------------------------Test Part 1: Trains ------------------------------------------------------------------//
   // To test Part 1, comment out the code specified below. Note that in the main function, you should comment out either the code for testing Part 1 or the code for testing Part 2, but not both at the same time.
    int numSimulationSteps = 43; // Define the number of steps you want the simulation to run

    RailwaySystem railwaySystem;
    railwaySystem.startSimulation(numSimulationSteps);


    // reading railway simulation logs
    std::vector<std::string> log = trains_log;

    std::string log_string;
    int record_index = 0;
    int index = 0;
    for (std::string rec : log) {
        std::cout << index << " --" << rec << std::endl;
        index++;

    }

    if (isSimulationCorrect(log, std::ref(log_string), std::ref(record_index))) {
        std::cout << "Railway Simulation is correct." << std::endl;
    }
    else {
        std::cout << "Error in Railway Simulation." << std::endl << log_string << " record_index=" << record_index << "\n\n\n";
    }



    //------------------------------------------------------------Test Part 2: Moving Particles --------------------------------------------------------//
    //To test Part 2, comment out the code specified below. Note that in the main function, you should comment out either the code for testing Part 1 or the code for testing Part 2, but not both at the same time.

    /*std::vector<Particle> pars = parallel_moving_particles(); // Call the function to run the parallel particle simulation
    int total_number_of_wallHits =test_particles_sim(pars);
    std::cout << "If your Part 2 simulation is correct, then you should get a total number of wall hits equal to 49\n";
    std::cout << "Your Part2 Results:\n  total_number_of_wallHits=" << total_number_of_wallHits <<"\n\n";
    */


    // Other outputs for Part2 implementatioin - Depending on the operating system's scheduling of the threads, your results might differ slightly from those shown below. However, the total number of wall hits should still be the same, at 49."
    //Part2 Results for parameters:
    //Number of Particles : 10
    //    Time Step(DT) : 0.4
    //    Grid Width : 40
    //    Grid Height : 20
    //    Number of Simulation Steps : 200
    //    Number of Threads : 10
    //    P - ID = 0, wallHits = 5
    //    P - ID = 1, wallHits = 4
    //    P - ID = 2, wallHits = 7
    //    P - ID = 3, wallHits = 3
    //    P - ID = 4, wallHits = 7
    //    P - ID = 5, wallHits = 3
    //    P - ID = 6, wallHits = 4
    //    P - ID = 7, wallHits = 3
    //    P - ID = 8, wallHits = 7
    //    P - ID = 9, wallHits = 6
    // 
    //  total_number_of_wallHits=49

    return 0;
}

