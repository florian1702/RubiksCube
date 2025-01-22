#include "RubiksCube.h"
#include "RubiksGameInterface.h"
#include <glm/gtx/intersect.hpp>
#include <iomanip>
#define NOMINMAX
#include <iostream>
#include <windows.h>

// INITIALIZES THE RUBIK'S CUBE BY SETTING UP THE 3D GRID OF CUBIES
void RubiksCube::Initialize(const RubiksGameInterface& gameInterface) {
	m_cubieRenderer.Initialize(); // INITIALIZE THE CUBIE RENDERER
	
	float offset = m_cubieRenderer.GetCubieExtension();  // OFFSET FOR POSITIONING CUBIES

	// CREATE A 3X3X3 GRID OF CUBIES WITH UNIQUE POSITIONS AND IDs
	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				m_grid[x][y][z] = new Cubie;
				m_grid[x][y][z]->m_id = (x * 3 * 3) + (y * 3) + z;
				m_grid[x][y][z]->m_position = glm::vec3((x - 1) * offset, (y - 1) * offset, (z - 1) * offset);
			}

	// GET INPUT SYSTEM FROM THE GAME INTERFACE
	m_input = &gameInterface.GetInputComponent();
}

// RENDERS EACH CUBIE IN THE RUBIK'S CUBE
void RubiksCube::Render(const glm::mat4& viewProjection) {

	// CONVERT ROTATION QUATERNION TO MATRIX
	glm::mat4 cubeRotation = glm::mat4_cast(m_modelRotation);

	// ITERATE OVER THE GRID AND RENDER EACH CUBIE WITH ITS TRANSFORMATION
	for (int x = 0; x < 3; ++x) {
		for (int y = 0; y < 3; ++y) {
			for (int z = 0; z < 3; ++z) {
				glm::mat4 cubieTranslation = glm::translate(glm::mat4(1.0f), m_grid[x][y][z]->m_position);
				glm::mat4 cubieRotation = m_grid[x][y][z]->m_visibleRotation;
				m_cubieRenderer.Render(viewProjection, cubeRotation * cubieRotation * cubieTranslation);
			}
		}
	}
}

void RubiksCube::Update(const RubiksGameInterface& gameInterface)
{
	if (m_animationState == AnimationState::SNAPING) {
		UpdateAnimation(gameInterface.GetDeltaTime());
	}

	UpdateMouse();
	m_previousScreenPosition = m_input->GetScreenPosition();
}

void RubiksCube::ClearResources()
{
	m_cubieRenderer.ClearResources();
	for (auto& arr2d : m_grid)
		for (auto& arr : arr2d)
			for (auto& e : arr)
				delete e;
}

// HANDLES CLICK ACTIONS FOR ROTATING THE CUBE OR FACES
void RubiksCube::UpdateMouse() {

	// LEFT CLICK
	if (m_input->GetActiveMouseButton() == InputSystem::LEFT_BUTTON) {
		switch (m_input->GetLeftClickState()) {
			case InputSystem::CLICK:
				// DETERMINE CLICKED FACE ONLY IF ANIMATION STATE IS STABLE
				if (m_animationState == AnimationState::STABLE) {
					DetermineClickedFace();
				}
				break;

			case InputSystem::HOLD:
				if (m_animationState == AnimationState::STABLE) {
					// START ROTATION IF FACE IS CLICKED AND DRAG DISTANCE IS ENOUGH (DEAD ZONE = 10)
					if (m_clickedFace != CubeFace::UNSET_FACE &&
						glm::length(m_input->GetScreenPosition() - m_input->GetDragStartScreenPosition()) > 10) {
						DetermineActiveSlice();
						m_animationState = AnimationState::ROTATING;
					}
				}
				// UPDATE ROTATION WHILE DRAGGING
				else if (m_animationState == AnimationState::ROTATING && m_activeRotationAxis != Axis::UNSET_AXIS) {
					DeltaRotateSlice();
				}
				break;

			case InputSystem::RELEASE:
				// START SNAP ANIMATION WHEN RELEASING AFTER ROTATION
				if (m_animationState == AnimationState::ROTATING) {
					StartSnappingAnimation();
					PlaySound(TEXT("rubiks_sfx.wav"), NULL, SND_FILENAME | SND_ASYNC);
					m_animationState = AnimationState::SNAPING;
				}
				break;
		}
	}

	//RIGHT CLICK
	else if (m_input->GetActiveMouseButton() == InputSystem::RIGHT_BUTTON) {
		// ROTATE CUBE WHEN HOLDING RIGHT MOUSE BUTTON
		if (m_input->GetRightClickState() == InputSystem::HOLD) {
			RotateCube();
		}
	}
}

// ROTATES THE WHOLE CUBE BASED ON MOUSE DRAG MOVEMENT
void RubiksCube::RotateCube() {
	//CALCULATE DIFFERENCE BETWEEN CURRENT AND PREVIOUS MOUSE POSITION
	glm::vec2 delta = m_input->GetScreenPosition() - m_previousScreenPosition;

	// CONVERT TO ROTATION ANGLES (IN RADIANS)
	float angleX = glm::radians(delta.x);
	float angleY = glm::radians(delta.y);

	// CREATE ROTATION QUATERNIONS AND APPLY TO MODEL ROTATION
	glm::quat rotationX = glm::angleAxis(angleX, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat rotationY = glm::angleAxis(angleY, glm::vec3(1.0f, 0.0f, 0.0f));
	m_modelRotation = rotationX * m_modelRotation;
	m_modelRotation = rotationY * m_modelRotation;
	m_modelRotation = glm::normalize(m_modelRotation); // KEEP THE ROTATION UNIT QUATERNION NORMALIZED
}


// DETERMINE THE FACE OF THE RUBIK'S CUBE THAT WAS CLICKED
void RubiksCube::DetermineClickedFace() {

	// GENERATE A PICKING RAY FROM THE CURRENT MOUSE POSITION
	glm::vec3 origin, direction;
	m_input->GetPickingRay(origin, direction);

	// OFFSET TO IDENTIFY PLANES REPRESENTING CUBE FACES
	float planeOffset = 1.5f * m_cubieRenderer.GetCubieExtension();

	// ITERATE OVER ALL POSSIBLE FACE NORMALS
	for (auto normal : NORMALS_OF_FACES) {

		// TRANSFORM FACE NORMAL TO WORLD SPACE
		const glm::vec3 faceNormal = glm::mat3_cast(m_modelRotation) * normal.second;

		// SKIP IF RAY IS POINTING AWAY FROM THE FACE
		if (glm::dot(direction, faceNormal) > 0)
			continue;

		float intersectionDistance;

		// CHECK IF THE RAY INTERSECTS THE FACE PLANE
		bool intersects = glm::intersectRayPlane(
			origin,                          // RAY ORIGIN
			direction,                       // RAY DIRECTION
			faceNormal * (planeOffset),      // POINT ON THE PLANE
			faceNormal,                      // PLANE NORMAL
			intersectionDistance);        // OUTPUT INTERSECTION DISTANCE

		if (!intersects)
			continue;

		// COMPUTE THE INTERSECTION POINT IN WORLD SPACE
		glm::vec3 intersectionPoint = origin + intersectionDistance * direction;

		// TRANSFORM INTERSECTION POINT TO OBJECT SPACE
		glm::vec3 intersectionPointInObjectSpace
			= glm::inverse(glm::mat3_cast(m_modelRotation)) * intersectionPoint;

		// ENSURE THE INTERSECTION IS WITHIN FACE BOUNDARIES (WITH TOLERANCE)
		if ((intersectionPointInObjectSpace.x > (planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.x < -(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.y >(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.y < -(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.z >(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.z < -(planeOffset + 0.1f)))
			continue;

		// IF INTERSECTION IS VALID, SAVE THE CLICKED FACE AND INTERSECTION POINT
		m_clickedFace = static_cast<CubeFace>(normal.first);
		m_facePlaneIntersectionPoint = intersectionPoint;
		std::cout << "Die geclickte Flaeche ist: " << static_cast<int>(m_clickedFace) << '\n';
		break;
	}
}

// DETERMINE THE ACTIVE FACE AND DRAG DIRECTION FOR ROTATING A SLICE
void RubiksCube::DetermineActiveSlice() {
	// TRANSFORM THE SAVED FACE INTERSECTION POINT INTO OBJECT SPACE
	glm::vec3 intersectionPointInObjectSpace
		= glm::inverse(glm::mat3_cast(m_modelRotation)) * m_facePlaneIntersectionPoint;

	// OFFSET FOR CUBE DIMENSIONS
	float planeOffset = 1.5f * m_cubieRenderer.GetCubieExtension();

	// DETERMINE THE SLICE INDEX FOR X, Y, AND Z DIMENSIONS BASED ON THE INTERSECTION POINT
	m_xSliceIndex
		= (intersectionPointInObjectSpace.x >= (-planeOffset / 3)
			&& intersectionPointInObjectSpace.x <= (planeOffset / 3))
		? 1 : (intersectionPointInObjectSpace.x > (planeOffset / 3) ? 2 : 0);
	m_ySliceIndex
		= (intersectionPointInObjectSpace.y >= (-planeOffset / 3)
			&& intersectionPointInObjectSpace.y <= (planeOffset / 3))
		? 1 : (intersectionPointInObjectSpace.y > (planeOffset / 3) ? 2 : 0);
	m_zSliceIndex
		= (intersectionPointInObjectSpace.z >= (-planeOffset / 3)
			&& intersectionPointInObjectSpace.z <= (planeOffset / 3))
		? 1 : (intersectionPointInObjectSpace.z > (planeOffset / 3) ? 2 : 0);

	// GENERATE A RAY TO DETERMINE CURRENT INTERSECTION POINT ON THE CLICKED PLANE
	float intersectionDistance;
	glm::vec3 origin, direction;
	m_input->GetPickingRay(origin, direction);

	glm::intersectRayPlane(origin,
		direction,
		glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(static_cast<int>(m_clickedFace)) * (planeOffset),
		glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(static_cast<int>(m_clickedFace)),
		intersectionDistance);

	// COMPUTE THE CURRENT INTERSECTION POINT IN OBJECT SPACE
	glm::vec3 currenIntersectionPointInObjectSpace =
		glm::inverse(glm::mat3_cast(m_modelRotation)) * (origin + intersectionDistance * direction);

	// IDENTIFY THE POSITIVE INDEX OF THE CLICKED FACE'S NORMAL
	int positiveNormalIndex = static_cast<int>(m_clickedFace) % 3;

	// COMPUTE THE DRAG DIRECTION VECTOR IN OBJECT SPACE
	glm::vec3 dragDirection = currenIntersectionPointInObjectSpace - intersectionPointInObjectSpace;

	// IDENTIFY THE CLOSEST DIRECTION TO THE DRAG VECTOR
	glm::vec3 dragDirectionNormal = FindClosestDirection(
		dragDirection,
		NORMALS_OF_FACES.at((positiveNormalIndex + 1) % 3), // VERTICAL DIRECTIONS
		NORMALS_OF_FACES.at((positiveNormalIndex + 2) % 3)
	);

	// DETERMINE THE ACTIVE AXIS BASED ON THE CLICKED FACE AND DRAG DIRECTION
	if (m_clickedFace == CubeFace::LEFT_FACE || m_clickedFace == CubeFace::RIGHT_FACE) {
		m_activeRotationAxis = (dragDirectionNormal.y == 1.0f) ? Axis::Z : Axis::Y;
	}
	else if (m_clickedFace == CubeFace::FRONT_FACE || m_clickedFace == CubeFace::BACK_FACE) {
		m_activeRotationAxis = (dragDirectionNormal.x == 1.0f) ? Axis::Y : Axis::X;
	}
	else { // TOP_FACE OR BOTTOM_FACE
		m_activeRotationAxis = (dragDirectionNormal.x == 1.0f) ? Axis::Z : Axis::X;
	}

	// LOG THE DETERMINED SLICE INDICES AND ACTIVE AXIS FOR DEBUGGING
	std::cout << "Der Slice ist "
		<< "X: " << m_xSliceIndex << ", "
		<< "Y: " << m_ySliceIndex << ", "
		<< "Z: " << m_zSliceIndex << '\n';
	std::cout << "Die aktive Achse ist: " << static_cast<int>(m_activeRotationAxis) << '\n';
}

// ROTATE THE SELECTED FACE BASED ON MOUSE DRAG INPUT
void RubiksCube::DeltaRotateSlice() {
	// CALCULATE THE MOUSE DRAG VECTOR (CURRENT SCREEN POSITION - PREVIOUS POSITION)
	glm::vec2 deltaDragVector = m_input->GetScreenPosition() - m_previousScreenPosition;

	// DETERMINE THE ACTIVE, CLICKED, AND DRAG AXES
	int activeRotationAxisIndex = static_cast<int>(m_activeRotationAxis);		// INDEX OF ACTIVE FACE NORMAL
	int clickedFaceIndex = static_cast<int>(m_clickedFace) % 3;     // INDEX OF THE CLICKED FACE
	int dragFaceIndex = 3 - activeRotationAxisIndex - clickedFaceIndex;     // REMAINING FACE THAT DEFINES DRAG

	// CONVERT DRAG NORMAL TO WORLD SPACE USING THE CURRENT CUBE ROTATION
	glm::vec3 dragNormalInWorld = glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(dragFaceIndex);

	// PROJECT THE WORLD DRAG NORMAL INTO SCREEN SPACE
	glm::vec2 dragNormalInScreenSpace
		= m_input->WorldToScreen(dragNormalInWorld)
		- m_input->WorldToScreen(glm::vec3(0.0f));

	// PROJECT THE MOUSE DRAG VECTOR ONTO THE DRAG DIRECTION
	float scaleProjectedVector
		= glm::dot(deltaDragVector, dragNormalInScreenSpace) / glm::length(dragNormalInScreenSpace);

	// COMPUTE THE AMOUNT OF ROTATION BASED ON THE DRAG MAGNITUDE
	float deltaRotation = glm::length(scaleProjectedVector * dragNormalInScreenSpace);

	// ADJUST THE SIGN OF THE ROTATION BASED ON DRAG DIRECTION
	if (scaleProjectedVector < 0)
		deltaRotation *= -1;

	// HANDLE DIRECTION CORRECTIONS FOR SPECIFIC FACES AND ACTIVE AXES
	if (m_clickedFace == CubeFace::RIGHT_FACE) {
		if (m_activeRotationAxis == Axis::Y)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::TOP_FACE) {
		if (m_activeRotationAxis == Axis::Z)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::FRONT_FACE) {
		if (m_activeRotationAxis == Axis::X)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::LEFT_FACE) {
		if (m_activeRotationAxis == Axis::Z)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::BOTTOM_FACE) {
		if (m_activeRotationAxis == Axis::X)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::BACK_FACE) {
		if (m_activeRotationAxis == Axis::Y)
			deltaRotation *= -1;
	}

	// SCALE ROTATION TO APPLY A MODIFIER (E.G., TO SPEED UP OR SMOOTH THE ROTATION)
	const float ROTATION_MODIFIER = 10.0f;
	deltaRotation *= ROTATION_MODIFIER;

	// CREATE A ROTATION MATRIX USING THE ACTIVE FACE NORMAL AND DELTA ROTATION
	glm::mat4 rotationMatrix = glm::rotate(
		glm::mat4(1.0f),
		glm::radians(deltaRotation),
		NORMALS_OF_FACES.at(static_cast<int>(m_activeRotationAxis))
	);

	// APPLY THE ROTATION MATRIX TO EACH CUBIE IN THE SELECTED SLICE
	ForEachInSlice([&rotationMatrix](Cubie* cubie, int index) {
		cubie->m_visibleRotation = rotationMatrix * cubie->m_visibleRotation;
		});

	// UPDATE THE TOTAL ROTATION DEGREE FOR THE FACE (USED FOR ANIMATION/SNAP LATER)
	m_totalFaceRotationDegree += deltaRotation;
}

// FINDS THE CLOSEST VECTOR (POSITIVE OR NEGATIVE) TO A GIVEN REFERENCE DIRECTION
glm::vec3 RubiksCube::FindClosestDirection(const glm::vec3& referenceDirection, const glm::vec3& vecU, const glm::vec3& vecV) {
	// COMPUTE DOT PRODUCTS BETWEEN THE REFERENCE DIRECTION AND THE GIVEN VECTORS
	float dotProductU = glm::dot(referenceDirection, vecU);          // vecU (positive direction)
	float dotProductV = glm::dot(referenceDirection, vecV);          // vecV (positive direction)
	float dotProductNegativeU = glm::dot(referenceDirection, -vecU); // vecU (negative direction)
	float dotProductNegativeV = glm::dot(referenceDirection, -vecV); // vecV (negative direction)

	// FIND THE MAXIMUM DOT PRODUCT (INDICATES CLOSEST DIRECTION)
	float maxDotProduct = std::max({ dotProductU, dotProductV, dotProductNegativeU, dotProductNegativeV });

	// RETURN THE VECTOR THAT CORRESPONDS TO THE MAXIMUM DOT PRODUCT
	if (maxDotProduct == dotProductU)
		return vecU;              
	else if (maxDotProduct == dotProductV)
		return vecV;            
	else if (maxDotProduct == dotProductNegativeU)
		return vecU;             
	else
		return vecV;             
}

// STARTS AN ANIMATION TO ALIGN THE CURRENT FACE ROTATION TO THE NEAREST MULTIPLE OF 90°
void RubiksCube::StartSnappingAnimation() {
	// NORMALIZE THE TOTAL ROTATION DEGREE TO FALL WITHIN [0, 360] RANGE
	while (m_totalFaceRotationDegree > 360)
		m_totalFaceRotationDegree -= 360; // REMOVE FULL ROTATIONS
	while (m_totalFaceRotationDegree < 0)
		m_totalFaceRotationDegree += 360; // HANDLE NEGATIVE ROTATIONS

	// SNAP THE ROTATION DEGREE TO THE NEAREST MULTIPLE OF 90°
	if (m_totalFaceRotationDegree < 45.0f)                 // CLOSEST TO 0°
		m_totalFaceRotationDegree = 0.0f;
	else if (m_totalFaceRotationDegree < 135.0f)           // CLOSEST TO 90°
		m_totalFaceRotationDegree = 90.0f;
	else if (m_totalFaceRotationDegree < 225.0f)           // CLOSEST TO 180°
		m_totalFaceRotationDegree = 180.0f;
	else if (m_totalFaceRotationDegree < 315.0f)           // CLOSEST TO 270°
		m_totalFaceRotationDegree = 270.0f;
	else                                                  // CLOSEST TO 0° (AFTER FULL ROTATION)
		m_totalFaceRotationDegree = 0.0f;

	// GENERATE THE FINAL ROTATION MATRIX FOR THE SNAPPED ANGLE
	glm::mat4 totalSnappedRotation = glm::rotate(
		glm::mat4(1.0f),                                        // IDENTITY MATRIX
		glm::radians(m_totalFaceRotationDegree),                // SNAPPED ANGLE (IN RADIANS)
		NORMALS_OF_FACES.at(static_cast<int>(m_activeRotationAxis) % 3) // ACTIVE FACE NORMAL VECTOR (AXIS)
	);

	// APPLY THE SNAPPED ROTATION TO ALL CUBIES IN THE ACTIVE SLICE
	ForEachInSlice([&totalSnappedRotation, this](Cubie* cubie, int index) {
		cubie->m_snapedRotation = totalSnappedRotation * cubie->m_snapedRotation; // UPDATE SNAPPED ROTATION
		// SAVE THE VISIBLE ROTATION BEFORE ANIMATION STARTS
		this->m_oldVisibleRotations[static_cast<int>(index / 3)][index % 3] =
			glm::quat_cast(cubie->m_visibleRotation);
		});
}


// UPDATES THE SNAPPING ANIMATION FOR THE CUBE'S FACE ROTATION
void RubiksCube::UpdateAnimation(float deltaTime) {
	// CHECK IF THE ANIMATION IS COMPLETE (TICK COUNTER HAS REACHED 1)
	if (1 - m_tickCounter <= 0.0001f) {
		// FINALIZE THE ROTATION: SET ALL CUBIES IN THE SLICE TO THEIR SNAPPED ROTATION
		ForEachInSlice([](Cubie* cubie, int index) {
			cubie->m_visibleRotation = cubie->m_snapedRotation;
			});

		// DETERMINE THE NUMBER OF 90-DEGREE ROTATIONS TO APPLY BASED ON TOTAL ROTATION
		int rotationCount = 0;
		if (m_totalFaceRotationDegree == 90)
			rotationCount = 1;
		if (m_totalFaceRotationDegree == 180)
			rotationCount = 2;
		if (m_totalFaceRotationDegree == 270)
			rotationCount = 3;

		// APPLY THE ROTATION TO THE SLICE IN THE GRID ARRAY
		for (int i = 0; i < rotationCount; i++) {
			// STORE THE CURRENT SLICE IN A TEMPORARY ARRAY
			std::array<std::array<Cubie*, 3>, 3> newFace;
			ForEachInSlice([&newFace](Cubie* cubie, int index) {
				newFace[static_cast<int>(index / 3)][index % 3] = cubie;
				});

			// TRANSPOSE THE TEMPORARY ARRAY (ROW AND COLUMN SWAP)
			std::array<std::array<Cubie*, 3>, 3> transposeFace;
			for (int row = 0; row < 3; row++)
				for (int col = 0; col < 3; col++)
					transposeFace[row][col] = newFace[col][row];

			// INVERT THE ROWS TO COMPLETE THE ROTATION
			std::array<std::array<Cubie*, 3>, 3> transposeRowInvertedFace;
			for (int row = 0; row < 3; row++)
				for (int col = 0; col < 3; col++)
					transposeRowInvertedFace[2 - row][col] = transposeFace[row][col];

			// UPDATE THE ORIGINAL SLICE WITH THE ROTATED ARRAY
			ForEachInSlice([&transposeRowInvertedFace](Cubie*& cubie, int index) {
				cubie = transposeRowInvertedFace[static_cast<int>(index / 3)][index % 3];
				});
		}

		// RESET FACE AND AXIS INFORMATION AFTER THE ROTATION
		m_clickedFace = CubeFace::UNSET_FACE;
		m_activeRotationAxis = Axis::UNSET_AXIS;
		m_xSliceIndex = 0;
		m_ySliceIndex = 0;
		m_zSliceIndex = 0;

		// TRANSITION BACK TO STABLE STATE AFTER THE ANIMATION
		m_animationState = AnimationState::STABLE;

		// RESET THE ROTATION AND ANIMATION VARIABLES
		m_totalFaceRotationDegree = 0;
		m_tickCounter = 0;
		return; // EXIT FUNCTION AS ANIMATION IS COMPLETE
	}

	// CONTINUE THE ANIMATION: INTERPOLATE THE ROTATION OF EACH CUBIE IN THE SLICE
	ForEachInSlice([this](Cubie*& cubie, int index) {
		cubie->m_visibleRotation =
			glm::mat4_cast(glm::slerp(
				this->m_oldVisibleRotations[static_cast<int>(index / 3)][index % 3],
				glm::quat_cast(cubie->m_snapedRotation),
				m_tickCounter)); // PROGRESS OF THE ANIMATION
		});

	// UPDATE THE TICK COUNTER FOR ANIMATION PROGRESS
	float nextSnapingAnimationTick = m_tickCounter + (0.05f + deltaTime);

	// HANDLE FLOATING-POINT PRECISION EDGE CASES
	if (m_tickCounter == nextSnapingAnimationTick)
		m_tickCounter = std::nextafterf(m_tickCounter, std::numeric_limits<float>::infinity());
	else
		m_tickCounter = nextSnapingAnimationTick;
}



// ITERATES OVER ALL CUBIES IN THE ACTIVE SLICE, APPLYING A GIVEN FUNCTION
// Expects a function: void(Cubie* cubie, int index)
template<typename Func>
void RubiksCube::ForEachInSlice(Func func) {
	int index = 0;
	// DETERMINE THE ACTIVE AXIS AND ITERATE OVER THE SLICE
	switch (m_activeRotationAxis) {
	case Axis::X:
		//
		//              Y
		//              ^
		//    [0][0]    |    [0][2]
		//          +---|---+
		//          |   |   |
		//  Z   <-------+   |
		//          |       |
		//          +-------+
		//    [2][0]         [2][2]
		//
		for (int y = 2; y >= 0; y--) {						// TRAVERSE Y-DIRECTION
			for (int z = 2; z >= 0; z--) {					// TRAVERSE Z-DIRECTION
				func(m_grid[m_xSliceIndex][y][z], index);	// APPLY FUNCTION
				index++;
			}
		}
		break;

	case Axis::Y:
		//
		//    [0][0]         [0][2]
		//          +-------+
		//          |       |
		//          |   +-------> X
		//          |   |   |
		//          +---|---+
		//    [2][0]    |    [2][2]
		//              V
		//              Z
		//
		for (int z = 0; z < 3; z++) {						// TRAVERSE Z-DIRECTION
			for (int x = 0; x < 3; x++) {					// TRAVERSE X-DIRECTION
				func(m_grid[x][m_ySliceIndex][z], index);	// APPLY FUNCTION
				index++;
			}
		}
		break;

	case Axis::Z:
		//
		//              Y
		//              ^
		//    [0][0]    |    [0][2]
		//          +---|---+
		//          |   |   |
		//          |   +-------> X
		//          |       |
		//          +-------+
		//    [2][0]         [2][2]
		//
		for (int y = 2; y >= 0; y--) {						// TRAVERSE Y-DIRECTION
			for (int x = 0; x < 3; x++) {					// TRAVERSE X-DIRECTION
				func(m_grid[x][y][m_zSliceIndex], index);	// APPLY FUNCTION
				index++;
			}
		}
		break;

	default:
		// ERROR: UNSUPPORTED AXIS
		break;
	}
}

// STATIC MAPPING OF NORMALS FOR EACH FACE OF THE RUBIK'S CUBE
const std::map<int, glm::vec3> RubiksCube::NORMALS_OF_FACES = {
	{0, glm::vec3(1.0f,  0.0f,  0.0f)},	// RIGHT FACE (+X)
	{1, glm::vec3(0.0f,  1.0f,  0.0f)},	// TOP FACE (+Y)
	{2, glm::vec3(0.0f,  0.0f,  1.0f)},	// FRONT FACE (+Z)
	{3, glm::vec3(-1.0f,  0.0f,  0.0f)},	// LEFT FACE (-X)
	{4, glm::vec3(0.0f, -1.0f,  0.0f)},	// BOTTOM FACE (-Y)
	{5, glm::vec3(0.0f,  0.0f, -1.0f)}		// BACK FACE (-Z)
};