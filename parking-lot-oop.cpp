#include<iostream>
#include<chrono>
#include<vector>
#include<memory>
#include<optional>

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
    unique_ptr<PricingStrategy> pricing;

    public:
    Vehicle(unique_ptr<PricingStrategy> p) : pricing(move(p)) {}

    double calculateParkingFee(double hours) {
        return pricing->calculateFee(hours);
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


class ParkingTicket{
    Vehicle* vehicle;
    ParkingSpot* spot;
    chrono::time_point<chrono::system_clock> entryTime;
    optional<chrono::time_point<chrono::system_clock>> exitTime;
    public:
        ParkingTicket(Vehicle* v, ParkingSpot* ps) : vehicle(v), spot(ps), entryTime(chrono::system_clock::now()) {}

        Vehicle* getVehicle() const {
            return vehicle;
        }

        ParkingSpot* getParkingSpot() const {
            return spot;
        }

        void closeTicket() {
            exitTime = chrono::system_clock::now();
        }

        double getParkingDurationInHours() const {
            auto endTime = exitTime.value_or(chrono::system_clock::now());

            auto duration = chrono::duration_cast<chrono::seconds>(
                endTime - entryTime
            );

            return duration.count() / 3600.0;
        }

        double calculateFee() const {
              return vehicle->calculateParkingFee( getParkingDurationInHours() );
        }
};


class Parking{
    int rows;
    int cols;
    vector<vector<ParkingSpot>> parkingSpots;
    vector<unique_ptr<ParkingTicket>> tickets;

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

    bool isOccupied(int r, int c) const {
        return parkingSpots[r][c].isOccupied();
    }

    pair<int,int> findVacantSpot() const {
        for(int i=0; i<rows; i++){
            for(int j=0; j<cols; j++){
                if(!parkingSpots[i][j].isOccupied())  return {i,j};
            }
        }
        return {-1,-1};
    }

    ParkingTicket* parkVehicle(unique_ptr<Vehicle> v){
        auto [r,c] = findVacantSpot();

        if (r == -1) {
            return nullptr;
        }

        parkingSpots[r][c].park(move(v));

        Vehicle* parkedVehicle = parkingSpots[r][c].getVehicle();

        auto ticket = make_unique<ParkingTicket>(
            parkedVehicle,
            &parkingSpots[r][c]
        );

        ParkingTicket* ticketPtr = ticket.get();
        tickets.push_back(move(ticket));

        return ticketPtr;
    }

    unique_ptr<Vehicle> vacateSpot(int r, int c){
        if(!isValidSpot(r,c) || !parkingSpots[r][c].isOccupied()) { return nullptr; }
        return parkingSpots[r][c].removeVehicle();
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
            {
            cout << "Which type of vehicle do you want to park? (1 for Car, 2 for Bike): ";
            int type;
            if (!(cin >> type)) {
                cin.clear();                     // Clear fail state
                cin.ignore(1000, '\n');          // Discard bad input
                cout << "Please enter a valid number.\n";
                continue;
            }
            unique_ptr<Vehicle> vehicle;

            if(type == 1){
                vehicle = make_unique<Car>();
            }
            else if(type == 2){
                vehicle = make_unique<Bike>();
            }
            else {
                cout << "Invalid vehicle type! Please choose a valid option.\n";
                break;
            }

            ParkingTicket* ticket = p.parkVehicle(move(vehicle));

            if(!ticket)
                cout << "Cant park mate, lot full";
            else
            {
                cout << "Your vehicle is parked at "
                << ticket->getParkingSpot()->getRow() << ", "
                << ticket->getParkingSpot()->getCol() << endl;
            }
            break;
        }

        case 2:
            {
            int x, y;
            cout << "Pls enter where your car is parked";
            cin >> x >> y;
            if(!p.isValidSpot(x, y)) {
                cout << "Invalid spot details! please choose a valid spot\n";
                break;
            }
            auto vehicle = p.vacateSpot(x, y);

            if (!vehicle) {
                cout << "No vehicle is parked at this spot.\n";
                break;
            }

            cout << "Vehicle vacated successfully.\n";
            break;
            }
        case 3:
            {
                int x, y;

                cout << "Pls enter where your car is parked\n";
                cin >> x >> y;

                if(!p.isValidSpot(x, y)) {
                    cout << "Invalid spot details! please choose a valid spot\n";
                    break;
                }

                if(!p.isOccupied(x, y)) {
                    cout << "No vehicle is parked at this spot!\n";
                    break;
                }

                double fee = p.calculateParkingFee(x, y);
                cout << "Parking fee: $" << fee << '\n';

                break;
            }

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
