#include "../headers/cacheSimulator.h"
#include <iostream>

CacheSimulator::CacheSimulator() : directCache(nullptr), nWayCache(nullptr), fullyCache(nullptr), cacheSize(0), blockSize(0), addressSize(0) {}

CacheSimulator::~CacheSimulator() {
    cleanup();
}

void CacheSimulator::run()
{
    setupCache();
    setupAddresses();

    bool running = true;
    while (running)
    {
        selectCacheType();
        simulateCache();
        
        int choice;
        do
        {
            displayMenu();
            cin >> choice;

            switch (choice)
            {
            case 1: // Show cache status
                printCacheStatus();
                break;
            case 2: // Show access history
                printAccessHistory();
                break;
            case 3: // Try different cache type
                break;
            case 4: // New addresses
                setupCache();
                setupAddresses();
                break;
            case 5: // Exit
                running = false;
                break;
            default:
                cout << "Invalid choice. Please try again." << endl;
            }

        } while (choice != 3 && choice != 4 && choice != 5);
    }
}

void CacheSimulator::displayMenu() const
{
    cout << "\n========== Cache Simulator Options ==========\n";
    cout << "1. Show cache status\n";
    cout << "2. Show access history\n";
    cout << "3. Try different cache type with same addresses\n";
    cout << "4. Start over with new cache and addresses\n";
    cout << "5. Exit\n";
    cout << "Enter choice: ";
}

void CacheSimulator::setupAddresses()
{
    cout << "\n========== Address Configuration ==========\n";
    cout << "1. Enter addresses manually\n";
    cout << "2. Use predefined addresses\n";
    cout << "Enter choice: ";
    
    int choice;
    cin >> choice;

    addresses.clear();
    
    if (choice == 1)
    {
        int numAddresses;
        cout << "Enter number of memory accesses: ";
        cin >> numAddresses;

        for (int i = 0; i < numAddresses; i++)
        {
            int address;
            cout << "Enter memory address " << (i + 1) << ": ";
            cin >> address;
            addresses.push_back(address);
        }
    }
    else
    {
        // Predefined address pattern
        addresses = {0, 4, 16, 132, 232, 160, 1024, 30, 140, 3100, 180, 2180};
        cout << "Using predefined addresses: ";
        for (int addr : addresses)
        {
            cout << addr << " ";
        }
        cout << endl;
    }
}

void CacheSimulator::setupCache()
{
    cout << "\n========== Cache Configuration ==========\n";
    cout << "Input cache size (bytes): ";
    cin >> cacheSize;
    cout << "Input block size (bytes): ";
    cin >> blockSize;
    cout << "Input address size (bits): ";
    cin >> addressSize;

    cleanup(); // Clean up previous cache objects
}

void CacheSimulator::cleanup()
{
    // Delete any previously allocated cache objects
    delete directCache;
    delete nWayCache;
    delete fullyCache;
    
    directCache = nullptr;
    nWayCache = nullptr;
    fullyCache = nullptr;
}

void CacheSimulator::selectCacheType()
{
    cout << "\n========== Cache Type Selection ==========\n";
    cout << "1. Direct Mapped\n";
    cout << "2. N-Way Set Associative\n";
    cout << "3. Fully Associative\n";
    cout << "Enter choice: ";
    cin >> cacheType;

    //claer cache
    if (directCache != nullptr) directCache->clearCache();
    if (nWayCache != nullptr) nWayCache->clearCache();
    if (fullyCache != nullptr) fullyCache->clearCache();

    switch (cacheType)
    {
    case 1: // Direct Mapped
        if (!directCache) directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
        directCache->showCacheConfiguration();
        break;
    
    case 2: // N-Way Set Associative
        cout << "Enter associativity (N): ";
        cin >> nWay;
        delete nWayCache; // Delete previous instance if any
        nWayCache = new NWaySetAssociative(cacheSize, blockSize, addressSize, nWay);
        nWayCache->showCacheConfiguration();
        break;
    
    case 3: // Fully Associative
        if (!fullyCache) fullyCache = new FullyAssociativeCache(cacheSize, blockSize, addressSize);
        fullyCache->showCacheConfiguration();
        break;
    
    default:
        cout << "Invalid choice. Using Direct Mapped as default.\n";
        cacheType = 1;
        if (!directCache) directCache = new DirectMappingCache(cacheSize, blockSize, addressSize);
        directCache->showCacheConfiguration();
    }
}

void CacheSimulator::simulateCache()
{
    cout << "\nSimulating memory access...\n";
    
    for (int addr : addresses)
    {
        switch (cacheType)
        {
        case 1:
            directCache->accessMemory(addr);
            break;
        case 2:
            nWayCache->accessMemory(addr);
            break;
        case 3:
            fullyCache->accessMemory(addr);
            break;
        }
    }
    
    cout << "Simulation complete.\n";
}

void CacheSimulator::printCacheStatus() const
{
    switch (cacheType)
    {
    case 1:
        directCache->printCacheStatus();
        break;
    case 2:
        nWayCache->printCacheStatus();
        break;
    case 3:
        fullyCache->printCacheStatus();
        break;
    }
}

void CacheSimulator::printAccessHistory() const
{
    switch (cacheType)
    {
    case 1:
        directCache->printAccessHistory();
        break;
    case 2:
        nWayCache->printAccessHistory();
        break;
    case 3:
        fullyCache->printAccessHistory();
        break;
    }
}