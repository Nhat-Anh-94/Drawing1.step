#include "DetectorConstruction.hh"
#include "G4Orb.hh"
#include "G4Box.hh"
#include "G4Cons.hh"
#include "G4LogicalVolume.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4SystemOfUnits.hh"
#include "G4Trd.hh"
#include "CADMesh.hh"

namespace B1
{

    G4VPhysicalVolume* DetectorConstruction::Construct()
    {
        // Get nist material manager
        G4NistManager* nist = G4NistManager::Instance();

        // Envelope parameters
        G4double env_sizeXY = 20 * cm, env_sizeZ = 30 * cm;
        G4Material* env_mat = nist->FindOrBuildMaterial("G4_WATER_VAPOR");

        // Option to switch on/off checking of volumes overlaps
        G4bool checkOverlaps = true;

        // World
        G4double world_sizeXY = 1.2 * env_sizeXY;
        G4double world_sizeZ = 1.2 * env_sizeZ;
        G4Material* world_mat = nist->FindOrBuildMaterial("G4_AIR");

        auto solidWorld = new G4Box("World", 0.5 * world_sizeXY, 0.5 * world_sizeXY, 0.5 * world_sizeZ);
        auto logicWorld = new G4LogicalVolume(solidWorld, world_mat, "World");
        auto physWorld = new G4PVPlacement(nullptr, G4ThreeVector(), logicWorld, "World", nullptr, false, 0, checkOverlaps);

        // Envelope
        auto solidEnv = new G4Box("Envelope", 0.5 * env_sizeXY, 0.5 * env_sizeXY, 0.5 * env_sizeZ);
        auto logicEnv = new G4LogicalVolume(solidEnv, env_mat, "Envelope");
        new G4PVPlacement(nullptr, G4ThreeVector(), logicEnv, "Envelope", logicWorld, false, 0, checkOverlaps);

        G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
        G4ThreeVector pos1 = G4ThreeVector(0, 2 * cm, -7 * cm);
        /*// Shape 1 (Sphere)
        G4Material* shape1_mat = nist->FindOrBuildMaterial("G4_A-150_TISSUE");
        G4ThreeVector pos1 = G4ThreeVector(0, 2 * cm, -7 * cm);
        auto solidShape1 = new G4Orb("Shape1", 2 * cm);
        auto logicShape1 = new G4LogicalVolume(solidShape1, shape1_mat, "Shape1");
        new G4PVPlacement(nullptr, pos1, logicShape1, "Shape1", logicEnv, false, 0, checkOverlaps);*/

        // Shape 2 (Sphere)
        G4Material* shape2_mat = nist->FindOrBuildMaterial("G4_BONE_COMPACT_ICRU");
        G4ThreeVector pos2 = G4ThreeVector(0, -1 * cm, 7 * cm);
        auto solidShape2 = new G4Orb("Shape2", 5 * cm);
        auto logicShape2 = new G4LogicalVolume(solidShape2, shape2_mat, "Shape2");
        new G4PVPlacement(nullptr, pos2, logicShape2, "Shape2", logicEnv, false, 0, checkOverlaps);

        ////////////////////
        // CADMesh :: STEP //
        ////////////////////
        auto step_mesh = CADMesh::TessellatedMesh::FromSTL("./Drawing2.stl");  // 
        step_mesh->SetScale(5);  // 
        //step_mesh->SetOffset(G4ThreeVector(0, 2 * cm, -7 * cm));  // 
        auto step_rotation = new G4RotationMatrix();
        step_rotation->rotateY(45 * deg);

        auto step_logical = new G4LogicalVolume(step_mesh->GetSolid(), shape1_mat, "step_logical", 0, 0, 0);
        new G4PVPlacement(nullptr, pos1, step_logical, "step_physical", logicEnv, false, 0);
 
        // Set Shape2 as scoring volume
        fScoringVolume = logicShape2;

        // Always return the physical World
        return physWorld;
    }

}  // namespace B1
