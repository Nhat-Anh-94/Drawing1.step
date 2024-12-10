#include "G4VUserDetectorConstruction.hh"
#include "G4NistManager.hh"
#include "G4TessellatedSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4GeneralParticleSource.hh"

#include "G4VModularPhysicsList.hh"
#include "G4Gamma.hh"

#include "G4RunManager.hh"
#include "G4VisExecutive.hh"

#include "G4SystemOfUnits.hh"

// Qt headers
#include <QApplication>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>

// CADMESH //
#include "CADMesh.hh"

// DetectorConstruction class (for Geant4 simulation)
class DetectorConstruction : public G4VUserDetectorConstruction
{
public:
    G4VPhysicalVolume* Construct() override
    {
        // Materials //
        G4NistManager* nist_manager = G4NistManager::Instance();
        G4Material* air = nist_manager->FindOrBuildMaterial("G4_AIR");
        G4Material* water = nist_manager->FindOrBuildMaterial("G4_WATER");

        // World //
        auto world_solid = new G4Box("world_solid", 2000 * mm, 200 * cm, 2 * m);
        auto world_logical = new G4LogicalVolume(world_solid, air, "world_logical", nullptr, nullptr, nullptr);
        world_logical->SetVisAttributes(G4VisAttributes::GetInvisible());

        auto world_physical = new G4PVPlacement(nullptr, G4ThreeVector(), world_logical, "world_physical", nullptr, false, 0);

        ////////////////////
        // CADMesh :: STEP //
        ////////////////////
        auto step_mesh = CADMesh::TessellatedMesh::FromSTL("./Drawing2.stl");  // Read the STL file
        step_mesh->SetScale(200);  // Set model scale
        step_mesh->SetOffset(G4ThreeVector(0, 0, 0));  // Set offset position
        auto step_rotation = new G4RotationMatrix();
        step_rotation->rotateY(45 * deg);

        auto step_logical = new G4LogicalVolume(step_mesh->GetSolid(), water, "step_logical", nullptr, nullptr, nullptr);
        new G4PVPlacement(step_rotation, G4ThreeVector(), step_logical, "step_physical", world_logical, false, 0);

        return world_physical;
    }
};

// PrimaryGeneratorAction class (for particle source)
class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
private:
    G4GeneralParticleSource* particle_gun;

public:
    PrimaryGeneratorAction()
    {
        particle_gun = new G4GeneralParticleSource();
        particle_gun->SetParticleDefinition(G4Gamma::Definition());
    }

    ~PrimaryGeneratorAction() override
    {
        delete particle_gun;
    }

    void GeneratePrimaries(G4Event* event) override
    {
        particle_gun->GeneratePrimaryVertex(event);
    }
};

// MainWindow class (Qt GUI for Geant4 simulation)
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow()
    {
        // Layout and buttons
        QVBoxLayout* layout = new QVBoxLayout(this);

        // Start Simulation Button
        QPushButton* startButton = new QPushButton("Start Simulation", this);
        layout->addWidget(startButton);

        // Stop Simulation Button (New feature)
        QPushButton* stopButton = new QPushButton("Stop Simulation", this);
        layout->addWidget(stopButton);

        // Message to show the user when the simulation is running
        statusLabel = new QLabel("Simulation is not running.", this);
        layout->addWidget(statusLabel);

        // Connect signals and slots
        connect(startButton, &QPushButton::clicked, this, &MainWindow::startSimulation);
        connect(stopButton, &QPushButton::clicked, this, &MainWindow::stopSimulation);
    }

public slots:
    void startSimulation()
    {
        // Initialize Geant4 RunManager
        run_manager = new G4RunManager();

        // Set up DetectorConstruction
        auto detector_construction = new DetectorConstruction();
        run_manager->SetUserInitialization(detector_construction);

        // Set up Physics List
        auto physics_list = new G4VModularPhysicsList();
        run_manager->SetUserInitialization(physics_list);

        // Set up Primary Generator Action
        auto primary_generator_action = new PrimaryGeneratorAction();
        run_manager->SetUserAction(primary_generator_action);

        run_manager->Initialize();  // Initialize simulation

        // Initialize Visualization Manager
        vis_manager = new G4VisExecutive();
        vis_manager->Initialize();

        // Update status label
        statusLabel->setText("Simulation is running...");

        // Run the simulation with a number of events (can be adjusted)
        run_manager->BeamOn(1000);  // Run 1000 events

        // Update status label
        statusLabel->setText("Simulation completed.");

        delete vis_manager;
        delete run_manager;
    }

    void stopSimulation()
    {
        // Logic to stop the simulation (a simple simulation stop mechanism)
        if (run_manager) {
            run_manager->BeamOn(0);  // Stop the simulation after 0 events
            statusLabel->setText("Simulation stopped.");
        }
        else {
            QMessageBox::warning(this, "Error", "Simulation has not started yet.");
        }
    }

private:
    QLabel* statusLabel;  // Label to show simulation status
    G4RunManager* run_manager = nullptr;  // Geant4 run manager
    G4VisExecutive* vis_manager = nullptr;  // Visualization manager
};

int main(int argc, char** argv)
{
    QApplication app(argc, argv);  // Initialize Qt application

    // Create main window
    MainWindow window;
    window.setWindowTitle("Geant4 Simulation with Qt");
    window.resize(300, 200);  // Resize window
    window.show();

    return app.exec();  // Start Qt application event loop
}

