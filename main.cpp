#include <iostream>
#include <fstream>
#include <vector>
#include <string>

using namespace std;

const string CURRENT_DATE = "2025-08-23";

// Helper function to check date overlaps
bool dateOverlaps(const string& start1, const string& end1, const string& start2, const string& end2) {
    return !(end1 < start2 || end2 < start1);
}

// Staff class for authentication and roles
class Staff {
private:
    int id;
    string name;
    string role;
    string username;
    string password;

public:
    Staff(int i, const string& n, const string& r, const string& u, const string& p)
        : id(i), name(n), role(r), username(u), password(p) {}

    int getId() const { return id; }
    const string& getName() const { return name; }
    const string& getRole() const { return role; }
    const string& getUsername() const { return username; }
    const string& getPassword() const { return password; }
    void setPassword(const string& p) { password = p; }
};

// Abstract Room class
class Room {
protected:
    int roomNumber;
    string type;
    string status;

public:
    Room(int num, const string& t) : roomNumber(num), type(t), status("clean") {}
    virtual ~Room() {}
    virtual double getDailyRate() const = 0;
    int getRoomNumber() const { return roomNumber; }
    const string& getType() const { return type; }
    const string& getStatus() const { return status; }
    void setStatus(const string& s) { status = s; }
};

// Derived room types
class StandardRoom : public Room {
public:
    StandardRoom(int num) : Room(num, "Standard") {}
    double getDailyRate() const override { return 3000.0; }
};

class DeluxeRoom : public Room {
public:
    DeluxeRoom(int num) : Room(num, "Deluxe") {}
    double getDailyRate() const override { return 5000.0; }
};

class SuiteRoom : public Room {
public:
    SuiteRoom(int num) : Room(num, "Suite") {}
    double getDailyRate() const override { return 8000.0; }
};

class LuxuryVillaRoom : public Room {
public:
    LuxuryVillaRoom(int num) : Room(num, "Luxury Villa") {}
    double getDailyRate() const override { return 15000.0; }
};

// Guest class
class Guest {
private:
    string name;
    string preferences;
    int points;

public:
    Guest(const string& n, const string& pref, int pts) : name(n), preferences(pref), points(pts) {}
    const string& getName() const { return name; }
    const string& getPreferences() const { return preferences; }
    int getPoints() const { return points; }
    void addPoints(int p) { points += p; }
};

// Booking class
class Booking {
private:
    int roomNumber;
    string customerName;
    string checkInDate;
    string checkOutDate;
    int stayDuration;
    double basePrice;
    double extras;
    bool paid;
    bool cancelled;

public:
    Booking(int rn, const string& cn, const string& ci, const string& co, int sd, double bp)
        : roomNumber(rn), customerName(cn), checkInDate(ci), checkOutDate(co), stayDuration(sd),
          basePrice(bp), extras(0.0), paid(false), cancelled(false) {}

    int getRoomNumber() const { return roomNumber; }
    const string& getCustomerName() const { return customerName; }
    const string& getCheckInDate() const { return checkInDate; }
    const string& getCheckOutDate() const { return checkOutDate; }
    int getStayDuration() const { return stayDuration; }
    double getBasePrice() const { return basePrice; }
    double getExtras() const { return extras; }
    double getTotalPrice() const { return basePrice + extras; }
    bool isPaid() const { return paid; }
    void setPaid(bool p) { paid = p; }
    void addExtras(double e) { extras += e; }
    bool isCancelled() const { return cancelled; }
    void cancel() { cancelled = true; }
};

// Global dynamic collections
vector<Staff*> staffList;
vector<Room*> rooms;
vector<Guest*> guests;
vector<Booking*> bookings;

// File handling functions
void loadStaff() {
    ifstream file("staff.txt");
    if (!file.is_open()) {
        string adminPass;
        cout << "First-time setup: Enter new admin password: ";
        cin >> adminPass;
        staffList.push_back(new Staff(1, "Admin", "admin", "admin", adminPass));
        ofstream outFile("staff.txt");
        if (outFile.is_open()) {
            outFile << "1,Admin,admin,admin," << adminPass << endl;
            outFile.close();
        }
        return;
    }
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        size_t pos3 = line.find(',', pos2 + 1);
        size_t pos4 = line.find(',', pos3 + 1);
        int id = stoi(line.substr(0, pos1));
        string name = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string role = line.substr(pos2 + 1, pos3 - pos2 - 1);
        string user = line.substr(pos3 + 1, pos4 - pos3 - 1);
        string pass = line.substr(pos4 + 1);
        staffList.push_back(new Staff(id, name, role, user, pass));
    }
    file.close();
}

void saveStaff() {
    ofstream file("staff.txt");
    if (!file.is_open()) {
        cout << "Error saving staff.txt" << endl;
        return;
    }
    for (const auto& s : staffList) {
        file << s->getId() << "," << s->getName() << "," << s->getRole() << "," << s->getUsername() << "," << s->getPassword() << endl;
    }
    file.close();
}

void loadRooms() {
    ifstream file("rooms.txt");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        int num = stoi(line.substr(0, pos1));
        string type = line.substr(pos1 + 1, pos2 - pos1 - 1);
        string status = line.substr(pos2 + 1);
        Room* r = nullptr;
        if (type == "Standard") r = new StandardRoom(num);
        else if (type == "Deluxe") r = new DeluxeRoom(num);
        else if (type == "Suite") r = new SuiteRoom(num);
        else if (type == "Luxury Villa") r = new LuxuryVillaRoom(num);
        if (r) {
            r->setStatus(status);
            rooms.push_back(r);
        }
    }
    file.close();
}

void saveRooms() {
    ofstream file("rooms.txt");
    if (!file.is_open()) {
        cout << "Error saving rooms.txt" << endl;
        return;
    }
    for (const auto& r : rooms) {
        file << r->getRoomNumber() << "," << r->getType() << "," << r->getStatus() << endl;
    }
    file.close();
}

void loadGuests() {
    ifstream file("guests.txt");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        size_t pos1 = line.find(',');
        size_t pos2 = line.find(',', pos1 + 1);
        string name = line.substr(0, pos1);
        string pref = line.substr(pos1 + 1, pos2 - pos1 - 1);
        int pts = stoi(line.substr(pos2 + 1));
        guests.push_back(new Guest(name, pref, pts));
    }
    file.close();
}

void saveGuests() {
    ofstream file("guests.txt");
    if (!file.is_open()) {
        cout << "Error saving guests.txt" << endl;
        return;
    }
    for (const auto& g : guests) {
        file << g->getName() << "," << g->getPreferences() << "," << g->getPoints() << endl;
    }
    file.close();
}

void loadBookings() {
    ifstream file("bookings.txt");
    if (!file.is_open()) return;
    string line;
    while (getline(file, line)) {
        size_t pos[8];
        pos[0] = line.find(',');
        for (int i = 1; i < 8; ++i) pos[i] = line.find(',', pos[i - 1] + 1);
        int rn = stoi(line.substr(0, pos[0]));
        string cn = line.substr(pos[0] + 1, pos[1] - pos[0] - 1);
        string ci = line.substr(pos[1] + 1, pos[2] - pos[1] - 1);
        string co = line.substr(pos[2] + 1, pos[3] - pos[2] - 1);
        int sd = stoi(line.substr(pos[3] + 1, pos[4] - pos[3] - 1));
        double bp = stod(line.substr(pos[4] + 1, pos[5] - pos[4] - 1));
        double ex = stod(line.substr(pos[5] + 1, pos[6] - pos[5] - 1));
        bool p = (line.substr(pos[6] + 1, pos[7] - pos[6] - 1) == "1");
        bool c = (line.substr(pos[7] + 1) == "1");
        Booking* b = new Booking(rn, cn, ci, co, sd, bp);
        b->addExtras(ex);
        b->setPaid(p);
        if (c) b->cancel();
        bookings.push_back(b);
    }
    file.close();
}

void saveBookings() {
    ofstream file("bookings.txt");
    if (!file.is_open()) {
        cout << "Error saving bookings.txt" << endl;
        return;
    }
    for (const auto& b : bookings) {
        file << b->getRoomNumber() << "," << b->getCustomerName() << "," << b->getCheckInDate() << ","
             << b->getCheckOutDate() << "," << b->getStayDuration() << "," << b->getBasePrice() << ","
             << b->getExtras() << "," << (b->isPaid() ? "1" : "0") << "," << (b->isCancelled() ? "1" : "0") << endl;
    }
    file.close();
}

// Finder functions
Room* findRoom(int num) {
    for (auto r : rooms) {
        if (r->getRoomNumber() == num) return r;
    }
    return nullptr;
}

Guest* findGuest(const string& name) {
    for (auto g : guests) {
        if (g->getName() == name) return g;
    }
    return nullptr;
}

Booking* findActiveBooking(int rn) {
    for (auto b : bookings) {
        if (b->getRoomNumber() == rn && !b->isCancelled() &&
            b->getCheckInDate() <= CURRENT_DATE && b->getCheckOutDate() >= CURRENT_DATE) {
            return b;
        }
    }
    return nullptr;
}

vector<Booking*> findBookingsByCustomer(const string& name) {
    vector<Booking*> res;
    for (auto b : bookings) {
        if (b->getCustomerName() == name && !b->isCancelled()) res.push_back(b);
    }
    return res;
}

Staff* findStaff(const string& username) {
    for (auto s : staffList) {
        if (s->getUsername() == username) return s;
    }
    return nullptr;
}

// Availability check
bool isRoomAvailable(int num, const string& start, const string& end) {
    for (const auto& b : bookings) {
        if (b->getRoomNumber() == num && !b->isCancelled() &&
            dateOverlaps(start, end, b->getCheckInDate(), b->getCheckOutDate())) {
            return false;
        }
    }
    return true;
}

// Authentication
pair<bool, string> login() {
    cout << "\nLogin as:\n1. Admin\n2. Receptionist\n3. Housekeeper\n4. Manager\n5. Concierge\n6. Exit\n";
    int choice;
    cin >> choice;
    if (choice == 6) return {false, "exit"};
    string role;
    switch (choice) {
        case 1: role = "admin"; break;
        case 2: role = "receptionist"; break;
        case 3: role = "housekeeper"; break;
        case 4: role = "manager"; break;
        case 5: role = "concierge"; break;
        default: cout << "Invalid choice" << endl; return {false, ""};
    }
    string user, pass;
    cout << "Username: ";
    cin >> user;
    cout << "Password: ";
    cin >> pass;
    for (const auto& s : staffList) {
        if (s->getUsername() == user && s->getPassword() == pass && s->getRole() == role) {
            return {true, role};
        }
    }
    cout << "Invalid credentials" << endl;
    return {false, ""};
}

// Main function
int main() {
    // Load data
    loadStaff();
    loadRooms();
    if (rooms.empty()) {
        for (int i = 1; i <= 5; ++i) {
            rooms.push_back(new StandardRoom(100 + i));
            rooms.push_back(new DeluxeRoom(200 + i));
            rooms.push_back(new SuiteRoom(300 + i));
            rooms.push_back(new LuxuryVillaRoom(400 + i));
        }
        saveRooms();
    }
    loadGuests();
    loadBookings();

    // Simulate real-time: Check for overdue check-outs
    for (auto b : bookings) {
        if (!b->isCancelled() && b->getCheckOutDate() < CURRENT_DATE) {
            Room* r = findRoom(b->getRoomNumber());
            if (r) r->setStatus("dirty");
        }
    }
    saveRooms();

    cout << "Welcome to Hotel Management System" << endl;
    bool programRunning = true;
    while (programRunning) {
        auto auth = login();
        if (auth.second == "exit") {
            programRunning = false;
            continue;
        }
        if (!auth.first) {
            continue;
        }
        string role = auth.second;
        bool sessionRunning = true;
        while (sessionRunning) {
            if (role == "admin") {
                cout << "\nAdmin Menu:\n1. Add/Modify Staff\n2. View Financial Reports\n3. Change Admin Password\n4. Logout\n";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    int id;
                    string name, r, u, p;
                    cout << "Staff ID: ";
                    cin >> id;
                    cout << "Name: ";
                    cin >> name;
                    cout << "Role (admin/receptionist/manager/housekeeper/concierge): ";
                    cin >> r;
                    cout << "Username: ";
                    cin >> u;
                    cout << "Password: ";
                    cin >> p;
                    staffList.push_back(new Staff(id, name, r, u, p));
                    saveStaff();
                } else if (choice == 2) {
                    double collected = 0.0;
                    double pendingAmount = 0.0;
                    double refunds = 0.0;
                    for (const auto& b : bookings) {
                        if (b->isCancelled()) {
                            refunds += b->getTotalPrice();
                        } else if (b->isPaid()) {
                            collected += b->getTotalPrice();
                        } else {
                            pendingAmount += b->getTotalPrice();
                        }
                    }
                    cout << "Total Bills Collected: Rs " << collected << endl;
                    cout << "Pending Bills: Rs " << pendingAmount << endl;
                    cout << "Refunds/Returns Issued: Rs " << refunds << endl;
                } else if (choice == 3) {
                    string newPass;
                    cout << "Enter new admin password: ";
                    cin >> newPass;
                    Staff* admin = findStaff("admin");
                    if (admin && admin->getRole() == "admin") {
                        admin->setPassword(newPass);
                        saveStaff();
                        cout << "Admin password updated successfully." << endl;
                    } else {
                        cout << "Admin account not found." << endl;
                    }
                } else if (choice == 4) {
                    sessionRunning = false;
                }
            } else if (role == "receptionist") {
                cout << "\nReceptionist Menu:\n1. Book Room\n2. Check Out\n3. Cancel Booking\n4. Add Service/Extras\n5. View Bookings\n6. Logout\n";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    string type, cname, pref, cinDate, coutDate;
                    int duration;
                    cout << "Room Type (Standard/Deluxe/Suite/Luxury Villa): ";
                    cin >> type;
                    cout << "Customer Name: ";
                    cin >> cname;
                    cout << "Preferences (e.g., non-smoking, sea view): ";
                    cin >> pref;
                    cout << "Check-In Date (YYYY-MM-DD): ";
                    cin >> cinDate;
                    cout << "Check-Out Date (YYYY-MM-DD): ";
                    cin >> coutDate;
                    cout << "Stay Duration (days): ";
                    cin >> duration;
                    Room* selected = nullptr;
                    double rate = 0.0;
                    for (const auto& r : rooms) {
                        if (r->getType() == type && r->getStatus() == "clean" && isRoomAvailable(r->getRoomNumber(), cinDate, coutDate)) {
                            selected = r;
                            rate = r->getDailyRate();
                            break;
                        }
                    }
                    if (!selected) {
                        cout << "No available room of this type." << endl;
                        continue;
                    }
                    Guest* g = findGuest(cname);
                    if (!g) {
                        g = new Guest(cname, pref, 0);
                        guests.push_back(g);
                    }
                    double discount = (g->getPoints() >= 5) ? 0.1 * rate * duration : 0.0;
                    double tp = rate * duration - discount;
                    bookings.push_back(new Booking(selected->getRoomNumber(), cname, cinDate, coutDate, duration, tp));
                    saveBookings();
                    saveGuests();
                    cout << "Room booked successfully. Room Number: " << selected->getRoomNumber() << ". Total: Rs " << tp << endl;
                } else if (choice == 2) {
                    int rn;
                    cout << "Room Number: ";
                    cin >> rn;
                    Booking* b = findActiveBooking(rn);
                    if (!b) {
                        cout << "No active booking for this room." << endl;
                        continue;
                    }
                    cout << "Total Bill: Rs " << b->getTotalPrice() << endl;
                    string pay;
                    cout << "Paid (yes/no)? ";
                    cin >> pay;
                    if (pay == "yes") b->setPaid(true);
                    Room* r = findRoom(rn);
                    if (r) r->setStatus("dirty");
                    Guest* g = findGuest(b->getCustomerName());
                    if (g) g->addPoints(b->getStayDuration());
                    saveBookings();
                    saveRooms();
                    saveGuests();
                    cout << "Check-out completed." << endl;
                } else if (choice == 3) {
                    string cname;
                    cout << "Customer Name: ";
                    cin >> cname;
                    auto custBookings = findBookingsByCustomer(cname);
                    if (custBookings.empty()) {
                        cout << "No bookings found." << endl;
                        continue;
                    }
                    cout << "Select booking to cancel (index):\n";
                    for (size_t i = 0; i < custBookings.size(); ++i) {
                        cout << i + 1 << ". Room " << custBookings[i]->getRoomNumber() << " from " << custBookings[i]->getCheckInDate() << endl;
                    }
                    int idx;
                    cin >> idx;
                    if (idx < 1 || idx > static_cast<int>(custBookings.size())) continue;
                    Booking* b = custBookings[idx - 1];
                    if (b->getCheckInDate() <= CURRENT_DATE) {
                        cout << "Cannot cancel started booking." << endl;
                        continue;
                    }
                    b->cancel();
                    saveBookings();
                    cout << "Booking cancelled." << endl;
                } else if (choice == 4) {
                    int rn;
                    cout << "Room Number: ";
                    cin >> rn;
                    Booking* b = findActiveBooking(rn);
                    if (!b) {
                        cout << "No active booking." << endl;
                        continue;
                    }
                    double cost;
                    cout << "Service Cost: ";
                    cin >> cost;
                    b->addExtras(cost);
                    saveBookings();
                    cout << "Service added." << endl;
                } else if (choice == 5) {
                    for (const auto& b : bookings) {
                        if (!b->isCancelled()) {
                            cout << "Room " << b->getRoomNumber() << " - " << b->getCustomerName() << " (" << b->getCheckInDate() << " to " << b->getCheckOutDate() << ") Paid: " << (b->isPaid() ? "Yes" : "No") << endl;
                        }
                    }
                } else if (choice == 6) {
                    sessionRunning = false;
                }
            } else if (role == "housekeeper") {
                cout << "\nHousekeeper Menu:\n1. View Rooms Needing Attention\n2. Clean Room\n3. Logout\n";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    for (const auto& r : rooms) {
                        if (r->getStatus() != "clean") {
                            cout << "Room " << r->getRoomNumber() << " Status: " << r->getStatus() << endl;
                        }
                    }
                } else if (choice == 2) {
                    int rn;
                    cout << "Room Number: ";
                    cin >> rn;
                    Room* r = findRoom(rn);
                    if (r && r->getStatus() == "dirty") {
                        r->setStatus("clean");
                        saveRooms();
                        cout << "Room cleaned." << endl;
                    } else {
                        cout << "Invalid or not dirty." << endl;
                    }
                } else if (choice == 3) {
                    sessionRunning = false;
                }
            } else if (role == "manager") {
                cout << "\nManager Menu:\n1. View Occupancy Report\n2. View Bookings\n3. View Staff\n4. Logout\n";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    int active = 0;
                    for (const auto& r : rooms) {
                        if (findActiveBooking(r->getRoomNumber())) ++active;
                    }
                    double occupancy = static_cast<double>(active) / rooms.size() * 100.0;
                    cout << "Current Occupancy: " << occupancy << "% (" << active << "/" << rooms.size() << ")" << endl;
                } else if (choice == 2) {
                    for (const auto& b : bookings) {
                        if (!b->isCancelled()) {
                            cout << "Room " << b->getRoomNumber() << " - " << b->getCustomerName() << " (" << b->getCheckInDate() << " to " << b->getCheckOutDate() << ")" << endl;
                        }
                    }
                } else if (choice == 3) {
                    for (const auto& s : staffList) {
                        cout << "ID " << s->getId() << " - " << s->getName() << " (" << s->getRole() << ")" << endl;
                    }
                } else if (choice == 4) {
                    sessionRunning = false;
                }
            } else if (role == "concierge") {
                cout << "\nConcierge Menu:\n1. Add Guest Service\n2. View Guest Preferences\n3. Logout\n";
                int choice;
                cin >> choice;
                if (choice == 1) {
                    int rn;
                    cout << "Room Number: ";
                    cin >> rn;
                    Booking* b = findActiveBooking(rn);
                    if (!b) {
                        cout << "No active booking." << endl;
                        continue;
                    }
                    double cost;
                    cout << "Service Cost: ";
                    cin >> cost;
                    b->addExtras(cost);
                    saveBookings();
                    cout << "Service added." << endl;
                } else if (choice == 2) {
                    string cname;
                    cout << "Customer Name: ";
                    cin >> cname;
                    Guest* g = findGuest(cname);
                    if (g) {
                        cout << "Preferences: " << g->getPreferences() << " Points: " << g->getPoints() << endl;
                    } else {
                        cout << "Guest not found." << endl;
                    }
                } else if (choice == 3) {
                    sessionRunning = false;
                }
            }
        }
    }

    // Save all data
    saveStaff();
    saveRooms();
    saveGuests();
    saveBookings();

    // Clean up dynamic memory
    for (auto s : staffList) delete s;
    staffList.clear();
    for (auto r : rooms) delete r;
    rooms.clear();
    for (auto g : guests) delete g;
    guests.clear();
    for (auto b : bookings) delete b;
    bookings.clear();

    return 0;
}