#include "RubiksCube.h"
#include "RubiksGameInterface.h"
#include <glm/gtx/intersect.hpp>
#include <iomanip>
#define NOMINMAX
#include <windows.h>

void RubiksCube::Initialize(const RubiksGameInterface& gameInterface) {
	m_cubieRenderer.Initialize();
	
	float offset = m_cubieRenderer.GetCubieExtension();

	for (int x = 0; x < 3; x++)
		for (int y = 0; y < 3; y++)
			for (int z = 0; z < 3; z++) {
				m_grid[x][y][z] = new Cubie;
				m_grid[x][y][z]->m_id = (x * 3 * 3) + (y * 3) + z;
				m_grid[x][y][z]->m_position = glm::vec3((x - 1) * offset, (y - 1) * offset, (z - 1) * offset);
			}
	m_input = &gameInterface.GetInputComponent();
}

void RubiksCube::Render(const glm::mat4& viewProjection) {
	
	glm::mat4 cubeRotation = glm::mat4_cast(m_modelRotation);
	
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

//INPUT
void RubiksCube::UpdateMouse() {

	//LEFT CLICK
	if (m_input->GetActiveMouseButton() == InputSystem::LEFT_BUTTON) {
		
		switch (m_input->GetLeftClickState()) {
			case InputSystem::CLICK:
				switch (m_animationState) {
					case AnimationState::STABLE:
						DetermineClickedFace();
						break;
					case AnimationState::ROTATING:
						break;
					case AnimationState::SNAPING:
						break;
					default:
						break;
				}
				break;


			case InputSystem::HOLD:
				switch (m_animationState) {
					case AnimationState::STABLE:
						if (m_clickedFace == CubeFace::UNSET_FACE)
							break;
						if (glm::length(m_input->GetScreenPosition() - m_input->GetDragStartScreenPosition()) > 10) {
							DetermineActiveFace();
							m_animationState = AnimationState::ROTATING;
							break;
						}
						break;
					case AnimationState::ROTATING:
						if (m_activeFaceNormal != Axis::UNSET_AXIS)
							DeltaRotateFace();
						break;
					case AnimationState::SNAPING:
						break;
					default:
						break;
				}
				break;

			case InputSystem::RELEASE:
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
		if (m_input->GetRightClickState() == InputSystem::HOLD) {
			RotateCube();
		}
	}
}

//CUBE
void RubiksCube::RotateCube() {
	//Calculate the difference between the current and previous screen positions
	glm::vec2 delta = m_input->GetScreenPosition() - m_previousScreenPosition;

	//Convert the delta vector to radians
	float angleX = glm::radians(delta.x);
	float angleY = glm::radians(delta.y);

	//Create quaternions for the rotations
	glm::quat rotationX = glm::angleAxis(angleX, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::quat rotationY = glm::angleAxis(angleY, glm::vec3(1.0f, 0.0f, 0.0f));

	//Apply the rotations to the cubeRotation
	m_modelRotation = rotationX * m_modelRotation;
	m_modelRotation = rotationY * m_modelRotation;
	m_modelRotation = glm::normalize(m_modelRotation);
}


//FACE ROTATION
void RubiksCube::DetermineClickedFace() {
	glm::vec3 origin, direction;
	m_input->GetPickingRay(origin, direction);

	float planeOffset = 1.5f * m_cubieRenderer.GetCubieExtension();
	for (auto normal : NORMALS_OF_FACES) {
		const glm::vec3 faceNormal = glm::mat3_cast(m_modelRotation) * normal.second;
		if (glm::dot(direction, faceNormal) > 0)
			continue;

		float intersectionDistance;
		bool intersects = glm::intersectRayPlane(origin,
			direction,
			faceNormal * (planeOffset),
			faceNormal,
			intersectionDistance);
		if (!intersects)
			continue;

		glm::vec3 intersectionPoint = origin + intersectionDistance * direction;
		glm::vec3 intersectionPointInObjectSpace
			= glm::inverse(glm::mat3_cast(m_modelRotation)) * intersectionPoint;
		//0.1f is tolerance
		if ((intersectionPointInObjectSpace.x > (planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.x < -(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.y >(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.y < -(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.z >(planeOffset + 0.1f) ||
			intersectionPointInObjectSpace.z < -(planeOffset + 0.1f)))
			continue;

		m_clickedFace = static_cast<CubeFace>(normal.first);
		m_facePlaneIntersectionPoint = intersectionPoint;
		break;
	}
}

void RubiksCube::DetermineActiveFace() {
	//Es wird der Schnittpunkt in Object Space bestimmt
	glm::vec3 intersectionPointInObjectSpace
		= glm::inverse(glm::mat3_cast(m_modelRotation)) * m_facePlaneIntersectionPoint;
	float planeOffset = 1.5f * m_cubieRenderer.GetCubieExtension();

	//Die slice indices werden bestimmt
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



	//Es wird der aktuelle Schnittpunkt auf der geklicken Ebene im Object Space bestimmt
	float intersectionDistance;
	glm::vec3 origin, direction;
	m_input->GetPickingRay(origin, direction);
	//Warnung: bei freier Kamerasteurung, könnte es auch kein Schnittpunkt geben
	//Hier wird die Normale in für die Rechnung rotiert, dannach wird die Rotation rückgängig gemacht
	glm::intersectRayPlane(origin,
		direction,
		glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(static_cast<int>(m_clickedFace)) * (planeOffset),
		glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(static_cast<int>(m_clickedFace)),
		intersectionDistance);
	glm::vec3 currenIntersectionPointInObjectSpace =
		glm::inverse(glm::mat3_cast(m_modelRotation)) * (origin + intersectionDistance * direction);

	//Normale der geklicken Face wird normalisiert
	int positiveNormalIndex = static_cast<int>(m_clickedFace) % 3;

	//Die Drag Richtung wird bestimmt
	glm::vec3 dragDirection = currenIntersectionPointInObjectSpace - intersectionPointInObjectSpace;
	glm::vec3 dragDirectionNormal = FindClosestDirection(
		dragDirection,
		NORMALS_OF_FACES.at((positiveNormalIndex + 1) % 3),
		NORMALS_OF_FACES.at((positiveNormalIndex + 2) % 3));

	if (m_clickedFace == CubeFace::LEFT_FACE || m_clickedFace == CubeFace::RIGHT_FACE) {
		if (dragDirectionNormal.y == 1.0f) {
			m_activeFaceNormal = Axis::Z;
		}
		else if (dragDirectionNormal.z == 1.0f) {
			m_activeFaceNormal = Axis::Y;
		}
	}
	else if (m_clickedFace == CubeFace::FRONT_FACE || m_clickedFace == CubeFace::BACK_FACE) {
		if (dragDirectionNormal.x == 1.0f) {
			m_activeFaceNormal = Axis::Y;
		}
		else if (dragDirectionNormal.y == 1.0f) {
			m_activeFaceNormal = Axis::X;
		}
	}
	else { //m_fr_clickedFace == TOP_FACE || m_fr_clickedFace == BOTTOM_FACE
		if (dragDirectionNormal.x == 1.0f) {
			m_activeFaceNormal = Axis::Z;
		}
		else if (dragDirectionNormal.z == 1.0f) {
			m_activeFaceNormal = Axis::X;
		}
	}

	std::cout << "Der Slice ist "
		<< "X: " << m_xSliceIndex << ", "
		<< "Y: " << m_ySliceIndex << ", "
		<< "Z: " << m_zSliceIndex << '\n';
	std::cout << "Die aktive Achse ist: " << static_cast<int>(m_activeFaceNormal) << '\n';
}

void RubiksCube::DeltaRotateFace() {
	glm::vec2 deltaDragVector = m_input->GetScreenPosition() - m_previousScreenPosition;

	int activeFaceIndex = static_cast<int>(m_activeFaceNormal);
	int clickedFaceIndex = static_cast<int>(m_clickedFace) % 3;

	//Index of the Normal that is not active nor clicked
	int dragFaceIndex = 3 - activeFaceIndex - clickedFaceIndex;
	glm::vec3 dragNormalInWorld
		= glm::mat3_cast(m_modelRotation) * NORMALS_OF_FACES.at(dragFaceIndex);

	glm::vec2 dragNormalInScreenSpace
		= m_input->WorldToScreen(dragNormalInWorld)
		- m_input->WorldToScreen(glm::vec3(0.0f));

	float scaleProjectedVector
		= glm::dot(deltaDragVector, dragNormalInScreenSpace) / glm::length(dragNormalInScreenSpace);

	float deltaRotation = glm::length(scaleProjectedVector * dragNormalInScreenSpace);

	if (scaleProjectedVector < 0)
		deltaRotation *= -1;

	// Nicht schön
	if (m_clickedFace == CubeFace::RIGHT_FACE) {
		if (m_activeFaceNormal == Axis::Y)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::TOP_FACE) {
		if (m_activeFaceNormal == Axis::Z)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::FRONT_FACE) {
		if (m_activeFaceNormal == Axis::X)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::LEFT_FACE) {
		if (m_activeFaceNormal == Axis::Z)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::BOTTOM_FACE) {
		if (m_activeFaceNormal == Axis::X)
			deltaRotation *= -1;
	}
	else if (m_clickedFace == CubeFace::BACK_FACE) {
		if (m_activeFaceNormal == Axis::Y)
			deltaRotation *= -1;
	}

	const float ROTATION_MODIFIER = 10.0f;

	glm::mat4 rotationMatrix
		= glm::rotate(
			glm::mat4(1.0f),
			glm::radians(deltaRotation * ROTATION_MODIFIER),
			NORMALS_OF_FACES.at(static_cast<int>(m_activeFaceNormal))
		);
	ForEachInSlice([&rotationMatrix](Cubie* cubie, int index) {
		cubie->m_visibleRotation
			= rotationMatrix * cubie->m_visibleRotation;
		});
	m_totalFaceRotationDegree += deltaRotation * ROTATION_MODIFIER;
}

glm::vec3 RubiksCube::FindClosestDirection(const glm::vec3& referenceDirection, const glm::vec3& vecU, const glm::vec3& vecV) {
	float dotProductU = glm::dot(referenceDirection, vecU);
	float dotProductV = glm::dot(referenceDirection, vecV);
	float dotProductNegativeU = glm::dot(referenceDirection, -vecU);
	float dotProductNegativeV = glm::dot(referenceDirection, -vecV);

	float maxDotProduct = std::max({ dotProductU, dotProductV, dotProductNegativeU, dotProductNegativeV });

	if (maxDotProduct == dotProductU)
		return vecU;

	else if (maxDotProduct == dotProductV)
		return vecV;

	else if (maxDotProduct == dotProductNegativeU)
		return vecU;

	else //maxDotProduct == dotProductNegativeV 
		return vecV;

}

//ANIMATION
void RubiksCube::StartSnappingAnimation() {
	// Normalize total face rotation degree
	while (m_totalFaceRotationDegree > 360)
		m_totalFaceRotationDegree -= 360;
	while (m_totalFaceRotationDegree < 0)
		m_totalFaceRotationDegree += 360;

	if (m_totalFaceRotationDegree < 45.0f)
		m_totalFaceRotationDegree = 0.0f;
	else if (m_totalFaceRotationDegree < 135.0f)
		m_totalFaceRotationDegree = 90.0f;
	else if (m_totalFaceRotationDegree < 225.0f)
		m_totalFaceRotationDegree = 180.0f;
	else if (m_totalFaceRotationDegree < 315.0f)
		m_totalFaceRotationDegree = 270.0f;
	else
		m_totalFaceRotationDegree = 0.0f;

	glm::mat4 totalSnappedRotation
		= glm::rotate(glm::mat4(1.0f),
			glm::radians(m_totalFaceRotationDegree),
			NORMALS_OF_FACES.at(static_cast<int>(m_activeFaceNormal) % 3));

	ForEachInSlice([&totalSnappedRotation, this](Cubie* cubie, int index) {
		cubie->m_snapedRotation = totalSnappedRotation * cubie->m_snapedRotation;
		this->m_oldVisibleRotations[static_cast<int>(index / 3)][index % 3] = glm::quat_cast(cubie->m_visibleRotation);
		}
	);
}

void RubiksCube::UpdateAnimation(float deltaTime) {
	if (1 - m_tickCounter <= 0.0001f) {
		ForEachInSlice([](Cubie* cubie, int index) {
			cubie->m_visibleRotation = cubie->m_snapedRotation;
			}
		);

		//Update Grid
		int rotationCount = 0;
		if (m_totalFaceRotationDegree == 90)
			rotationCount = 1;
		if (m_totalFaceRotationDegree == 180)
			rotationCount = 2;
		if (m_totalFaceRotationDegree == 270)
			rotationCount = 3;

		for (int i = 0; i < rotationCount; i++) {
			std::array<std::array<Cubie*, 3>, 3> newFace;

			ForEachInSlice([&newFace](Cubie* cubie, int index) {
				newFace[static_cast<int>(index / 3)][index % 3] = cubie;
				}
			);

			//Transpose
			std::array<std::array<Cubie*, 3>, 3> transposeFace;
			for (int row = 0; row < 3; row++)
				for (int col = 0; col < 3; col++)
					transposeFace[row][col] = newFace[col][row];

			//Zeilen invertieren
			std::array<std::array<Cubie*, 3>, 3> transposeRowInvertedFace;
			for (int row = 0; row < 3; row++)
				for (int col = 0; col < 3; col++)
					transposeRowInvertedFace[2 - row][col] = transposeFace[row][col];

			ForEachInSlice([&transposeRowInvertedFace](Cubie*& cubie, int index) {
				cubie = transposeRowInvertedFace[static_cast<int>(index / 3)][index % 3];
				}
			);
		}
		m_clickedFace = CubeFace::UNSET_FACE;

		m_activeFaceNormal = Axis::UNSET_AXIS;
		m_xSliceIndex = 0;
		m_ySliceIndex = 0;
		m_zSliceIndex = 0;

		m_animationState = AnimationState::STABLE;

		m_totalFaceRotationDegree = 0;
		m_tickCounter = 0;

		return;
	}

	ForEachInSlice([this](Cubie*& cubie, int index) {
		cubie->m_visibleRotation
			= glm::mat4_cast(
				glm::slerp(this->m_oldVisibleRotations[static_cast<int>(index / 3)][index % 3],
					glm::quat_cast(cubie->m_snapedRotation),
					m_tickCounter));
		}
	);

	float nextSnapingAnimationTick
		= m_tickCounter + (0.05f + deltaTime);
	if (m_tickCounter == nextSnapingAnimationTick)
		m_tickCounter = std::nextafterf(m_tickCounter, std::numeric_limits<float>::infinity());
	else
		m_tickCounter = nextSnapingAnimationTick;
}


//OTHER HELPING METHODS
// Erwartet void (Cubie* cubie, int index) 
template<typename Func>
void RubiksCube::ForEachInSlice(Func func) {
	int index = 0;
	switch (m_activeFaceNormal) {
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
		for (int y = 2; y >= 0; y--) {
			for (int z = 2; z >= 0; z--) {
				func(m_grid[m_xSliceIndex][y][z], index);
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
		for (int z = 0; z < 3; z++) {
			for (int x = 0; x < 3; x++) {
				func(m_grid[x][m_ySliceIndex][z], index);
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
		for (int y = 2; y >= 0; y--) {
			for (int x = 0; x < 3; x++) {
				func(m_grid[x][y][m_zSliceIndex], index);
				index++;
			}
		}
		break;

	default:
		//ERROR
		break;
	}
}

//STATIC
const std::map<int, glm::vec3> RubiksCube::NORMALS_OF_FACES = {
	   {0,	glm::vec3(1.0f, 0.0f, 0.0f)},
	   {1,	glm::vec3(0.0f, 1.0f, 0.0f)},
	   {2,	glm::vec3(0.0f, 0.0f, 1.0f)},
	   {3,	glm::vec3(-1.0f, 0.0f, 0.0f)},
	   {4,	glm::vec3(0.0f, -1.0f, 0.0f)},
	   {5,	glm::vec3(0.0f, 0.0f, -1.0f)}
};