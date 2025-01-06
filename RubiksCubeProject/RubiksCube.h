#pragma once
#include "CubieRenderer.h"
#include "InputSystem.h"
#include "Cubie.h"

#include <map>
#include <array>
#include <glm/ext.hpp>

class RubiksGameInterface;

// The RubiksCube class handles the initialization, rendering, user input, rotation logic, and animation for the Rubik's Cube simulation.
class RubiksCube {
public:
	enum class AnimationState {
		STABLE = 0,
		ROTATING = 1,
		SNAPING = 2
	};

	enum class Axis {
		UNSET_AXIS = -1,
		X = 0,
		Y = 1,
		Z = 2
	};

	enum class CubeFace {
		UNSET_FACE = -1,
		RIGHT_FACE = 0,
		TOP_FACE = 1,
		FRONT_FACE = 2,
		LEFT_FACE = 3,
		BOTTOM_FACE = 4,
		BACK_FACE = 5
	};

	void Initialize(const RubiksGameInterface& gameInterface);
	void Render(const glm::mat4& viewProjection);
	void Update(const RubiksGameInterface& gameInterface);
	void ClearResources();

private:
	//INPUT 
	void UpdateMouse();

	//CUBE
	void RotateCube();

	//FACE ROTATION
	void DetermineClickedFace();
	void DetermineActiveFace();
	glm::vec3 FindClosestDirection(const glm::vec3& referenceDirection, const glm::vec3& vectorU, const glm::vec3& vectorV);
	void DeltaRotateFace();

	//ANIMATION
	void StartSnappingAnimation();
	void UpdateAnimation(float deltaTime);

	//OTHER HELPING METHODS
	template<typename Func>
	void ForEachInSlice(Func func);

	//COMPONENTS
	CubieRenderer m_cubieRenderer;
	const InputSystem* m_input;
	
	//CUBE
	glm::quat m_modelRotation;
	std::array <std::array <std::array <Cubie*, 3>, 3>, 3> m_grid;

	//FACE ROTATION
	CubeFace m_clickedFace = CubeFace::UNSET_FACE;

	Axis m_activeFaceNormal = Axis::UNSET_AXIS;
	int m_xSliceIndex = 0;
	int m_ySliceIndex = 0;
	int m_zSliceIndex = 0;
	glm::vec3 m_facePlaneIntersectionPoint;

	//ANIMATION
	AnimationState m_animationState = AnimationState::STABLE;
	float m_totalFaceRotationDegree;
	std::array<std::array<glm::quat, 3>, 3> m_oldVisibleRotations;
	float m_tickCounter;

	//CACHE
	glm::vec2 m_previousScreenPosition;

	//STATIC
	const static std::map<int, glm::vec3> NORMALS_OF_FACES;
};