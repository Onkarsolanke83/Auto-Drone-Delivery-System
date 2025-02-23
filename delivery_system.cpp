#include <iostream>
#include <vector>
#include <queue>
#include <limits>
#include <algorithm>
#include <unordered_set>
#include <string>

class Graph {
public:
    struct Edge {
        int to;
        double weight;
        Edge(int t, double w) : to(t), weight(w) {}
    };

    std::vector<std::vector<Edge>> adj;
    int V;

    Graph(int vertices) : V(vertices) {
        adj.resize(V);
    }

    void addEdge(int from, int to, double weight) {
        for (const auto& edge : adj[from]) {
            if (edge.to == to) return;
        }
        adj[from].emplace_back(to, weight);
        adj[to].emplace_back(from, weight);
    }
};

class DeliveryTree {
public:
    struct Node {
        int location;
        std::vector<Node*> children;
        Node(int loc) : location(loc) {}
    };

    Node* root;

    DeliveryTree(int rootLocation) : root(new Node(rootLocation)) {}

    void addDelivery(int from, int to) {
        Node* parent = findNode(root, from);
        if (parent) {
            parent->children.push_back(new Node(to));
        }
    }

private:
    Node* findNode(Node* node, int location) {
        if (node->location == location) return node;
        for (Node* child : node->children) {
            Node* found = findNode(child, location);
            if (found) return found;
        }
        return nullptr;
    }
};

class Drone {
public:
    int id;
    int currentLocation;
    double battery;
    bool hasDelivered;
    std::string name;
    std::string status; // Status of the drone

    Drone(int i, const std::string& droneName, int loc, double bat)
        : id(i), name(droneName), currentLocation(loc), battery(bat), hasDelivered(false), status("Available") {}
};

class DeliverySystem {
private:
    Graph cityMap;
    DeliveryTree schedule;
    std::vector<Drone> drones;

public:
    DeliverySystem(int numLocations, int numDrones) : cityMap(numLocations), schedule(0) {
        for (int i = 0; i < numDrones; ++i) {
            drones.emplace_back(i, "Drone_" + std::to_string(i + 1), 0, 100.0);
        }
    }

    std::vector<int> findShortestPath(int start, int end) {
        std::vector<double> dist(cityMap.V, std::numeric_limits<double>::infinity());
        std::vector<int> prev(cityMap.V, -1);
        std::priority_queue<std::pair<double, int>, std::vector<std::pair<double, int>>, std::greater<>> pq;

        dist[start] = 0;
        pq.push({0, start});

        while (!pq.empty()) {
            int u = pq.top().second;
            pq.pop();

            if (u == end) break;

            for (const auto& edge : cityMap.adj[u]) {
                int v = edge.to;
                double weight = edge.weight;
                if (dist[u] + weight < dist[v]) {
                    dist[v] = dist[u] + weight;
                    prev[v] = u;
                    pq.push({dist[v], v});
                }
            }
        }

        std::vector<int> path;
        for (int v = end; v != -1; v = prev[v]) {
            path.push_back(v);
        }
        std::reverse(path.begin(), path.end());
        return path;
    }

    void assignDeliveries(const std::vector<std::pair<int, int>>& deliveries) {
        for (const auto& delivery : deliveries) {
            int from = delivery.first;
            int to = delivery.second;
            schedule.addDelivery(from, to);

            Drone* bestDrone = nullptr;
            double minDistance = std::numeric_limits<double>::infinity();
            for (auto& drone : drones) {
                if (drone.status != "Available") continue;
                double distance = calculateDistance(drone.currentLocation, from);
                if (distance < minDistance && drone.battery > distance * 2) {
                    minDistance = distance;
                    bestDrone = &drone;
                }
            }

            if (bestDrone) {
                bestDrone->status = "Delivering"; // Update drone status
                bestDrone->battery -= minDistance;
                bestDrone->currentLocation = to;
                std::cout << "Assigned delivery from " << from << " to " << to << " to " << bestDrone->name << std::endl;
            } else {
                std::cout << "No available drone for delivery from " << from << " to " << to << std::endl;
            }
        }
    }

    double calculateDistance(int loc1, int loc2) {
        std::vector<int> path = findShortestPath(loc1, loc2);
        double distance = 0;
        for (size_t i = 1; i < path.size(); ++i) {
            for (const auto& edge : cityMap.adj[path[i - 1]]) {
                if (edge.to == path[i]) {
                    distance += edge.weight;
                    break;
                }
            }
        }
        return distance;
    }

    void simulateDeliveries() {
        std::cout << "Starting delivery simulation..." << std::endl;
        for (auto& drone : drones) {
            if (drone.status == "Delivering") {
                std::cout << drone.name << " completed its deliveries and returned to hub." << std::endl;
                drone.currentLocation = 0; 
                drone.battery = 100.0; 
                drone.status = "Available"; // Reset status
            }
        }
        std::cout << "All deliveries completed." << std::endl;
    }

    void takeInput(int numEdges, int numDeliveries) {
        std::cout << "Enter the edges (from to weight):" << std::endl;
        for (int i = 0; i < numEdges; ++i) {
            int from, to;
            double weight;
            std::cin >> from >> to >> weight;
            cityMap.addEdge(from, to, weight);
        }

        std::cout << "Enter the deliveries (from to):" << std::endl;
        std::vector<std::pair<int, int>> deliveries;
        for (int i = 0; i < numDeliveries; ++i) {
            int from, to;
            std::cin >> from >> to;
            deliveries.emplace_back(from, to);
        }

        assignDeliveries(deliveries);
    }

    void displayDrones() {
        for (const auto& drone : drones) {
            std::cout << drone.name << " - Status: " << drone.status << ", Battery: " << drone.battery << std::endl;
        }
    }

    void addDrone(const std::string& name) {
        int newId = drones.size();
        drones.emplace_back(newId, name, 0, 100.0);
        std::cout << "Added " << name << " as Drone_" << newId + 1 << std::endl;
    }

    void removeDrone(int id) {
        if (id < 0 || id >= drones.size()) {
            std::cout << "Invalid Drone ID!" << std::endl;
            return;
        }
        drones.erase(drones.begin() + id);
        std::cout << "Removed Drone_" << id + 1 << std::endl;
    }
};

class Employee {
public:
    std::string name;
    std::string id;
    std::string password;

    Employee(const std::string& name, const std::string& id, const std::string& pass)
        : name(name), id(id), password(pass) {}
};

class Admin {
private:
    std::vector<Employee> employees;

public:
    void addEmployee(const std::string& name, const std::string& id, const std::string& password) {
        employees.emplace_back(name, id, password);
        std::cout << name << " has been registered." << std::endl;
    }

    bool authenticateEmployee(const std::string& id, const std::string& password) {
        for (const auto& emp : employees) {
            if (emp.id == id && emp.password == password) {
                return true;
            }
        }
        return false;
    }

    void displayEmployees() {
        std::cout << "Registered Employees:" << std::endl;
        for (const auto& emp : employees) {
            std::cout << emp.name << std::endl;
        }
    }
};

void adminMenu(DeliverySystem& system, Admin& admin) {
    int choice;
    do {
        std::cout << "\n--- Admin Menu ---\n";
        std::cout << "1. Display Drones\n";
        std::cout << "2. Add Drone\n";
        std::cout << "3. Remove Drone\n";
        std::cout << "4. View Registered Employees\n";
        std::cout << "5. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1:
                system.displayDrones();
                break;
            case 2: {
                std::string droneName;
                std::cout << "Enter Drone Name: ";
                std::cin >> droneName;
                system.addDrone(droneName);
                break;
            }
            case 3: {
                int droneId;
                std::cout << "Enter Drone ID to remove: ";
                std::cin >> droneId;
                system.removeDrone(droneId);
                break;
            }
            case 4:
                admin.displayEmployees();
                break;
            case 5:
                std::cout << "Exiting Admin Menu.\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 5);
}

void employeeMenu(DeliverySystem& system) {
    int choice;
    do {
        std::cout << "\n--- Employee Menu ---\n";
        std::cout << "1. Send Drone for Delivery\n";
        std::cout << "2. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> choice;

        switch (choice) {
            case 1: {
                int from, to;
                std::cout << "Enter delivery locations (from and to): ";
                std::cin >> from >> to;
                std::cout << "Sending drone for delivery from " << from << " to " << to << "...\n";
                // Call the function to assign delivery directly
                system.assignDeliveries({{from, to}});
                break;
            }
            case 2:
                std::cout << "Exiting Employee Menu.\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    } while (choice != 2);
}

int main() {
    DeliverySystem system(10, 10);
    Admin admin;

    // Register initial employees with IDs and passwords
    admin.addEmployee("Employee_1", "emp1", "pass1");
    admin.addEmployee("Employee_2", "emp2", "pass2");
    admin.addEmployee("Employee_3", "emp3", "pass3");

    // Main Menu
    int mainChoice;
    do {
        std::cout << "\n--- Main Menu ---\n";
        std::cout << "1. Admin Login\n";
        std::cout << "2. Employee Login\n";
        std::cout << "3. Exit\n";
        std::cout << "Choose an option: ";
        std::cin >> mainChoice;

        switch (mainChoice) {
            case 1: {
                std::string adminId, adminPassword;
                std::cout << "Enter Admin ID: ";
                std::cin >> adminId;
                std::cout << "Enter Admin Password: ";
                std::cin >> adminPassword;

                if (adminId == "Admin" && adminPassword == "DSA@123") {
                    adminMenu(system, admin);
                } else {
                    std::cout << "Invalid Admin ID or Password." << std::endl;
                }
                break;
            }
            case 2: {
                std::string inputID, inputPassword;
                std::cout << "Enter Employee ID: ";
                std::cin >> inputID;
                std::cout << "Enter Password: ";
                std::cin >> inputPassword;

                if (admin.authenticateEmployee(inputID, inputPassword)) {
                    employeeMenu(system);
                } else {
                    std::cout << "Invalid Employee ID or Password." << std::endl;
                }
                break;
            }
            case 3:
                std::cout << "Exiting program.\n";
                break;
            default:
                std::cout << "Invalid option. Please try again.\n";
        }
    } while (mainChoice != 3);

    return 0;
}