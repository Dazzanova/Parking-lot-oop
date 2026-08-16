#include<iostream>
#include<chrono>
#include<vector>
#include<memory>
using namespace std;

class PricingStrategy{
    public:
    virtual double calculateFee(double hours) = 0;
    virtual ~PricingStrategy() {}
};

class HourlyPricing : public PricingStrategy{
    double rate;
    public:
    HourlyPricing(double r) : rate(r) {}
    double calculateFee(double hours) override {
        return rate * hours;
    }
};

class Vehicle{
    chrono::time_point<chrono::system_clock> parkingStartTime;
    unique_ptr<PricingStrategy> pricing;

    public:
    Vehicle(unique_ptr<PricingStrategy> p) : pricing(move(p)) {}

    void startParking(){
        parkingStartTime = chrono::system_clock::now();
    }


    double calculateParkingDurationInHours(){
        auto currentTime = chrono::system_clock::now();
        auto duration = chrono::duration_cast<chrono::seconds>(currentTime - parkingStartTime);
        return duration.count() / 3600.0;
    }

    double calculateParkingFee() {
        return pricing->calculateFee(calculateParkingDurationInHours());
    }

    virtual string getType() = 0;
    virtual ~Vehicle() = default;
};

class Car : public Vehicle{
    public:
    string getType() override { return "C"; }
    Car() : Vehicle(make_unique<HourlyPricing>(5.0)) {}
    ~Car() {}
};

class Bike : public Vehicle{
    public:
    string getType() override { return "B"; }
    Bike() : Vehicle(make_unique<HourlyPricing>(2.0)) {}
    ~Bike() {}
};

/*
Grid design:

Parking
   |
   +---- ParkingSpot(row ,col)
              |
              +---- unique_ptr<Vehicle>

    parking manages parkingSpot which in turn manages the vehicle occpying it
*/

class ParkingSpot{
    int row;
    int col;
    unique_ptr<Vehicle> vehicle;
    public:
    ParkingSpot(int r, int c) : row(r) , col(c) {}

    bool isOccupied() const {
        return vehicle != nullptr;
    }

    void park(unique_ptr<Vehicle> v){
        vehicle = move(v);
    }

    unique_ptr<Vehicle> removeVehicle(){
        return move(vehicle);
    }

    int getRow() const { return row; }

    int getCol() const { return col; }

    Vehicle* getVehicle() const { return vehicle.get(); }
};

class Parking{
    int rows;
    int cols;
    vector<vector<ParkingSpot>> parkingSpots;

    public:
    Parking(int r, int c){
        rows = r;
        cols = c;
        parkingSpots.resize(r);
        for(int i=0;i<r;i++){
            parkingSpots[i].reserve(c);
            for(int j=0;j<c;j++){
                parkingSpots[i].emplace_back(i,j);
            }
        }
    }

    bool isValidSpot(int r, int c) const { return (r>=0 && r<rows && c>=0 && c<cols); }

    pair<int,int> findVacantSpot() const {
        for(int i=0; i<rows; i++){
            for(int j=0; j<cols; j++){
                if(!parkingSpots[i][j].isOccupied())  return {i,j};
            }
        }
        return {-1,-1};
    }

    void parkVehicle(unique_ptr<Vehicle> v){
        auto [r,c] = findVacantSpot();

        if (r == -1) {
            cout << "Sorry, all spots occupied!";
            return;
        }

        v->startParking();
        parkingSpots[r][c].park(move(v));
        cout << "Your vehicle is now parked at " << r << "," << c << "! (remember the spot!)" << endl;

    }

    unique_ptr<Vehicle> vacateSpot(int r, int c){
        if(!isValidSpot(r,c) || !parkingSpots[r][c].isOccupied()) { return nullptr; }
        return parkingSpots[r][c].removeVehicle();
    }

    double calculateParkingFee (int r,int c) const {
        if(!isValidSpot(r,c)) {
            cout << "Invalid spot details! please choose a valid spot\n";
            return 0;
        }

        if(!parkingSpots[r][c].isOccupied()) {
            cout << "No vehicle is parked at this spot!\n";
            return 0;
        }

        return parkingSpots[r][c].getVehicle()->calculateParkingFee();
}

    void printParking() const {
        for(int i=0;i<rows;i++){
            for(int j=0;j<cols;j++){
                if(parkingSpots[i][j].isOccupied())
                cout << parkingSpots[i][j].getVehicle()->getType() << "  ";
                else cout << "." << "  ";
            }
            cout << endl;
        }
    }
};

int main(){
    Parking p(5,8);

    int i;

    while (true) {
    cout << "\n\n-----------------------------\n";
    cout << "Welcome to the Parking lot!\n";
    cout << "Please choose an option:\n";
    cout << "-----------------------------\n\n";
    cout << "\n1. Park a vehicle\n";
    cout << "2. Vacate a spot\n";
    cout << "3. Calculate parking fee\n";
    cout << "4. Print parking status\n";
    cout << "5. Exit\n";
    cout << "Enter choice: ";

    if (!(cin >> i)) {
        cin.clear();                     // Clear fail state
        cin.ignore(1000, '\n');          // Discard bad input
        cout << "Please enter a valid number.\n";
        continue;
    }

    switch (i) {
        case 1:
            cout << "Which type of vehicle do you want to park? (1 for Car, 2 for Bike): ";
            int type;
            if (!(cin >> type)) {
                cin.clear();                     // Clear fail state
                cin.ignore(1000, '\n');          // Discard bad input
                cout << "Please enter a valid number.\n";
                continue;
            }

            if(type == 1){
                //Car c (wrong way, destroyed after the scope ends, so we need to use dynamic memory allocation)
                p.parkVehicle(make_unique<Car>());
                cout << "Your car is parked!";
            }
            else if(type == 2){
                p.parkVehicle(make_unique<Bike>());
                cout << "Your bike is parked!";
            }
            else cout << "Invalid vehicle type! Please choose a valid option.\n";
            break;

        case 2:
            int x, y;
            cout << "Pls enter where your car is parked";
            cin >> x >> y;
            if(!p.isValidSpot(x, y)) {
                cout << "Invalid spot details! please choose a valid spot\n";
                break;
            }
            p.vacateSpot(x, y);
            cout << "Car vacated successfully.\n";
            break;

        case 3:
            cout << "Pls enter where your car is parked" << endl;
            cin >> x >> y;
            if(!p.isValidSpot(x, y)) {
                cout << "Invalid spot details! please choose a valid spot\n";
                break;
            }
            cout << "Parking fee: $" << p.calculateParkingFee(x, y) << '\n';
            break;

        case 4:
            cout << "Now showing parking lot status:\n";
            p.printParking();
            break;

        case 5:
            cout << "Thanks for visiting!" << endl;
            return 0;

        default:
            cout << "Hey, thats an invalid option! Please choose a valid option.\n";
        }
    }
}
